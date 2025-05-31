#include <stdio.h>
#include <string.h>
#include <errno.h>

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
matrixinit(const uint32_t rows, const uint32_t cols)
{
	Matrix *mat;

	mat = mmalloc(sizeof(Matrix));

	mat->rows = rows;
	mat->cols = cols;

	mat->data = mmalloc(rows * cols * sizeof(double));

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
                const uint32_t K, const char *filepath)
{
	Matrix *idx;
	Matrix *dst;

	idx = matrixinit(N, K);
	dst = matrixinit(N, K);

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
