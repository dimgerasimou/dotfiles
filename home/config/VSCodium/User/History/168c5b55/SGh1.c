#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <omp.h>

#include "knnsearch.h"

/* global functions */
void
die(const char *message)
{
	fprintf(stderr, "%s\n", message);
	exit(1);
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

void
matrixfree(Matrix_t **mat)
{
	if (!(*mat))
		return;
	if ((*mat)->data)
		free((*mat)->data);
	free(*mat);
	*mat = NULL;
}

size_t
matrixtypesize(const enum mat_type type)
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
		die("Invalid matrix type");
	}
	return 0;
}

Matrix_t*
matrixinit(const uint32_t rows, const uint32_t cols, const enum mat_type type)
{
	Matrix_t *mat;

	mat = mmalloc(sizeof(Matrix_t));

	mat->rows = rows;
	mat->cols = cols;
	mat->type = type;

	mat->data = mmalloc(mat->rows * mat->cols * matrixtypesize(mat->type));

	return mat;
}

void
matrixtodouble(Matrix_t *mat)
{
	if (mat->type == DOUBLE)
		return;

	double *temp = mmalloc(mat->rows * mat->cols * sizeof(double));
	switch (mat->type) {
	case SINGLE:
		#pragma omp parallel for
		for (uint32_t i = 0; i < mat->rows * mat->cols; i++)
			temp[i] = (double) ((float*) mat->data)[i];
		break;

	case INT64:
		#pragma omp parallel for
		for (uint32_t i = 0; i < mat->rows * mat->cols; i++)
			temp[i] = (double) ((int64_t*) mat->data)[i];
		break;
	
	case INT32:
		#pragma omp parallel for
		for (uint32_t i = 0; i < mat->rows * mat->cols; i++)
			temp[i] = (double) ((int32_t*) mat->data)[i];
		break;
		
	case INT16:
		#pragma omp parallel for
		for (uint32_t i = 0; i < mat->rows * mat->cols; i++)
			temp[i] = (double) ((int16_t*) mat->data)[i];
		break;
	
	default:
		die("Invalid matrix type");
	}
	mat->type = DOUBLE;
	free(mat->data);
	mat->data = temp;
}

void
matrixtosingle(Matrix_t *mat)
{
	if (mat->type == SINGLE)
		return;

	float *temp = mmalloc(mat->rows * mat->cols * sizeof(float));
	switch (mat->type) {
	case DOUBLE:
		#pragma omp parallel for
		for (uint32_t i = 0; i < mat->rows * mat->cols; i++)
			temp[i] = (float) ((double*) mat->data)[i];
		break;

	case INT64:
		#pragma omp parallel for
		for (uint32_t i = 0; i < mat->rows * mat->cols; i++)
			temp[i] = (float) ((int64_t*) mat->data)[i];
		break;
	
	case INT32:
		#pragma omp parallel for
		for (uint32_t i = 0; i < mat->rows * mat->cols; i++)
			temp[i] = (float) ((int32_t*) mat->data)[i];
		break;
		
	case INT16:
		#pragma omp parallel for
		for (uint32_t i = 0; i < mat->rows * mat->cols; i++)
			temp[i] = (float) ((int16_t*) mat->data)[i];
		break;
	
	default:
		die("Invalid matrix type");
	}
	mat->type = SINGLE;
	free(mat->data);
	mat->data = temp;
}

void*
mmalloc(size_t size)
{
	void *ptr = NULL;

	ptr = malloc(size);
	if (!ptr) {
		fprintf(stderr, "malloc() failed with - %s\n", strerror(errno));
		exit(errno);
	}

	return ptr;
}
/*
void
neighborarrsave(Neighbor *neighbors, const uint32_t N,
                const uint32_t K, const char *filepath)
{
	Matrix_t *idx;
	Matrix_t *dst;

	idx = matrixinit(N, K);
	dst = matrixinit(N, K);

	#pragma omp parallel for
	for (uint32_t i = 0; i < N; i++) {
		for (uint32_t j = 0; j < K; j++) {
			idx->data[j * N + i] = neighbors[(i * K) + j].idx + 1;
			dst->data[j * N + i] = neighbors[(i * K) + j].dst;
		}
	}

	matrixsave(idx, dst, filepath);

	matrixfree(&idx);
	matrixfree(&dst);
}
*/
