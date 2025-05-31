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

	D_M = mmalloc(M * N * sizeof(double));
	cblas_dgemm(CblasColMajor, CblasNoTrans, CblasTrans, M, N, dims, -2.0,
	            C, M, Q, N, 0.0, D_M, M);

	for (uint32_t i = 0; i < M; i++) {
		for (uint32_t j = 0; j < N; j++) {
			double cs = 0.0, qs = 0.0;

			for (uint32_t k = 0; k < dims; k++) {
				cs += C[k * M + i] * C[k * M + i];
				qs += Q[k * N + j] * Q[k * N + j];
			}

			D[i * N + j] = sqrt(fabs(cs + D_M[j * M + i] + qs));
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
		printf("Q and C dimensions are not the same\n");
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

		qselect(neighbors_row, 0, Q->rows - 1, *K);
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
