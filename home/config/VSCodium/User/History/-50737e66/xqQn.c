#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <matio.h>
#include <math.h>
#include <errno.h>
#include <hdf5.h>

const char *type_str[] = {"double", "single", "int64", "int32", "int16"};

typedef enum {
	DOUBLE,
	SINGLE,
	INT64,
	INT32,
	INT16,
} mat_type;

typedef enum  {
	MAT = 1,
	HDF5 = 2,
} io_type;

typedef struct {
	io_type type;
	char    *path;
} IOHeader;

typedef struct {
	void     *data;
	uint32_t rows;
	uint32_t cols;
	mat_type type;
} Matrix_t;

static void
die(const char *message, const char *argument)
{
	if (argument)
		fprintf(stderr, "%s - %s\n", message, argument);
	else
	 	fprintf(stderr, "%s\n", message);

	if (!errno)
		errno = 1;
	exit(errno);
}

void*
mmalloc(size_t size)
{
	void *ptr = NULL;

	if (!size)
		die("malloc() invalid size: 0", NULL);

	ptr = malloc(size);

	if (!ptr)
		die("malloc() failed to allocate memory", strerror(errno));

	return ptr;
}

static void
matrixfree(Matrix_t **mat)
{
	if (!(*mat))
		return;
	if ((*mat)->data)
		free((*mat)->data);
	free(*mat);
	*mat = NULL;
}

static size_t
matrixtypesize(const mat_type type)
{
	switch (type) {
	case DOUBLE:
		return sizeof(double);
		break;

	case SINGLE:
		return sizeof(float);
		break;

	case INT64:
		return sizeof(int64_t);
		break;
	
	case INT32:
		return sizeof(int32_t);
		break;
		
	case INT16:
		return sizeof(int16_t);
		break;
	
	default:
		errno = ENOTSUP;
		die("Invalid matrix type", NULL);
	}
	return 0;
}

static Matrix_t*
matrixinit(const uint32_t rows, const uint32_t cols, const mat_type type)
{
	Matrix_t *mat;

	mat = mmalloc(sizeof(Matrix_t));

	mat->rows = rows;
	mat->cols = cols;
	mat->type = type;

	mat->data = mmalloc(mat->rows * mat->cols * matrixtypesize(mat->type));

	return mat;
}

static Matrix_t*
matiogetmat(const char *filepath, const char *varname)
{
	mat_t    *fp  = NULL;
	matvar_t *var = NULL;
	Matrix_t *ret = NULL;

	if (!(fp = Mat_Open(filepath, MAT_ACC_RDONLY)))
		die("Failed to open mat file", filepath);

	if (!(var = Mat_VarRead(fp, varname)))
		die("Failed to get variable", varname);

	switch (var->data_type) {
	case MAT_T_DOUBLE: {
		ret = matrixinit(var->dims[0], var->dims[1], DOUBLE);
		double *data = (double*) var->data;

		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				((double*)ret->data)[i * var->dims[1] + j] = data[j * var->dims[0] + i];
			}
		}
		break;
	}
	case MAT_T_SINGLE: {
		ret = matrixinit(var->dims[0], var->dims[1], SINGLE);
		float *data = (float*) var->data;

		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				((float*)ret->data)[i * var->dims[1] + j] = data[j * var->dims[0] + i];
			}
		}
		break;
	}
	case MAT_T_INT64: {
		ret = matrixinit(var->dims[0], var->dims[1], INT64);
		int64_t *data = (int64_t*) var->data;

		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				((int64_t*)ret->data)[i * var->dims[1] + j] = data[j * var->dims[0] + i];
			}
		}
		break;
	}
	case MAT_T_INT32: {
		ret = matrixinit(var->dims[0], var->dims[1], INT32);
		int32_t *data = (int32_t*) var->data;

		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				((int32_t*)ret->data)[i * var->dims[1] + j] = data[j * var->dims[0] + i];
			}
		}
		break;
	}
	default:
		die("Unsupported data type for variable", varname);
	}
	
	Mat_VarFree(var);
	Mat_Close(fp);
	return ret;
}

