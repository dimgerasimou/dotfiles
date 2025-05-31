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

Matrix_t*
matrixinit(const uint32_t rows, const uint32_t cols)
{
	Matrix_t *mat;

	mat = mmalloc(sizeof(Matrix_t));

	mat->rows = rows;
	mat->cols = cols;

	mat->data = mmalloc(rows * cols * sizeof(double));

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

