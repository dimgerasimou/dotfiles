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

Matrix*
matrixinit(const uint32_t rows, const uint32_t cols, const enum mat_type type)
{
	Matrix *mat;

	mat = mmalloc(sizeof(Matrix));

	mat->rows = rows;
	mat->cols = cols;
	mat->type = type;

	switch (mat->type) {
	case DOUBLE:
		mat->type_size = sizeof(double);
		break;

	case SINGLE:
		mat->type_size = sizeof(float);
		break;

	case INT64:
		mat->type_size = sizeof(int64_t);
		break;
	default:
		die("Invalid matrix type");
	}

	mat->data = mmalloc(mat->rows * mat->cols * mat->type_size);

	return mat;
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
neighborarrsave(Neighbor *neighbors, const uint32_t N,
                const uint32_t K, const IOHeader output, const enum mat_type type)
{
	Matrix *idx;
	Matrix *dst;

	idx = matrixinit(N, K, DOUBLE);
	dst = matrixinit(N, K, );

	#pragma omp parallel for
	for (uint32_t i = 0; i < N; i++) {
		for (uint32_t j = 0; j < K; j++) {
			idx->data[j * N + i] = neighbors[(i * K) + j].idx + 1;
			dst->data[j * N + i] = neighbors[(i * K) + j].dst;
		}
	}

	matrixsave(idx, dst, output.path);

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
qselect_qsort(Neighbor *neighbors, int left, int right)
{
	if (left < right) {

		int pivotIndex = qselect_partition(neighbors, left, right);

		qselect_qsort(neighbors, left, pivotIndex - 1);
		qselect_qsort(neighbors, pivotIndex + 1, right);
	}
}
