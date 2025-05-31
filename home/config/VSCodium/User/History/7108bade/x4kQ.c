#include <math.h>
#include <matio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <openblas/cblas.h>

/* global variables */
uint16_t g_blocksize = 256;

/* struct definitions*/
typedef struct {
	uint32_t idx;
	double   dst;
} Neighbor;

typedef struct {
	double   *data;
	uint32_t rows;
	uint32_t cols;
} Matrix;

/* function definitions */
void       getdistance(const double *C, const double *Q, double *D, double *D_M, const uint32_t M, const uint32_t N, const uint32_t dims);
int        isuint(char *s);
Neighbor** knnsearch(double *C, double *Q, const uint32_t M, const uint32_t N, const uint32_t dims, const uint32_t K);
Matrix*    matiogetmat(const char *filepath, const char *varname);
uint32_t   matiogetuint(const char *filepath, const char *varname);
void       matrixfree(Matrix **mat);
Matrix*    matrixinit(const uint32_t rows, const uint32_t cols, double *data, int dataColumnMajor);
void       matrixprint(Matrix *mat, const char *name);
int        matrixsave(Matrix *idx, Matrix *dst, const char *filepath);
void       neighborarrsave(Neighbor **neighbors, const uint32_t N, const uint32_t K, const char *filepath);
void       neighborarrfree(Neighbor ***neighbors, const uint32_t N);
Neighbor** neighborsinit(const uint32_t N, const uint32_t K);
void       qselect(Neighbor *neighbors, int left, int right, int K);
int        qselect_partition(Neighbor *neighbors, int left, int right);
void       qselect_swap(Neighbor *a, Neighbor *b);
void       qselect_qsort(Neighbor *neighbors, int left, int right);

void
getdistance(const double *C, const double *Q, double *D, double *D_M, const uint32_t M,
            const uint32_t N, const uint32_t dims)
{
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, M, N, dims, -2.0,
	            C, dims, Q, dims, 0.0, D_M, N);

	for (uint32_t i = 0; i < M; i++) {
		for (uint32_t j = 0; j < N; j++) {
			double cs = 0.0, qs = 0.0;
			for (uint32_t k = 0; k < dims; k++) {
				cs += C[i * dims + k] * C[i * dims + k];
				qs += Q[j * dims + k] * Q[j * dims + k];
			}
			D[j * M + i] = sqrt(fabs(cs + D_M[i * N + j] + qs));
		}
	}
}

int
isuint(char *s)
{
	for (char *ptr = s; *ptr != '\0'; ptr++) {
		if (*ptr > '9' || *ptr < '0')
			return 0;
	}

	if (!strcmp(s, "0"))
		return 0;

	return 1;
}

Neighbor**
knnsearch(double *C, double *Q, const uint32_t M, const uint32_t N,
          const uint32_t dims, const uint32_t K)
{
	Neighbor **neighbors;
	Neighbor *neighbors_row;
	double   *Dblock;
	double   *Dmult;

	neighbors = neighborsinit(N, K);

	// Blockwise allocation of distance matrix
	if (!(Dblock = malloc(g_blocksize * g_blocksize * sizeof(double)))) {
		fprintf(stderr, "Failed to allocate Dblock\n");
		exit(5);
	}

	if (!(Dmult = malloc(g_blocksize * g_blocksize * sizeof(double)))) {
		fprintf(stderr, "Failed to allocate Dmult\n");
		exit(5);
	}

	if (!(neighbors_row = malloc((g_blocksize + K) * sizeof(Neighbor)))) {
		fprintf(stderr, "Failed to allocate neighbors_row\n");
		exit(5);
	}

	// Main loop over blocks in C (rows) and Q (columns)
	for (uint32_t i = 0; i < M; i += g_blocksize) {
		uint32_t Mblocksize = (i + g_blocksize > M) ? (M - i) : g_blocksize;

		for (uint32_t j = 0; j < N; j += g_blocksize) {
			uint32_t Nblocksize = (j + g_blocksize > N) ? (N - j) : g_blocksize;

			// Compute the distance matrix block
			getdistance(&C[j * dims], &Q[i * dims], Dblock, Dmult, Nblocksize, Mblocksize, dims);

			for (uint32_t ii = 0; ii < Mblocksize; ii++) {
				uint32_t rowidx = i + ii;
				uint32_t coloff = (j < K) ? j : K;

				for (uint32_t jj = 0; jj < Nblocksize; jj++) {
					neighbors_row[coloff + jj].idx = j + jj;
					neighbors_row[coloff + jj].dst = Dblock[jj + ii * Nblocksize];
				}

				for (uint32_t jj = 0; jj < coloff; jj++)
					neighbors_row[jj] = neighbors[rowidx][jj];

				qselect(neighbors_row, 0, coloff + Nblocksize - 1, K);

				for (uint32_t idx = 0; idx < K; idx++) 
					neighbors[rowidx][idx] = neighbors_row[idx];
			}
		}
	}

	for (uint32_t i = 0; i < N; i++)
		qselect_qsort(neighbors[i], 0, K - 1);

	free(Dblock);
	free(Dmult);
	free(neighbors_row);

	return neighbors;
}