static Matrix_t*
hdf5getmat(const char *filepath, const char *varname)
{
	hid_t file_id, dataset_id, dataspace_id, datatype_id;
	H5T_class_t type_class;
	hsize_t dims[2] = {0, 0};
	herr_t status;
	Matrix_t *ret = NULL;

	if ((file_id = H5Fopen(filepath, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)
		die("Invalid filepath", filepath);

	if ((dataset_id = H5Dopen(file_id, varname, H5P_DEFAULT)) < 0) {
		H5Fclose(file_id);
		die("Invalid filepath", filepath);
	}

	dataspace_id = H5Dget_space(dataset_id);
	H5Sget_simple_extent_dims(dataspace_id, dims, NULL);

	datatype_id = H5Dget_type(dataset_id);
	type_class = H5Tget_class(datatype_id);

	switch (type_class) {
	case H5T_INTEGER:
		if (H5Tget_size(datatype_id) == 8) {
			ret = matrixinit(dims[0], dims[1], INT64);
			status = H5Dread(dataset_id, H5T_NATIVE_LONG, H5S_ALL, H5S_ALL, H5P_DEFAULT, (int64_t *) ret->data);
			if (status < 0)
				die("Error reading hdf5 data", varname);
		} else if (H5Tget_size(datatype_id) == 4) {
			ret = matrixinit(dims[0], dims[1], INT32);
			status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, (int32_t *) ret->data);
			if (status < 0)
				die("Error reading hdf5 data", varname);
		} else if (H5Tget_size(datatype_id) == 2) {
			ret = matrixinit(dims[0], dims[1], INT16);
			status = H5Dread(dataset_id, H5T_NATIVE_SHORT, H5S_ALL, H5S_ALL, H5P_DEFAULT, (int16_t *) ret->data);
			if (status < 0)
				die("Error reading hdf5 data", varname);
		}
		break;
	case H5T_FLOAT:
		if (H5Tget_size(datatype_id) == 4) {
			ret = matrixinit(dims[0], dims[1], SINGLE);
			status = H5Dread(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, (float *) ret->data);
			if (status < 0)
				die("Error reading hdf5 data", varname);
		} else if (H5Tget_size(datatype_id) == 8) {
			ret = matrixinit(dims[0], dims[1], DOUBLE);
			status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (double *) ret->data);
			if (status < 0)
				die("Error reading hdf5 data", varname);
		}
		break;

	case H5T_STRING:
		die("Error reading hdf5 data, variable has type 'string'", varname);
		break;

	default:
		die("Unsupported data type", varname);
	}
	

	H5Tclose(datatype_id);
	H5Sclose(dataspace_id);
	H5Dclose(dataset_id);
	H5Fclose(file_id);
	return ret;
}

static Matrix_t*
matrixget(IOHeader header, const char *varname)
{
	if (header.type == MAT)
		return matiogetmat(header.path, varname);
	if (header.type == HDF5)
		return hdf5getmat(header.path, varname);

	die("Error with variable type", varname);
	return NULL;
}

static int
compareint32(const void *A, const void *B)
{
	return (*(int32_t *)A - *(int32_t *)B);
}

static int
comparesingle(const void *A, const void *B)
{
	float *fA = (float*) A;
	float *fB = (float*) B;

	if (*fA < *fB) return -1;
	if (*fA > *fB) return 1;
	return 0;
}

static int
comparedouble(const void *A, const void *B)
{
	double *dA = (double*) A;
	double *dB = (double*) B;

	if (*dA < *dB) return -1;
	if (*dA > *dB) return 1;
	return 0;
}

