#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <openblas/cblas.h>
#include <omp.h>

#include "knnsearch.h"

typedef struct {
	float    dst;
	uint32_t idx;
} Neighbor;

static void qselect(Neighbor *neighbors, int left, int right, int K);
static void qselect_qsort(Neighbor *neighbors, int left, int right);
static void qselect_swap(Neighbor *a, Neighbor *b);
/*
 * dynamically adjusts the blocksize to a power of 2, based on the cache size:
 * total memory = (2 * g_blocksize * cols + 2 * g_blocksize^2 + 2 * g_blocksize) * sizeof(double).
 *
 * For over 2^27 bytes in the D matrix returns a g_blocksize, where total memory exceedes
 * the L2 cache, even though of the increased probability of cache misses, the penalty on average is less
 * than the cost of reducing the iterations by half. For bytes less than 2^27, g_blocksize
 * is set that the total memory usage of each thread is less than the L2 cache.
*/
static uint32_t
dynamic_blocksize(size_t element_size, uint32_t cols, uint64_t nelem)
{
	long cache_size;
	double max_blocksize;

	cache_size = sysconf(_SC_LEVEL2_CACHE_SIZE);
	if (cache_size == -1) cache_size = 512 * 1024;  // default to 512 KiB if unknown

	max_blocksize = (- (double) (cols + 1) + sqrt((double) (cols + 1) * (cols + 1) + (double) (2 * cache_size) / element_size)) / 2.0;

	// round to nearest power of 2 for block alignment
	uint16_t ret = 1;
	while (ret < (int) max_blocksize) ret *= 2;

	if (nelem < (1 << 27) / element_size)
		ret /= 2;

	return ret > 32 ? ret : 32;  // return 32 as a safe fallback
}

/*
 * calculates the norms of the given matrix
 */
static float*
getnorms(Matrix_t *mat)
{
	float *norms;

	norms = mmalloc(mat->rows * sizeof(float));

	#pragma omp parallel for
	for (uint32_t i = 0; i < mat->rows; i++)
		norms[i] = cblas_sdot(mat->cols, &((float *) mat->data)[i * mat->cols], 1, &((float *) mat->data)[i * mat->cols], 1);

	return norms;
}

/*
 * using cblas gets the euclidian distance of the vectors of the first matrix (rows)
 * compared with the vectors of the second (rows) and returns in a matrix.
 */
static void
getdistance(const float *C, const float *Q, const Matrix_t *D, float *D_M, float *Cnorm, float *Qnorm, const uint32_t cols)
{
	cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasTrans, D->cols, D->rows, cols, -2.0,
	            C, cols, Q, cols, 0.0, D_M, D->rows);

	for (uint32_t i = 0; i < D->rows; i++) {
		for (uint32_t j = 0; j < D->cols; j++) {
			((float*) D->data)[i * D->cols + j] = Cnorm[j] + D_M[j * D->rows + i] + Qnorm[i];
		}
	}
}

Matrix_t**
knnsearchsingle(Matrix_t *C, Matrix_t *Q, uint32_t *K, uint32_t blocksize)
{
	uint16_t nthreads;
	Neighbor *neighbors, **neighbors_b;
	float   *Cnorm, *Qnorm, **D_M_b;
	Matrix_t **D_b;
	Matrix_t **ret;

	matrixtosingle(Q);
	matrixtosingle(C);

	if (!blocksize)
		blocksize = dynamic_blocksize(sizeof(float), C->cols, C->rows * Q->rows);

	if (Q->cols != C->cols) {
		printf("Dimensions are not the same\n");
		matrixfree(&C);
		matrixfree(&Q);
		exit(0);
	}

	*K = C->rows < *K ? C->rows : *K;

	neighbors   = mmalloc(Q->rows * *K * sizeof(Neighbor));

	nthreads    = omp_get_max_threads();
	D_b         = mmalloc(nthreads * sizeof(Matrix_t*));
	D_M_b       = mmalloc(nthreads * sizeof(float*));
	neighbors_b = mmalloc(nthreads * sizeof(Neighbor*));

	Cnorm = getnorms(C);
	Qnorm = getnorms(Q);

	#pragma omp parallel
	{
		uint16_t tID = omp_get_thread_num();

		D_b[tID] =  matrixinit(blocksize, blocksize, SINGLE);
		neighbors_b[tID] = mmalloc((blocksize + *K) * sizeof(Neighbor));
		D_M_b[tID] = mmalloc(blocksize * blocksize * sizeof(float));

		#pragma omp for
		for (uint32_t i = 0; i < Q->rows; i += blocksize) {
			Matrix_t *D = D_b[tID];
			Neighbor *neighbors_row = neighbors_b[tID];
			float *D_M = D_M_b[tID];
			
			D->rows = (i + blocksize > Q->rows) ? (Q->rows - i) : blocksize;

			for (uint32_t j = 0; j < C->rows; j += blocksize) {
				D->cols = (j + blocksize > C->rows) ? (C->rows - j) : blocksize;
				getdistance((float*) C->data + j * C->cols, (float*) Q->data + i * Q->cols, D, D_M, &Cnorm[j], &Qnorm[i], C->cols);

				uint32_t coloffset = j < *K ? j : *K;
				for (uint32_t ii = 0; ii < D->rows; ii++) {
					for (uint32_t jj = 0; jj < D->cols; jj++) {
						neighbors_row[coloffset + jj].idx = j + jj;
						neighbors_row[coloffset + jj].dst = ((float*) D->data)[jj + ii * D->cols];
					}

					for (uint32_t jj = 0; jj < coloffset; jj++)
						neighbors_row[jj] = neighbors[((ii + i) * *K) + jj];

					qselect(neighbors_row, 0, coloffset + D->cols - 1, *K);

					for (uint32_t idx = 0; idx < *K; idx++) 
						neighbors[((ii + i) * *K) + idx] = neighbors_row[idx];
				}
			}
		}
	}

	for (int i = 0; i < nthreads; i++) {
		free(neighbors_b[i]);
		matrixfree(&D_b[i]);
		free(D_M_b[i]);
	}

	free(neighbors_b);
	free(D_b);
	free(D_M_b);
	free(Cnorm);
	free(Qnorm);

	ret = mmalloc(2 * sizeof(Matrix_t*));
	ret[0] = matrixinit(Q->rows, *K, INT32);
	ret[1] = matrixinit(Q->rows, *K, SINGLE);

	int32_t *idx_temp = (int32_t*) ret[0]->data;
	float   *dst_temp = (float*) ret[1]->data;

	#pragma omp parallel for
	for (uint32_t i = 0; i < Q->rows; i++) {
		qselect_qsort(neighbors, i * *K, (i * *K) + *K - 1);

		for (uint32_t j = 0; j < *K; j++) {
			idx_temp[(i * *K) + j] = neighbors[(i * *K) + j].idx;
			dst_temp[(i * *K) + j] = neighbors[(i * *K) + j].dst < 0 ? 0 : sqrt(neighbors[(i * *K) + j].dst);
		}
	}

	free(neighbors);

	return ret;
}

static void
qselect_swap(Neighbor *a, Neighbor *b)
{
	Neighbor temp = *a;
	*a = *b;
	*b = temp;
}

static int
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

static void
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

static void
qselect_qsort(Neighbor *neighbors, int left, int right)
{
	if (left < right) {

		int pivotIndex = qselect_partition(neighbors, left, right);

		qselect_qsort(neighbors, left, pivotIndex - 1);
		qselect_qsort(neighbors, pivotIndex + 1, right);
	}
}
