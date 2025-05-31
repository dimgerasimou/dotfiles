#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <openblas/cblas.h>

#include "knnsearch.h"

/* global variables */
uint32_t g_blocksize = 32;

/* 
 * copys all data of the src matrix to the dst, from a given row index
 * up to g_blocksize rows and sets the correct sizes
 */
void
matrixtiling(Matrix *dst, Matrix *src, uint32_t iIdx)
{
	dst->rows = (iIdx + g_blocksize > src->rows) ? (src->rows - iIdx) : g_blocksize;

	for (uint32_t j = 0; j < dst->cols; j++)
		memcpy(&dst->data[j * dst->rows], &src->data[j * src->rows + iIdx], dst->rows * sizeof(double));
}

/*
 * using calculates the norms of the given matrix
 */
void
getnorm(Matrix *mat, double *norms)
{
	double *ones = (double*) malloc(mat->cols * sizeof(double));
	for (uint32_t i = 0; i < mat->cols; i++)
		ones[i] = 1.0;

	for (uint32_t i = 0; i < mat->rows; i++)
		norms[i] = cblas_dgemv(CblasColMajor, CblasNoTrans, mat->rows, mat->cols, 1.0, mat->data, mat->rows, ones, 1, 0.0, &norms[i], 1);

	free(ones);
}

/*
 * using cblas gets the euclidian distance of the vectors of the first matrix (rows)
 * compared with the vectors of the second (rows) and returns it element-wise in a matrix.
 */
void
getdistance(const Matrix *C, const Matrix *Q, const Matrix *D, double *D_M, double *Cnorm, double *Qnorm)
{

	cblas_dgemm(CblasColMajor, CblasNoTrans, CblasTrans, C->rows, Q->rows, C->cols, -2.0,
	            C->data, C->rows, Q->data, Q->rows, 0.0, D_M, C->rows);

	for (uint32_t i = 0; i < C->rows; i++) {
		for (uint32_t j = 0; j < Q->rows; j++) {
			D->data[i * Q->rows + j] = Cnorm[i] + D_M[j * C->rows + i] + Qnorm[j];
		}
	}
}

/*
 * returns the k - nearest neighbors of the vector querries (rows of Q)
 * compared to the vectors of the corpus set (rows of C)
 */
Neighbor*
knnsearch(Matrix *C, Matrix *Q, uint32_t *K)
{
	Neighbor *neighbors;
	Neighbor *neighbors_row;
	Matrix   *D, *Qtemp, *Ctemp;
	double   *D_M, *Cnorm, *Qnorm;

	if (Q->cols != C->cols) {
		printf("Dimensions are not the same\n");
		matrixfree(&C);
		matrixfree(&Q);
		exit(0);
	}

	*K = C->rows < *K ? C->rows : *K;

	neighbors = mmalloc(Q->rows * *K * sizeof(Neighbor));
	neighbors_row = mmalloc((g_blocksize + *K) * sizeof(Neighbor));

	D = matrixinit(g_blocksize, g_blocksize, NULL);
	Ctemp = matrixinit(g_blocksize,  C->cols, NULL);
	Qtemp = matrixinit(g_blocksize,  Q->cols, NULL);
	Cnorm = mmalloc(C->rows * sizeof(double));
	Qnorm = mmalloc(Q->rows * sizeof(double));

	D_M = mmalloc(g_blocksize * g_blocksize * sizeof(double));

	getnorm(C, Cnorm);
	getnorm(Q, Qnorm);

	for (uint32_t i = 0; i < Q->rows; i += g_blocksize) {
		matrixtiling(Qtemp, Q, i);

		for (uint32_t j = 0; j < C->rows; j += g_blocksize) {
			matrixtiling(Ctemp, C, j);

			D->rows = Qtemp->rows;
			D->cols = Ctemp->rows;

			getdistance(Ctemp, Qtemp, D, D_M, &Cnorm[j], &Qnorm[i]);
			
			uint32_t coloffset = j < *K ? j : *K;
			for (uint32_t ii = 0; ii < D->rows; ii++) {
				

				for (uint32_t jj = 0; jj < D->cols; jj++) {
					neighbors_row[coloffset + jj].idx = j + jj;
					neighbors_row[coloffset + jj].dst = D->data[ii + jj * D->rows];
				}

				for (uint32_t jj = 0; jj < coloffset; jj++)
					neighbors_row[jj] = neighbors[((ii + i) * *K) + jj];

				qselect(neighbors_row, 0, coloffset + D->cols - 1, *K);

				for (uint32_t idx = 0; idx < *K; idx++) 
					neighbors[((ii + i) * *K) + idx] = neighbors_row[idx];
			}
		}
	}

	for (uint32_t i = 0; i < Q->rows; i++) {
		qselect_qsort(neighbors, i * *K, (i * *K) + *K - 1);
		for (uint32_t j = 0; j < *K; j++) {
			neighbors[(i * *K) + j].dst = sqrt(fabs(neighbors[(i * *K) + j].dst));
		}
	}

	matrixfree(&D);
	matrixfree(&Qtemp);
	matrixfree(&Ctemp);
	free(neighbors_row);
	free(D_M);
	free(Cnorm);
	free(Qnorm);

	return neighbors;
}

int
main(int argc, char *argv[])
{
	struct timeval t0, t1;
	Neighbor *neighbors;
	uint32_t K;
	Matrix   *C;
	Matrix   *Q;

	if (argc < 3) {
		fprintf(stderr, "Invalid number of arguments\n");
		exit(1);
	}

	if (argc > 3 && isuint(argv[3]))
		g_blocksize = atoi(argv[3]);

	K = matiogetuint(argv[1], "K");
	C = matiogetmat(argv[1], "C");
	Q = matiogetmat(argv[1], "Q");

	gettimeofday(&t0, NULL);

	// benchmarked function here
	neighbors = knnsearch(C, Q,&K);

	gettimeofday(&t1, NULL);
	printf("%.8g\n", t1.tv_sec - t0.tv_sec + 1E-6 * (t1.tv_usec - t0.tv_usec));

	neighborarrsave(neighbors, Q->rows, K, argv[2]);
	free(neighbors);

	matrixfree(&C);
	matrixfree(&Q);
	return 0;
}