static void
comparedistances(IOHeader headerA, IOHeader headerB, const double fpe)
{
	Matrix_t *A, *B;
	uint64_t  mismatched;
	double    inaccuracy;
	double    max_error;
	double    difference;
	double    difference_sum;
	double    difference_sq_sum;
	double    relative_error_sum;


	A = matrixget(headerA, "distances");
	B = matrixget(headerB, "distances");

	printf("distances:\n");

	if (A->type != B->type) {
		printf("data types do not match\n");
		matrixfree(&A);
		matrixfree(&B);
		return;
	}

	if (A->cols != B->cols || A->rows != B->rows) {
		printf("dimensions do not match\n");
		matrixfree(&A);
		matrixfree(&B);
		return;
	}


	inaccuracy = 0.0;
	max_error = 0.0;
	difference_sum = 0.0;
	difference_sq_sum = 0.0;
	relative_error_sum = 0.0;

	switch (A->type) {
	case DOUBLE:{
		double *A_temp = mmalloc(A->cols * sizeof(double));
		double *B_temp = mmalloc(B->cols * sizeof(double));

		for (uint32_t i = 0; i < A->rows; i++) {
			mismatched = 0;
			memcpy(A_temp, &((double*) A->data)[i * A->cols], A->cols * sizeof(double));
			memcpy(B_temp, &((double*) B->data)[i * B->cols], B->cols * sizeof(double));

			qsort(A_temp, A->cols, sizeof(double), comparedouble);
			qsort(B_temp, B->cols, sizeof(double), comparedouble);

			uint32_t aptr = 0, bptr = 0;

			while (aptr < A->cols && bptr < B->cols) {
				if (A_temp[aptr] - B_temp[bptr] < -fpe) {
					mismatched++;
					aptr++;
				} else if (A_temp[aptr] - B_temp[bptr] > fpe) {
					mismatched++;
					bptr++;
				} else {
					aptr++;
					bptr++;
				}
			}

			while (aptr < A->cols) {
				mismatched++;
				aptr++;
			}

			while (bptr < B->cols) {
				mismatched++;
				bptr++;
			}

			mismatched /= 2;
			inaccuracy += (double) mismatched / A->cols;

			for (uint32_t j = 0; j < A->cols; j++) {
				difference = fabs(A_temp[j] - B_temp[j]);
				max_error = difference > max_error ? difference : max_error;
				difference_sum += difference;
				difference_sq_sum += difference * difference;
				relative_error_sum += difference / (A_temp[j] + 1e-10);
			}
		}
		free(A_temp);
		free(B_temp);
		break;
		}

	case SINGLE:
		{
		float *A_temp = mmalloc(A->cols * sizeof(float));
		float *B_temp = mmalloc(B->cols * sizeof(float));

		for (uint32_t i = 0; i < A->rows; i++) {
			mismatched = 0;
			memcpy(A_temp, &((float*) A->data)[i * A->cols], A->cols * sizeof(float));
			memcpy(B_temp, &((float*) B->data)[i * B->cols], B->cols * sizeof(float));

			qsort(A_temp, A->cols, sizeof(float), comparesingle);
			qsort(B_temp, B->cols, sizeof(float), comparesingle);

			uint32_t aptr = 0, bptr = 0;

			while (aptr < A->cols && bptr < B->cols) {
				if (A_temp[aptr] - B_temp[bptr] < -fpe) {
					mismatched++;
					aptr++;
				} else if (A_temp[aptr] - B_temp[bptr] > fpe) {
					mismatched++;
					bptr++;
				} else {
					aptr++;
					bptr++;
				}
			}

			while (aptr < A->cols) {
				mismatched++;
				aptr++;
			}

			while (bptr < B->cols) {
				mismatched++;
				bptr++;
			}

			mismatched /= 2;
			inaccuracy += (double) mismatched / A->cols;

			for (uint32_t j = 0; j < A->cols; j++) {
				difference = (double) fabsf(A_temp[j] - B_temp[j]);
				max_error = difference > max_error ? difference : max_error;
				difference_sum += difference;
				difference_sq_sum += difference * difference;
				if (A_temp[j] != 0)
					relative_error_sum += difference / A_temp[j];
			}
		}
		free(A_temp);
		free(B_temp);
		break;
		}

	default:
		printf("unsupported datatype\n");
		matrixfree(&A);
		matrixfree(&B);
		return;
	}

	printf("\tAccuracy:                %.8lf\n", 1 - inaccuracy / A->rows);
	printf("\tMax error:               %.8lf\n", max_error);
	printf("\tMean Absolute Error:     %.8lf\n", difference_sum / (A->rows * A->cols));
	printf("\tMean Square Error:       %.8lf\n", difference_sq_sum * difference_sum / (A->rows * A->cols));
	printf("\tMean Relative Error:     %.8lf\n", relative_error_sum / (A->rows * A->cols));

	matrixfree(&A);
	matrixfree(&B);

	printf("\n");
}

static double
top_k_overlap(const int32_t *A, const int32_t *B, uint32_t k, const uint32_t n)
{
	int overlap = 0;

	k = n < k ? n : k;

	for (uint32_t i = 0; i < k; i++) {
		for (uint32_t j = 0; j < k; j++) {
			if (A[i] == B[j]) {
				overlap++;
				break;
			}
		}
	}

	return (double) overlap / k;
}

static double
rank_correlation(int32_t *A, int32_t *B, uint32_t n)
{
	int32_t *rank_A = mmalloc(n * sizeof(int32_t));
	int32_t *rank_B = mmalloc(n * sizeof(int32_t));

	memcpy(rank_A, A, n * sizeof(int32_t));
	memcpy(rank_B, B, n * sizeof(int32_t));

	// Compute ranks for A
	for (uint32_t i = 0; i < n; i++) {
		rank_A[i] = i;
	}
	qsort(rank_A, n, sizeof(int32_t), compareint32);

	// Compute ranks for B
	for (uint32_t i = 0; i < n; i++) {
		rank_B[i] = i;
	}
	qsort(rank_B, n, sizeof(int32_t), compareint32);

	// Calculate rank correlation
	double d_squared_sum = 0;
	for (uint32_t i = 0; i < n; i++) {
		int32_t d = rank_A[i] - rank_B[i];
		d_squared_sum += d * d;
	}

	double rho = 1 - (6 * d_squared_sum) / (n * (n * n - 1));

	free(rank_A);
	free(rank_B);

	return rho;
}

