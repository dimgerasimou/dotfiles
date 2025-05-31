#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <openblas/cblas.h>
#include <omp.h>
#include <string.h>

#include "knnsearch.h"

#define BUCKET_COUNT 1 // Adjust based on your needs
#define INITIAL_BUCKET_SIZE 16 // Initial bucket capacity

typedef struct {
	double  dst;
	int32_t idx;
} Neighbor;

typedef struct {
	uint32_t **bucket_data;   // Each row represents a bucket in a table
	uint32_t *bucket_sizes;   // Current sizes of each bucket
	uint32_t nbuckets;     // Total number of buckets per table
	uint32_t capacity;        // Capacity of each bucket
} HashTable;

typedef struct {
	double **rand_vectors;
	double *rand_offsets;
	double  W;
	uint32_t L;
	uint32_t K;
	uint32_t D;
} LshParameters;

void
matrixprint(Matrix_t *mat, const char *name)
{
	printf("%s:\n %dx%d\n", name, mat->rows, mat->cols);
	for (uint32_t i = 0; i < mat->rows; i++) {
		for (uint32_t j = 0; j < mat->cols; j++) {
			printf("%10lf ", ((double*)mat->data)[i * mat->cols + j]);
		}
		printf("\n");
	}
	printf("\n");
}

static double*
randomnormalvector(uint32_t dims) {
	double x, y;
	double *vec;
	
	vec = mmalloc(dims * sizeof(double));

	for (uint32_t i = 0; i < dims; i++) {
		x = (double) rand() / RAND_MAX;
		y = (double) rand() / RAND_MAX;
		vec[i] = sqrt(-2.0 * log(x)) * cos(2.0 * M_PI * y);
	}
	return vec;
}

static LshParameters*
lshinit(const double W, const uint32_t L, const uint32_t K, const uint32_t dims)
{
	LshParameters *ret;
	ret = mmalloc(sizeof(LshParameters));

	ret->K = K;
	ret->L = L;
	ret->W = W;
	ret->D = dims;

	ret->rand_vectors = mmalloc(L * K * sizeof(double*));
	ret->rand_offsets = mmalloc(L * K * sizeof(double));

	for (uint32_t i = 0; i < L * K; i++) {
		ret->rand_vectors[i] = randomnormalvector(dims);
		ret->rand_offsets[i] = ((double)rand() / RAND_MAX) * W;
	}

	return ret;
}

static void
lshprint(LshParameters *lsh)
{
	printf("Random Hash Functions:\n");
	for (uint32_t i = 0; i < lsh->L; i++) {
		printf("Table %2d:\n", i + 1);
		for (uint32_t j = 0; j < lsh->K; j++) {
			printf("  Hash Function %2d: a = [ ", j + 1);
			for (uint32_t d = 0; d < lsh->D; d++) {
				printf("%.2f ", lsh->rand_vectors[i * lsh->K + j][d]);
			}
			printf("], b = %.2f\n", lsh->rand_offsets[i * lsh->K + j]);
		}
	}
	printf("\n");
}

static void
hashcompute(Matrix_t *C, LshParameters *lsh, uint32_t *hashes)
{
	for (uint32_t i = 0; i < C->rows; i++) { // For each point
		for (uint32_t t = 0; t < lsh->L; t++) { // For each table
			for (uint32_t h = 0; h < lsh->K; h++) { // For each hash function
				// Dot product: a * x
				double dot = cblas_ddot(C->cols, &((double*) C->data)[i * C->cols], 1, lsh->rand_vectors[t * lsh->K + h], 1);
				// Compute hash: floor((dot + b) / w)
				int hash = (int)((dot + lsh->rand_offsets[t * lsh->K + h]) / lsh->W);
				hashes[(i * lsh->L + t) * lsh->K + h] = hash;
			}
		}
	}
}

static HashTable*
hashtableinit(uint32_t nbuckets, uint32_t capacity)
{
	HashTable *ht;

	ht = mmalloc(sizeof(HashTable));

	ht->bucket_data = mmalloc(nbuckets * sizeof(uint32_t *));
	ht->bucket_sizes = calloc(nbuckets, sizeof(uint32_t));
	ht->nbuckets = nbuckets;
	ht->capacity = capacity;

	for (uint32_t i = 0; i < nbuckets; i++)
		ht->bucket_data[i] = mmalloc(capacity * sizeof(uint32_t));

	return ht;
}

static void
bucketinsert(HashTable *ht, uint32_t ibucket, uint32_t ivec)
{
	if (ht->bucket_sizes[ibucket] >= ht->capacity) {
		uint32_t *temp;
		
		ht->capacity *= 2;
		temp =  realloc(ht->bucket_data[ibucket], ht->capacity * sizeof(uint32_t));
		
		if (!temp)
			die("Failed to resize bucket", "realloc()");
		
		ht->bucket_data[ibucket] = temp;
	}

	ht->bucket_data[ibucket][ht->bucket_sizes[ibucket]++] = ivec;
}

static void
candidatescollect(Matrix_t *Q, LshParameters *lsh, HashTable **tables, uint32_t *hashes, uint32_t **candidates, uint32_t *candidate_sizes)
{
	uint32_t count, bucket_idx;
	uint8_t *visited;

	visited = mmalloc(BUCKET_COUNT *  sizeof(uint8_t));

	for (uint32_t q = 0; q < Q->rows; q++) {
		count = 0;
		memset(visited, 0, BUCKET_COUNT * sizeof(uint8_t));

		for (uint32_t t = 0; t < lsh->L; t++) {
			bucket_idx = 0;
			for (uint32_t h = 0; h < lsh->K; h++) {
				bucket_idx ^= hashes[(q * lsh->L + t) * lsh->K + h];
			}
			bucket_idx %= BUCKET_COUNT;

			// Avoid duplicate candidates from the same bucket
			if (!visited[bucket_idx]) {
				visited[bucket_idx] = 1;
				for (uint32_t j = 0; j < tables[t]->bucket_sizes[bucket_idx]; j++) {
					candidates[q][count++] = tables[t]->bucket_data[bucket_idx][j];
				}
			}
		}
		candidate_sizes[q] = count;
	}
	if (visited)
		free(visited);
}

