#ifndef KNNSEARCH_H
#define KNNSEARCH_H

#include <stdint.h>
#include <stdlib.h> 

/* struct definitions*/

enum io_type {
	MAT = 1,
	HDF5 = 2,
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
	double   *data;
	uint32_t rows;
	uint32_t cols;
} Matrix;

/* matio.c */

/*
 * loads a matrix of type double from a variable saved in a .mat file.
 */
Matrix* matrixget(IOHeader header, const char *varname);

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
 * returns whetever a string is a uint (0 false, 1 true).
 */
void die(const char *message);

/*
 * returns whetever a string is a uint (0 false, 1 true).
 */
int isuint(char *s);

/*
 * wrapper to free Matrix structs. (pointer pass by reference)
 */
void       matrixfree(Matrix **mat);

/*
 * initializes a Matrix struct. If the `data` pointer is not NULL, it memcopys
 * the data to the struct's data.
 */
Matrix*    matrixinit(const uint32_t rows, const uint32_t cols);

/*
 * prints the matrix to stdout. - UNUSED
 */
void       matrixprint(Matrix *mat, const char *name);

/*
 * wrapper for malloc() that on fail puts an error message and exits.
 */
void*      mmalloc(size_t size);

/*
 * converts the neighbors to matrices and saves it to a .mat file through matrixsave()
 */
void       neighborarrsave(Neighbor *neighbors, const uint32_t N, const uint32_t K, const char *filepath);

#endif // KNNSEARCH_H
