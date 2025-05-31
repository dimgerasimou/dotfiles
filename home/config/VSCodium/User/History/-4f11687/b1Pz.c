#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <openblas/cblas.h>
#include <omp.h>

#include "knnsearch.h"

typedef struct {
	double  dst;
	int32_t idx;
} Neighbor;

static void qselect(Neighbor *neighbors, int left, int right, int K);
static void quicksort(Neighbor *neighbors, int left, int right);

/*
 * dynamically adjusts the blocksize to a power of 2, based on the cache size:
 * total memory = (2 * g_blocksize * cols + 2 * g_blocksize^2 + 2 * g_blocksize) * sizeof(double).
 *
 * For over 2^27 bytes in the D matrix returns a g_blocksize, where total memory exceedes
 * the L2 cache, even though of the increased probability of cache misses, the penalty on average is less
 * than the cost of reducing the iterations by half. For bytes less than 2^27, g_blocksize
 * is set that the total memory usage of each thread is less than the L2 cache.
*/
static uint32_t
dynamic_blocksize(const size_t element_size, const uint32_t cols, const uint64_t nelem)
{
	long cache_size;
	double max_blocksize;

	cache_size = sysconf(_SC_LEVEL2_CACHE_SIZE);
	if (cache_size == -1) cache_size = 512 * 1024;  // default to 512 KiB if unknown

	max_blocksize = (- (double) (cols + 1) + sqrt((double) (cols + 1) * (cols + 1) + (double) (2 * cache_size) / element_size)) / 2.0;

	// round to nearest power of 2 for block alignment
	uint16_t ret = 1;
	while (ret < (int) max_blocksize) ret *= 2;

	if (nelem < (1 << 27) / element_size)
		ret /= 2;

	return ret > 32 ? ret : 32;  // return 32 as a safe fallback
}

/*
 * calculates the norms of the given matrix
 */
static double*
getnorms(const Matrix_t *mat)
{
	double *norms;

	norms = mmalloc(mat->rows * sizeof(double));

	#pragma omp parallel for
	for (uint32_t i = 0; i < mat->rows; i++)
		norms[i] = cblas_ddot(mat->cols, &((double *) mat->data)[i * mat->cols], 1, &((double *) mat->data)[i * mat->cols], 1);

	return norms;
}

/*
 * using cblas gets the euclidian distance of the vectors of the first matrix (rows)
 * compared with the vectors of the second (rows) and returns in a matrix.
 */
static void
getdistance(const double *C, const double *Q, const Matrix_t *D, double *D_M, const double *Cnorm, const double *Qnorm, const uint32_t cols)
{
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, D->cols, D->rows, cols, -2.0,
	            C, cols, Q, cols, 0.0, D_M, D->rows);

	for (uint32_t i = 0; i < D->rows; i++) {
		for (uint32_t j = 0; j < D->cols; j++) {
			((double*) D->data)[i * D->cols + j] = Cnorm[j] + D_M[j * D->rows + i] + Qnorm[i];
		}
	}
}

