#ifndef KNNSEARCH_H
#define KNNSEARCH_H

#include <stdint.h>
#include <stdlib.h> 

/* struct definitions*/

enum io_type {
	MAT = 1,
	HDF5 = 2,
};

enum mat_type {
	DOUBLE,
	SINGLE,
	INT64,
	INT32,
	INT16,
};

typedef struct {
	enum io_type type;
	char *path;
} IOHeader;

typedef struct {
	uint32_t idx;
	double   dst;
} Neighbor;

typedef struct {
	enum mat_type type;
	uint32_t rows;
	uint32_t cols;
	uint32_t *idx;
	void     *dst;
} Neighbors;

typedef struct {
	void *data;
	uint32_t rows;
	uint32_t cols;
	enum mat_type type;
	size_t type_size;
} Matrix;

/* matio.c */

/*
 * loads a matrix of type double from a variable saved in a .mat file.
 */
Matrix* matrixget(IOHeader header, const char *varname);

/*
 * converts the matrix data to a float array
 */
void matrixtodouble(Matrix *mat);

/*
 * converts the matrix data to a double array
 */
void matrixtosingle(Matrix *mat);

/*
 * loads an unsigned integer from a variable saved in a .mat file.
 */
uint32_t uintget(IOHeader header, const char *varname);

/*
 * saves the neighbors' proerties matrices to a .mat file
 */
int matrixsave(Matrix *idx, Matrix *dst, const char *filepath);

/* arrays.c */

/*
 * prints error message and exits.
 */
void die(const char *message);

/*
 * returns whetever a string is a uint (0 false, 1 true).
 */
int isuint(char *s);

/*
 * wrapper to free Matrix structs. (pointer pass by reference)
 */
void matrixfree(Matrix **mat);

/*
 * returns the size of the given type
 */
size_t matrixtypesize(const enum mat_type type);

/*
 * initializes a Matrix struct. If the `data` pointer is not NULL, it memcopys
 * the data to the struct's data.
 */
Matrix* matrixinit(const uint32_t rows, const uint32_t cols, const enum mat_type type);

/*
 * prints the matrix to stdout. - UNUSED
 */
void matrixprint(Matrix *mat, const char *name);

/*
 * wrapper for malloc() that on fail puts an error message and exits.
 */
void* mmalloc(size_t size);

/*
 * wrapper to free Neighbors structs. (pointer pass by reference)
 */
void neighborsfree(Neighbors **neighbors);

/*
 * initializes a neighbors struct
 */
Neighbors* neighborsinit(const uint32_t N, const uint32_t K, const enum mat_type type);

/*
 * converts the neighbors to matrices and saves it to a .mat file through matrixsave()
 */
void neighborssave(Neighbors *neighbors, const IOHeader output);

void qselect(Neighbor *neighbors, int left, int right, int K);
void qselect_qsort(Neighbors *neighbors, int left, int right);

#endif // KNNSEARCH_H
