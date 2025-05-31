#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <omp.h>

#include "knnsearch.h"

/* file functions */
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

static int
qsort_partition(Neighbors *neighbors, int left, int right)
{
	double pivot; = neighbors[right].dst;
	int    i     = left;

	if (neighbors->type == SINGLE)
		pivot = ((float*) neighbors->dst)[right];
	else
		pivot = ((double*) neighbors->dst)[right];

	for (int j = left; j < right; j++) {
		if (neighbors[j].dst < pivot) {
			qselect_swap(&neighbors[i], &neighbors[j]);
			i++;
		}
	}

	qselect_swap(&neighbors[i], &neighbors[right]);
	return i;
}

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
matrixfree(Matrix **mat)
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

Matrix*
matrixinit(const uint32_t rows, const uint32_t cols, const enum mat_type type)
{
	Matrix *mat;

	mat = mmalloc(sizeof(Matrix));

	mat->rows = rows;
	mat->cols = cols;
	mat->type = type;

	mat->data = mmalloc(mat->rows * mat->cols * matrixtypesize(mat->type_size));

	return mat;
}

void
matrixtodouble(Matrix *mat)
{
	if (mat->type == DOUBLE)
		return;

	float *temp = mmalloc(mat->rows * mat->cols * sizeof(double));
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
	mat->type_size = sizeof(double);
	free(mat->data);
	mat->data = temp;
}

void
matrixtosingle(Matrix *mat)
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
	mat->type_size = sizeof(float);
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

void
neighborsfree(Neighbors **neighbors)
{
	if (!*neighbors)
		return;
	if ((*neighbors)->dst)
		free((*neighbors)->dst);
	if ((*neighbors)->idx)
		free((*neighbors)->idx);
	free(*neighbors);
	*neighbors = NULL;
}

Neighbors*
neighborsinit(const uint32_t N, const uint32_t K, const enum mat_type type)
{
	Neighbors *ret = mmalloc(N * K * sizeof(Neighbors));

	ret->rows = N;
	ret->cols = K;
	ret->type = type;

	ret->idx = mmalloc(N * K * matrixtypesize(INT32));
	ret->idx = mmalloc(N * K * matrixtypesize(type));

	return ret;
}

void
neighborssave(Neighbors *neighbors, const IOHeader output)
{
	Matrix *idx;
	Matrix *dst;

	idx = matrixinit(neighbors->rows, neighbors->cols, INT32);
	dst = matrixinit(neighbors->rows, neighbors->cols, neighbors->type);

	if (output.type == MAT) {
		if (neighbors->type == DOUBLE) {
			#pragma omp parallel for
			for (uint32_t i = 0; i < neighbors->rows; i++) {
				for (uint32_t j = 0; j < neighbors->cols; j++) {
					((int32_t*)idx->data)[j * neighbors->rows + i] = ((int32_t*) neighbors->idx)[(i * neighbors->cols) + j] + 1;
					((double*)dst->data)[j * neighbors->rows + i] = ((double*) neighbors->dst)[(i * neighbors->cols) + j];
				}
			}
		} else if (neighbors->type == SINGLE) {
			#pragma omp parallel for
			for (uint32_t i = 0; i < neighbors->rows; i++) {
				for (uint32_t j = 0; j < neighbors->cols; j++) {
					((int32_t*)idx->data)[j * neighbors->rows + i] = ((int32_t*) neighbors->idx)[(i * neighbors->cols) + j] + 1;
					((float*)dst->data)[j * neighbors->rows + i] = ((float*) neighbors->dst)[(i * neighbors->cols) + j];
				}
			}
		} else {
			die("Wrong filetype");
		}

		matrixsave(idx, dst, output.path);
	}

	matrixfree(&idx);
	matrixfree(&dst);
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

void
qSort(Neighbors *neighbors, int left, int right)
{
	if (neighbors->type == SINGLE)
		qsort_single(neighbors, left, right);
	else if (neighbors->type == DOUBLE)
		qsort_double(neighbors, left, right);
	if (left < right) {

		int pivotIndex = qsort_partition(neighbors, left, right);

		qSort(neighbors, left, pivotIndex - 1);
		qSort(neighbors, pivotIndex + 1, right);
	}
}