static void
compareneighbors(IOHeader headerA, IOHeader headerB)
{
	Matrix_t *A, *B;
	int32_t  *A_temp, *B_temp;
	uint64_t  mismatched;
	double    inaccuracy;
	double    top_k_overlap_sum;
	double    rank_correlation_sum;


	A = matrixget(headerA, "neighbors");
	B = matrixget(headerB, "neighbors");

	printf("neighbors:\n");

	if (A->type != B->type) {
		printf("data types do not match\n");
		matrixfree(&A);
		matrixfree(&B);
		return;
	}

	if (A->type != INT32) {
		printf("unsupported datatype\n");
		matrixfree(&A);
		matrixfree(&B);
		return;
	}

	if (A->cols != B->cols || A->rows != B->rows) {
		printf("dimensions do not match\n");
		matrixfree(&A);
		matrixfree(&B);
		return;
	}

	A_temp = mmalloc(A->cols * sizeof(int32_t));
	B_temp = mmalloc(B->cols * sizeof(int32_t));
	inaccuracy = 0.0;
	top_k_overlap_sum = 0.0;
	rank_correlation_sum = 0.0;

	for (uint32_t i = 0; i < A->rows; i++) {
		mismatched = 0;
		memcpy(A_temp, &((int32_t*) A->data)[i * A->cols], A->cols * sizeof(int32_t));
		memcpy(B_temp, &((int32_t*) B->data)[i * B->cols], B->cols * sizeof(int32_t));

		qsort(A_temp, A->cols, sizeof(int32_t), compareint32);
		qsort(B_temp, B->cols, sizeof(int32_t), compareint32);

		top_k_overlap_sum += top_k_overlap(A_temp, B_temp, 10, A->cols);
		rank_correlation_sum += rank_correlation(A_temp, B_temp, A->cols);

		uint32_t aptr = 0, bptr = 0;

		while (aptr < A->cols && bptr < B->cols) {
			if (A_temp[aptr] < B_temp[bptr]) {
				mismatched++;
				aptr++;
			} else if (A_temp[aptr] > B_temp[bptr]) {
				mismatched++;
				bptr++;
			} else {
				aptr++;
				bptr++;
			}
		}

		while (aptr < A->cols) {
			mismatched++;
			aptr++;
		}

		while (bptr < B->cols) {
			mismatched++;
			bptr++;
		}

		mismatched /= 2;
		inaccuracy += (double) mismatched / A->cols;
	}

	free(A_temp);
	free(B_temp);

	printf("\tAccuracy:      %.8lf\n", 1 - inaccuracy / A->rows);
	printf("\tTop K overlap: %.8lf\n", top_k_overlap_sum / A->rows);

	matrixfree(&A);
	matrixfree(&B);

	printf("\n");
}

static const char*
getextention(const char *path)
{
	const char *slash = strrchr(path, '/');
	const char *filename = (slash) ? slash + 1 : path;

	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) {
		return "";
	}
	return dot + 1;
}

static int
parsefile(const char *path, IOHeader *header)
{
	const char *extention = getextention(path);

	if (!strcmp(extention, "mat"))
		header->type = MAT;
	else if (!strcmp(extention, "h5") || !strcmp(extention, "hdf5"))
		header->type = HDF5;
	else
		return 1;

	header->path = strdup(path);
	return 0;
}

static void
parseargs(int argc, char *argv[], IOHeader *headerA, IOHeader *headerB)
{

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-e")) {
			if (argc < i + 1)
				die("Invalid number of arguments", argv[i]);
			if (parsefile(argv[++i], headerA))
				die("Invalid filetype", argv[i]);
		} else if (!strcmp(argv[i], "-a")) {
			if (argc < i + 1)
				die("Invalid number of arguments", argv[i]);
			if (parsefile(argv[++i], headerB))
				die("Invalid filetype", argv[i]);
		} else {
			errno = EINVAL;
			die("Invalid argument", argv[i]);
		}
	}

	if (!headerA->type || !headerB->type) {
		errno = EINVAL;
		die("Did not specify the input files", NULL);
	}
}

int
main(int argc, char *argv[])
{
	IOHeader headerA = {0, NULL}, headerB = {0, NULL};

	parseargs(argc, argv, &headerA, &headerB);

	compareneighbors(headerA, headerB);
	comparedistances(headerA, headerB, 0.000001);

	return 0;
}
