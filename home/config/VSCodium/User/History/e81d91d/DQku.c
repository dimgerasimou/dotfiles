#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <openblas/cblas.h>

#include "knnsearch.h"

void
getdistance(const double *C, const double *Q, double *D, double *D_M, const uint32_t M,
            const uint32_t N, const uint32_t dims)
{
	cblas_dgemm(CblasColMajor, CblasNoTrans, CblasTrans, M, N, dims, -2.0,
	            C, dims, Q, dims, 0.0, D_M, N);

	for (uint32_t i = 0; i < N; i++) {
		for (uint32_t j = 0; j < M; j++) {
			double cs = 0.0, qs = 0.0;
			for (uint32_t k = 0; k < dims; k++) {
				cs += C[i * N + k] * C[i * N + k];
				qs += Q[j * M + k] * Q[j * M + k];
			}
			D[j * N + i] = sqrt(fabs(cs + D_M[i * M + j] + qs));
		}
	}
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

	Dblock = calloc(M*N, sizeof(double));
	Dmult = calloc(M*N, sizeof(double));
	neighbors_row = mmalloc(M * sizeof(Neighbor));

	// Compute the distance matrix block
	//getdistance(C, Q, Dblock, Dmult, M, N, dims);

	printf("linear Q\n");
	for (uint i = 0; i < N * dims; i++) {
		printf("%12lf ", Q[i]);
	}
	printf("\n\n");

	printf("linear C\n");
	for (uint i = 0; i < M * dims; i++) {
		printf("%12lf ", C[i]);
	}
	printf("\n\n");

	printf("linear D\n");
	for (uint i = 0; i < M * N; i++) {
		printf("%12lf ", Dblock[i]);
	}
	printf("\n\n");

	for (uint i = 0; i < N; i++) {
		for (uint j = 0; j < M; j++) {
			printf("%12lf ", Dblock[i * N + j]);
		}
		printf("\n");
	}
	printf("\n");

	for (uint i = 0; i < N; i++) {
		for (uint j = 0; j < M; j++) {
			printf("%12lf ", Dmult[i * N + j]);
		}
		printf("\n");
	}
	printf("\n");

	exit(0);


	for (uint32_t i = 0; i < M; i++) {
		for (uint32_t j = 0; j < N; j++) {
			neighbors_row[j].idx = j;
			neighbors_row[j].dst = Dblock[j + i * N];
		}

		qselect(neighbors_row, 0, N - 1, K);
		qselect_qsort(neighbors_row, 0, K - 1);

		for (uint32_t idx = 0; idx < K; idx++) 
			neighbors[i][idx] = neighbors_row[idx];
	}

	free(Dblock);
	free(Dmult);
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

	if (Q->cols != C->cols) {
		printf("Q and C dimensions are not the same\n");
		matrixfree(&C);
		matrixfree(&Q);
		exit(0);
	}

	matrixprint(Q, "Q");
	matrixprint(C, "C");

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