static double
euclidean_distance(const double *x, const double *y, uint32_t dims)
{
	double dist = 0.0;
	for (uint32_t i = 0; i < dims; i++) {
		double diff = x[i] - y[i];
		dist += diff * diff;
	}

	return sqrt(dist);
}

// Comparison function for sorting neighbors
static int compare_neighbors(const void *a, const void *b) {
	Neighbor *na = (Neighbor *)a;
	Neighbor *nb = (Neighbor *)b;
	return (na->dst > nb->dst) - (na->dst < nb->dst);
}

Matrix_t**
knnsearch(Matrix_t *C, Matrix_t *Q, uint32_t *K, uint32_t blocksize, uint16_t quick_sort)
{
	Matrix_t      **ret    = NULL;
	LshParameters *lsh     = NULL;
	uint32_t      *hashes  = NULL;
	HashTable     **tables = NULL;
	double        w        = 0.1;

	openblas_set_num_threads(1);

	matrixtodouble(Q);
	matrixtodouble(C);

	if (Q->cols != C->cols) {
		matrixfree(&C);
		matrixfree(&Q);
		printf("Matrix dimensions are not the same\n");
		exit(0);
	}

	(void) quick_sort;
	(void) blocksize;
	(void) K;

	lsh = lshinit(w, 1, 2, C->cols);

	lshprint(lsh);

	hashes = mmalloc((C->rows > Q->rows ? C->rows : Q->rows) * lsh->L * lsh->K * sizeof(uint32_t));
	hashcompute(C, lsh, hashes);

	tables = mmalloc(lsh->L * sizeof(HashTable*));

	for (uint32_t i = 0; i < lsh->L; i++)
		tables[i] = hashtableinit(BUCKET_COUNT, INITIAL_BUCKET_SIZE);

	for (uint32_t i = 0; i < C->rows; i++) {
		for (uint32_t t = 0; t < lsh->L; t++) {
			uint32_t bucket_idx = 0;
			for (uint32_t h = 0; h < lsh->K; h++) {
				bucket_idx ^= hashes[(i * lsh->L + t) * lsh->K + h];
			}
			bucket_idx %= BUCKET_COUNT;
			bucketinsert(tables[t], bucket_idx, i);
		}
	}

	uint32_t **candidates, *ncandidates;

	candidates = mmalloc(Q->rows * sizeof(uint32_t*));
	ncandidates = mmalloc(Q->rows * sizeof(uint32_t));
	for (uint32_t i = 0; i < Q->rows; i++) {
		candidates[i] = mmalloc(C->rows * sizeof(uint32_t));
		memset(candidates[i], 0, C->rows * sizeof(uint32_t));
	}

	candidatescollect(Q, lsh, tables, hashes, candidates, ncandidates);

	printf("candidates:\n");
	for (uint i = 0; i < Q->rows; i++) {
		printf("n:%u -", ncandidates[i]);
		for (uint j = 0; j < ncandidates[i]; j++) {
			printf(" %u", candidates[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	// Allocate space for kNN results
	ret = mmalloc(2 * sizeof(Matrix_t*));
	ret[0] = matrixinit(Q->rows, *K, INT32);
	ret[1] = matrixinit(Q->rows, *K, DOUBLE);

	// Compute k-nearest neighbors for each query
	for (uint32_t q = 0; q < Q->rows; q++) {
		uint32_t ncand = ncandidates[q];
		double *distances = mmalloc(ncand * sizeof(double));
		Neighbor *neighbors = mmalloc(ncand * sizeof(Neighbor));

		// Calculate distances for all candidates
		for (uint32_t c = 0; c < ncand; c++) {
			uint32_t cand_idx = candidates[q][c];
			distances[c] = euclidean_distance(
			&((double*) Q->data)[q * Q->cols],
			&((double*) C->data)[cand_idx * C->cols],
			C->cols
			);
			neighbors[c].dst = distances[c];
			neighbors[c].idx = cand_idx;
		}

		// Sort distances and select the top K (can use partial sort for efficiency)
		qsort(neighbors, ncand, sizeof(Neighbor), compare_neighbors);

		// Save top K neighbors to the result matrix
		for (uint32_t k = 0; k < *K; k++) {
			printf("idx:%d dst:%.2lf ", neighbors[k].idx, neighbors[k].dst);
			((int32_t*) ret[0]->data)[q * ret[0]->cols + k] = neighbors[k].idx;  // Neighbor index
			((double*)  ret[1]->data)[q * ret[0]->cols + k] = neighbors[k].dst;  // Distance
		}
		printf("\n");

		free(distances);
		free(neighbors);
	}

	for (uint32_t i = 0; i < Q->rows; i++) {
		free(candidates[i]);
	}

	free(candidates);
	free(ncandidates);

	for (uint32_t t = 0; t < lsh->L; t++) {
		for (uint32_t b = 0; b < tables[t]->nbuckets; b++) {
			free(tables[t]->bucket_data[b]);
		}
		free(tables[t]->bucket_data);
		free(tables[t]->bucket_sizes);
		free(tables[t]);
	}

	free(tables);
	free(hashes);
	free(lsh->rand_vectors);
	free(lsh->rand_offsets);
	free(lsh);
	return ret;
}