Matrix_t**
knnsearch(Matrix_t *C, Matrix_t *Q, uint32_t *K, uint32_t blocksize, uint16_t quick_sort)
{
	uint16_t nthreads;
	Neighbor *neighbors, **neighbors_b;
	double   *Cnorm, *Qnorm, **D_M_b;
	Matrix_t **D_b;
	Matrix_t **ret;

	matrixtodouble(Q);
	matrixtodouble(C);

	if (!blocksize)
		blocksize = dynamic_blocksize(sizeof(double), C->cols, C->rows * Q->rows);

	if (Q->cols != C->cols) {
		matrixfree(&C);
		matrixfree(&Q);
		printf("Matrix dimensions are not the same\n");
		exit(0);
	}

	*K = C->rows < *K ? C->rows : *K;

	neighbors   = mmalloc(Q->rows * *K * sizeof(Neighbor));

	nthreads    = omp_get_max_threads();
	D_b         = mmalloc(nthreads * sizeof(Matrix_t*));
	D_M_b       = mmalloc(nthreads * sizeof(double*));
	neighbors_b = mmalloc(nthreads * sizeof(Neighbor*));

	Cnorm = getnorms(C);
	Qnorm = getnorms(Q);

	#pragma omp parallel
	{
		uint16_t tID = omp_get_thread_num();

		D_b[tID] =  matrixinit(blocksize, blocksize, DOUBLE);
		neighbors_b[tID] = mmalloc((blocksize + *K) * sizeof(Neighbor));
		D_M_b[tID] = mmalloc(blocksize * blocksize * sizeof(double));

		#pragma omp for
		for (uint32_t i = 0; i < Q->rows; i += blocksize) {
			Matrix_t *D = D_b[tID];
			Neighbor *neighbors_row = neighbors_b[tID];
			double *D_M = D_M_b[tID];
			
			D->rows = (i + blocksize > Q->rows) ? (Q->rows - i) : blocksize;

			for (uint32_t j = 0; j < C->rows; j += blocksize) {
				D->cols = (j + blocksize > C->rows) ? (C->rows - j) : blocksize;
				getdistance((double*) C->data + j * C->cols, (double*) Q->data + i * Q->cols, D, D_M, &Cnorm[j], &Qnorm[i], C->cols);

				uint32_t coloffset = j < *K ? j : *K;
				for (uint32_t ii = 0; ii < D->rows; ii++) {
					uint32_t colsize = coloffset + D->cols;
					for (uint32_t jj = 0; jj < D->cols; jj++) {
						neighbors_row[coloffset + jj].idx = j + jj;
						neighbors_row[coloffset + jj].dst = ((double*) D->data)[jj + ii * D->cols];
					}

					for (uint32_t jj = 0; jj < coloffset; jj++)
						neighbors_row[jj] = neighbors[((ii + i) * *K) + jj];

					qselect(neighbors_row, 0, colsize - 1, *K);

					for (uint32_t idx = 0; idx < (colsize < *K ? colsize : *K); idx++) 
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

	ret = mmalloc(2 * sizeof(Matrix_t*));
	ret[0] = matrixinit(Q->rows, *K, INT32);
	ret[1] = matrixinit(Q->rows, *K, DOUBLE);

	int32_t *idx_temp = (int32_t*) ret[0]->data;
	double  *dst_temp = (double*) ret[1]->data;

	#pragma omp parallel for
	for (uint32_t i = 0; i < Q->rows; i++) {
		if (quick_sort)
			quicksort(neighbors, i * *K, (i * *K) + *K - 1);
		
		for (uint32_t j = 0; j < *K; j++) {
			idx_temp[(i * *K) + j] = neighbors[(i * *K) + j].idx;
			dst_temp[(i * *K) + j] = neighbors[(i * *K) + j].dst < 0 ? 0 : sqrt(neighbors[(i * *K) + j].dst);
		}
	}

	free(neighbors);

	return ret;
}

static void
qselect_swap(Neighbor *a, Neighbor *b)
{
	Neighbor temp = *a;
	*a = *b;
	*b = temp;
}

static int
qselect_partition(Neighbor *neighbors, const int left, const int right)
{
	int mid = left + (right - left) / 2;

	// Modified comparisons to handle dst tie-breakers by idx
	if (neighbors[left].dst > neighbors[mid].dst ||
	    (neighbors[left].dst == neighbors[mid].dst && neighbors[left].idx > neighbors[mid].idx))
		qselect_swap(&neighbors[left], &neighbors[mid]);

	if (neighbors[left].dst > neighbors[right].dst ||
	    (neighbors[left].dst == neighbors[right].dst && neighbors[left].idx > neighbors[right].idx))
		qselect_swap(&neighbors[left], &neighbors[right]);

	if (neighbors[mid].dst > neighbors[right].dst ||
	    (neighbors[mid].dst == neighbors[right].dst && neighbors[mid].idx > neighbors[right].idx))
		qselect_swap(&neighbors[mid], &neighbors[right]);

	if (right - 1 > left)
		qselect_swap(&neighbors[mid], &neighbors[right - 1]);
	double pivot = neighbors[right - 1].dst;
	int pivot_idx = neighbors[right - 1].idx;

	int i = left, j = right - 1;

	while (1) {
		while (neighbors[++i].dst < pivot || 
		      (neighbors[i].dst == pivot && neighbors[i].idx < pivot_idx));
		while (neighbors[--j].dst > pivot || 
		      (neighbors[j].dst == pivot && neighbors[j].idx > pivot_idx));
		if (i >= j) break;
		qselect_swap(&neighbors[i], &neighbors[j]);
	}
	qselect_swap(&neighbors[i], &neighbors[right - 1]);
	return i;
}

static void
qselect(Neighbor *neighbors, const int left, const int right, const int K)
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

static void
quicksort(Neighbor *neighbors, const int left, const int right)
{
	if (left < right) {

		int pivotIndex = qselect_partition(neighbors, left, right);

		quicksort(neighbors, left, pivotIndex - 1);
		quicksort(neighbors, pivotIndex + 1, right);
	}
}