Matrix*
matiogetmat(const char *filepath, const char *varname)
{
	mat_t    *fp  = NULL;
	matvar_t *var = NULL;
	Matrix   *ret;

	if (!(fp = Mat_Open(filepath, MAT_ACC_RDONLY))) {
		fprintf(stderr, "Couldn't open '%s' .mat file\n", filepath);
		exit(1);
	}

	var = Mat_VarRead(fp, varname);
	if (!var || var->data_type != MAT_T_DOUBLE) {
		fprintf(stderr, "Couldn't get variable '%s'\n", varname);
		exit(1);
	}

	ret = matrixinit(var->dims[0], var->dims[1], (double*) var->data, 1);
	
	Mat_VarFree(var);
	Mat_Close(fp);
	return ret;
}

uint32_t
matiogetuint(const char *filepath, const char *varname) {
	mat_t    *fp  = NULL;
	matvar_t *var = NULL;
	int      result = 0;


	if (!(fp = Mat_Open(filepath, MAT_ACC_RDONLY))) {
		fprintf(stderr, "Couldn't open '%s' .mat file\n", filepath);
		exit(1);
	}

	var = Mat_VarRead(fp, varname);
	if (!var) {
		fprintf(stderr, "Couldn't get variable '%s'\n", varname);
		exit(1);
	}

	if (var->data_type == MAT_T_INT32) {
		result = ((int32_t *)var->data)[0];
	} else if (var->data_type == MAT_T_INT16) {
		result = ((int16_t *)var->data)[0];
	} else {
		fprintf(stderr, "Varabile '%s', wrong filetype\n", varname);
		exit(1);
	}

	Mat_VarFree(var);
	Mat_Close(fp);
	return result;
}

void
matrixfree(Matrix **mat)
{
	if (!(*mat))
		return;
	if ((*mat)->data)
		free((*mat)->data);
	free(*mat);
	*mat = NULL;
}

Matrix*
matrixinit(const uint32_t rows, const uint32_t cols,
           double *data, int dataColumnMajor)
{
	Matrix *mat;

	if (!(mat = malloc(sizeof(Matrix)))) {
		fprintf(stderr, "malloc() failed\n");
		exit(5);
	}

	mat->rows = rows;
	mat->cols = cols;

	if (!(mat->data = malloc(rows * cols * sizeof(double)))) {
		fprintf(stderr, "malloc() failed\n");
		exit(5);
	}

	if (data) {
		if (dataColumnMajor) {
			for (uint32_t i = 0; i < rows; i++) {
				for (uint32_t j = 0; j < cols; j++)
					mat->data[j + cols * i] = data[i + rows * j];
			}
		} else {
			for (uint32_t i = 0; i < rows; i++) {
				for (uint32_t j = 0; j < cols; j++)
					mat->data[j + cols * i] = data[j + cols * i];
			}
		}
	}

	return mat;
}

void
matrixprint(Matrix *mat, const char *name)
{
	printf("%s:\n %dx%d\n", name, mat->rows, mat->cols);
	for (uint32_t i = 0; i < mat->rows; i++) {
		for (uint32_t j = 0; j < mat->cols; j++) {
			printf("%10lf ", mat->data[i * mat->cols + j]);
		}
		printf("\n");
	}
	printf("\n");
}

int
matrixsave(Matrix *idx, Matrix *dst, const char *filepath)
{
	mat_t    *fp   = NULL;
	matvar_t *var  = NULL;
	double   *data = NULL;

	const size_t dims[2] = { idx->rows, idx->cols };

	if (!(fp = Mat_CreateVer(filepath, NULL, MAT_FT_MAT5))) {
		fprintf(stderr, "Error creating '%s' .mat variable\n", filepath);
		return 1;
	}

	if (!(data = malloc(dims[0] * dims[1] * sizeof(double)))) {
		fprintf(stderr, "malloc() failed\n");
		exit(5);
	}

	for (uint32_t i = 0; i < dims[0]; i++) {
		for (uint32_t j = 0; j < dims[1]; j++)
			data[i + dims[0] * j] = idx->data[j + dims[1] * i];
	}

	var = Mat_VarCreate("idx", MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, data, MAT_F_DONT_COPY_DATA);

	if (!var) {
		fprintf(stderr, "Error creating the variable 'idx'\n");
		return 1;
	}

	if (Mat_VarWrite(fp, var, MAT_COMPRESSION_NONE) != 0) {
		fprintf(stderr, "Error writing the variable 'idx' to the file '%s'\n", filepath);
		Mat_VarFree(var);
		Mat_Close(fp);
		free(data);
		return 1;
	}

	Mat_VarFree(var);

	for (uint32_t i = 0; i < dims[0]; i++) {
		for (uint32_t j = 0; j < dims[1]; j++)
			data[i + dims[0] * j] = dst->data[j + dims[1] * i];
	}

	var = Mat_VarCreate("dst", MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, data, MAT_F_DONT_COPY_DATA);
	if (!var) {
		fprintf(stderr, "Error creating the variable 'dst'\n");
		return 1;
	}

	if (Mat_VarWrite(fp, var, MAT_COMPRESSION_NONE) != 0) {
		fprintf(stderr, "Error writing the variable 'dst' to the file '%s'\n", filepath);
		Mat_VarFree(var);
		Mat_Close(fp);
		free(data);
		return 1;
	}

	Mat_VarFree(var);
	Mat_Close(fp);
	free(data);
	return 0;
}

