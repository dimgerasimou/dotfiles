#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <openblas/cblas.h>

#include "knnsearch.h"

void
getdistance(const Matrix *C, const Matrix *Q, const Matrix *D)
{
	double *D_M;

	D_M = mmalloc(C->rows * Q->rows * sizeof(double));
	cblas_dgemm(CblasColMajor, CblasNoTrans, CblasTrans, C->rows, Q->rows, C->cols, -2.0,
	            C->data, C->rows, Q->data, Q->rows, 0.0, D_M, C->rows);

	for (uint32_t i = 0; i < C->rows; i++) {
		for (uint32_t j = 0; j < Q->rows; j++) {
			double cs = 0.0, qs = 0.0;

			for (uint32_t k = 0; k < C->cols; k++) {
				cs += C->data[k * C->rows + i] * C->data[k * C->rows + i];
				qs += Q->data[k * Q->rows + j] * Q->data[k * Q->rows + j];
			}

			D->data[i * Q->rows + j] = sqrt(fabs(cs + D_M[j * C->rows + i] + qs));
		}
	}
	free(D_M);
}

Neighbor**
knnsearch(Matrix *C, Matrix *Q, uint32_t *K)
{
	Neighbor **neighbors;
	Neighbor *neighbors_row;
	Matrix   *D;

	if (Q->cols != C->cols) {
		printf("Dimensions are not the same\n");
		matrixfree(&C);
		matrixfree(&Q);
		exit(0);
	}

	*K = C->rows < *K ? C->rows : *K;

	neighbors = neighborsinit(Q->rows, *K);
	D = matrixinit(Q->rows, C->rows, NULL);
	neighbors_row = mmalloc(C->rows * sizeof(Neighbor));

	// Compute the distance matrix
	getdistance(C, Q, D);

	for (uint32_t i = 0; i < Q->rows; i++) {
		for (uint32_t j = 0; j < C->rows; j++) {
			neighbors_row[j].idx = j;
			neighbors_row[j].dst = D->data[j * Q->rows + i];
		}

		qselect(neighbors_row, 0, C->rows - 1, *K);
		qselect_qsort(neighbors_row, 0, *K - 1);

		for (uint32_t idx = 0; idx < *K; idx++) 
			neighbors[i][idx] = neighbors_row[idx];
	}

	matrixfree(&D);
	free(neighbors_row);

	return neighbors;
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

	K = matiogetuint(argv[1], "K");
	C = matiogetmat(argv[1], "C");
	Q = matiogetmat(argv[1], "Q");

	gettimeofday(&t0, NULL);

	// benchmarked function here
	neighbors = knnsearch(C, Q,&K);

	gettimeofday(&t1, NULL);
	printf("%.8g\n", t1.tv_sec - t0.tv_sec + 1E-6 * (t1.tv_usec - t0.tv_usec));

	neighborarrsave(neighbors, Q->rows, K, argv[2]);
	neighborarrfree(&neighbors, Q->rows);

	matrixfree(&C);
	matrixfree(&Q);
	return 0;
}
