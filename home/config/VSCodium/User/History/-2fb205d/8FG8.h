#ifndef KNNSEARCH_H
#define KNNSEARCH_H

#include <stdint.h>
#include <stdlib.h> 

/* struct definitions*/
typedef struct {
	uint32_t idx;
	double   dst;
} Neighbor;

typedef struct {
	double   *data;
	uint32_t rows;
	uint32_t cols;
} Matrix;

/* matio.c */

/*
 * loads a matrix of type double from a variable saved in a .mat file.
 */
Matrix*    matiogetmat(const char *filepath, const char *varname);

/*
 * loads an unsigned integer from a variable saved in a .mat file.
 */
uint32_t   matiogetuint(const char *filepath, const char *varname);

/*
 * saves the neighbors' proerties matrices to a .mat file
 */
int        matrixsave(Matrix *idx, Matrix *dst, const char *filepath);

/* arrays.c */


int        isuint(char *s);
void       matrixfree(Matrix **mat);

/*
 * initializes a Matrix struct. If the `data` pointer is not NULL, it memcopys
 * the data to the struct's data.
 */
Matrix*    matrixinit(const uint32_t rows, const uint32_t cols, const double *data);
void       matrixprint(Matrix *mat, const char *name); /* unused */

/*
 * wrapper for malloc() that on fail puts an error message and exits.
 */
void*      mmalloc(size_t size);

/*
 * wrapper to free a neighbor struct 2d array. (pointer pass by reference)
 */
void       neighborarrfree(Neighbor ***neighbors, const uint32_t N);

/*
 * converts the neighbors to matrices and saves it to a .mat file through matrixsave()
 */
void       neighborarrsave(Neighbor **neighbors, const uint32_t N, const uint32_t K, const char *filepath);

/*
 * initialises an empty 2d array of neighbor structs.
 */
Neighbor** neighborsinit(const uint32_t N, const uint32_t K);

void       qselect(Neighbor *neighbors, int left, int right, int K);
void       qselect_qsort(Neighbor *neighbors, int left, int right);

#endif // KNNSEARCH_H