void
neighborarrsave(Neighbor **neighbors, const uint32_t N,
                const uint32_t K, const char *filepath)
{
	Matrix *idx;
	Matrix *dst;

	idx = matrixinit(N, K, NULL, 0);
	dst = matrixinit(N, K, NULL, 0);

	for (uint32_t i = 0; i < N; i++) {
		for (uint32_t j = 0; j < K; j++) {
			idx->data[i * K + j] = neighbors[i][j].idx + 1;
			dst->data[i * K + j] = neighbors[i][j].dst;
		}
	}

	matrixsave(idx, dst, filepath);

	matrixfree(&idx);
	matrixfree(&dst);
}

void
neighborarrfree(Neighbor ***neighbors, const uint32_t N)
{
	if (!(*neighbors))
		return;

	for (uint32_t i = 0; i < N; i++) {
		if ((*neighbors)[i])
			free((*neighbors)[i]);
	}

	free(*neighbors);
	*neighbors = NULL;
}

Neighbor**
neighborsinit(const uint32_t N, const uint32_t K)
{
	Neighbor **ret = NULL;

	if (!(ret = malloc(N * sizeof(Neighbor*)))) {
		fprintf(stderr, "malloc() failed\n");
		exit(5);
	}

	for (uint32_t i = 0; i < N; i++) {
		if (!(ret[i] = malloc(K * sizeof(Neighbor)))) {
			fprintf(stderr, "malloc() failed\n");
			exit(5);
		}

	}
	
	return ret;
}

void
qselect(Neighbor *neighbors, int left, int right, int K)
{
	if (left < right) {
		int pivotIndex = qselect_partition(neighbors, left, right);

		if (pivotIndex == K)
			return;
		else if (pivotIndex < K)
			qselect(neighbors, pivotIndex + 1, right, K);
		else
			qselect(neighbors, left, pivotIndex - 1, K);
	}
}

int
qselect_partition(Neighbor *neighbors, int left, int right)
{
	double pivot = neighbors[right].dst;
	int    i     = left;

	for (int j = left; j < right; j++) {
		if (neighbors[j].dst < pivot) {
			qselect_swap(&neighbors[i], &neighbors[j]);
			i++;
		}
	}

	qselect_swap(&neighbors[i], &neighbors[right]);
	return i;
}

void
qselect_swap(Neighbor *a, Neighbor *b)
{
	Neighbor temp = *a;
	*a = *b;
	*b = temp;
}

void
qselect_qsort(Neighbor *neighbors, int left, int right)
{
	if (left < right) {

		int pivotIndex = qselect_partition(neighbors, left, right);

		qselect_qsort(neighbors, left, pivotIndex - 1);
		qselect_qsort(neighbors, pivotIndex + 1, right);
	}
}

int
main(int argc, char *argv[])
{
	struct timeval t0, t1;
	Neighbor **neighbors;
	uint32_t K;
	Matrix   *C;
	Matrix   *Q;

	if (argc < 3) {
		fprintf(stderr, "Invalid number of arguments\n");
		exit(1);
	}

	if (argc > 3 && isuint(argv[3]))
		g_blocksize = atoi(argv[3]);

	K = matiogetuint(argv[1], "K");
	C = matiogetmat(argv[1], "C");
	Q = matiogetmat(argv[1], "Q");

	if (Q->rows != C->rows) {
		printf("Q and S dimensions are not the same\n");
		matrixfree(&C);
		matrixfree(&Q);
		exit(0);
	}

	gettimeofday(&t0, NULL);

	// benchmarked function here
	K = Q->rows < K ? Q->rows : K;
	neighbors = knnsearch(C->data, Q->data, C->rows, Q->rows, C->cols, K);

	gettimeofday(&t1, NULL);
	printf("%.8g\n", t1.tv_sec - t0.tv_sec + 1E-6 * (t1.tv_usec - t0.tv_usec));

	neighborarrsave(neighbors, Q->rows, K, argv[2]);
	neighborarrfree(&neighbors, Q->rows);

	matrixfree(&C);
	matrixfree(&Q);
	return 0;
}
