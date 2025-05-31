#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <openblas/cblas.h>
#include <omp.h>

#include "knnsearch.h"

/* global variables */
uint16_t g_blocksize = 32;

/*
 * dynamically adjusts the blocksize to a power of 2, based on the cache size:
 * total memory = (2 * g_blocksize * cols + 2 * g_blocksize^2 + 2 * g_blocksize) * sizeof(double).
 *
 * For over 2^24 elements in the D matrix returns a g_blocksize, where total memory exceedes
 * the L2 cache, even though of the increased probability of cache misses, the penalty on average is less
 * than the cost of reducing the iterations by half. For elements less than 2^24, g_blocksize
 * is set that the total memory usage of each thread is less than the L2 cache.
*/
void
dynamic_blocksize(size_t element_size, uint32_t cols, uint64_t nelem)
{
	long cache_size;
	double max_blocksize;

	// check if 
	cache_size = sysconf(_SC_LEVEL2_CACHE_SIZE);
	if (cache_size == -1) cache_size = 512 * 1024;  // default to 512 KiB if unknown

	max_blocksize = (- (double) (cols + 1) + sqrt((double) (cols + 1) * (cols + 1) + (double) (2 * cache_size) / element_size)) / 2.0;

	// round to nearest power of 2 for block alignment
	uint16_t ret = 1;
	while (ret < (int) max_blocksize) ret *= 2;

	if (nelem < 16777216)
		ret /= 2;

	g_blocksize =  ret > 32 ? ret : 32;  // return 32 as a safe fallback
}

/*
 * calculates the norms of the given matrix
 */
void
getnorms(Matrix *mat, double *norms)
{
	#pragma omp parallel for
	for (uint32_t i = 0; i < mat->rows; i++)
		norms[i] = cblas_ddot(mat->cols, &mat->data[i * mat->cols], 1, &mat->data[i * mat->cols], 1);
}

/*
 * using cblas gets the euclidian distance of the vectors of the first matrix (rows)
 * compared with the vectors of the second (rows) and returns in a matrix.
 */
void
getdistance(const double *C, const double *Q, const Matrix *D, double *D_M, double *Cnorm, double *Qnorm, const uint32_t cols)
{
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, D->cols, D->rows, cols, -2.0,
	            C, cols, Q, cols, 0.0, D_M, D->rows);

	for (uint32_t i = 0; i < D->rows; i++) {
		for (uint32_t j = 0; j < D->cols; j++) {
			D->data[i * D->cols + j] = Cnorm[j] + D_M[j * D->rows + i] + Qnorm[i];
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
	uint16_t tID, nthreads = omp_get_max_threads();
	Neighbor *neighbors, **neighbors_b;
	double   *Cnorm, *Qnorm, **D_M_b;
	Matrix   **D_b;

	if (Q->cols != C->cols) {
		printf("Dimensions are not the same\n");
		matrixfree(&C);
		matrixfree(&Q);
		exit(0);
	}

	*K = C->rows < *K ? C->rows : *K;

	neighbors   = mmalloc(Q->rows * *K * sizeof(Neighbor));
	Cnorm       = mmalloc(C->rows * sizeof(double));
	Qnorm       = mmalloc(Q->rows * sizeof(double));
	D_b         = mmalloc(nthreads * sizeof(Matrix*));
	D_M_b       = mmalloc(nthreads * sizeof(double*));
	neighbors_b = mmalloc(nthreads * sizeof(Neighbor *));

	getnorms(C, Cnorm);
	getnorms(Q, Qnorm);

	#pragma omp parallel
	{
		tID = omp_get_thread_num();

		D_b[tID] =  matrixinit(g_blocksize, g_blocksize);
		neighbors_b[tID] = mmalloc((g_blocksize + *K) * sizeof(Neighbor));
		D_M_b[tID] = mmalloc(g_blocksize * g_blocksize * sizeof(double));
		printf("%u\n", tID);

		#pragma omp for
		for (uint32_t i = 0; i < Q->rows; i += g_blocksize) {
			printf("%u\n", tID);
			Matrix *D = D_b[tID];
			Neighbor *neighbors_row = neighbors_b[tID];
			double *D_M = D_M_b[tID];

			D->rows = (i + g_blocksize > Q->rows) ? (Q->rows - i) : g_blocksize;

			for (uint32_t j = 0; j < C->rows; j += g_blocksize) {
				D->cols = (j + g_blocksize > C->rows) ? (C->rows - j) : g_blocksize;

				getdistance(C->data + j * C->cols, Q->data + i * Q->cols, D, D_M, &Cnorm[j], &Qnorm[i], C->cols);

				uint32_t coloffset = j < *K ? j : *K;
				for (uint32_t ii = 0; ii < D->rows; ii++) {
					for (uint32_t jj = 0; jj < D->cols; jj++) {
						neighbors_row[coloffset + jj].idx = j + jj;
						neighbors_row[coloffset + jj].dst = D->data[jj + ii * D->cols];
					}

					for (uint32_t jj = 0; jj < coloffset; jj++)
						neighbors_row[jj] = neighbors[((ii + i) * *K) + jj];

					qselect(neighbors_row, 0, coloffset + D->cols - 1, *K);

					for (uint32_t idx = 0; idx < *K; idx++) 
						neighbors[((ii + i) * *K) + idx] = neighbors_row[idx];
				}
			}
		}
	}

	for (int i = 0; i < nthreads; i++) {
		free(neighbors_b[i]);
		matrixfree(&D_b[i]);
		free(D_M_b[i]);
	}

	free(neighbors_b);
	free(D_b);
	free(D_M_b);
	free(Cnorm);
	free(Qnorm);

	#pragma omp parallel for
	for (uint32_t i = 0; i < Q->rows; i++) {
		qselect_qsort(neighbors, i * *K, (i * *K) + *K - 1);
		for (uint32_t j = 0; j < *K; j++) {
			neighbors[(i * *K) + j].dst = sqrt(neighbors[(i * *K) + j].dst);
		}
	}

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

	K = matiogetuint(argv[1], "K");
	C = matiogetmat(argv[1], "C");
	Q = matiogetmat(argv[1], "Q");

	if (argc > 3 && isuint(argv[3]))
		g_blocksize = atoi(argv[3]);
	else
		dynamic_blocksize(sizeof(double), C->cols, C->rows * Q->rows);

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
