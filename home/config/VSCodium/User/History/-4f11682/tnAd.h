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
	void          *data;
	uint32_t      rows;
	uint32_t      cols;
	enum mat_type type;
} Matrix_t;

/* matio.c */

/*
 * loads a matrix of type double from a variable saved in a .mat file.
 */
Matrix_t* matrixget(IOHeader header, const char *varname);

/*
 * loads an unsigned integer from a variable saved in a .mat file.
 */
uint32_t uintget(IOHeader header, const char *varname);

/*
 * saves the neighbors' proerties matrices to a .mat file
 */
int matrixsave(Matrix_t *idx, Matrix_t *dst, const char *filepath);

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
void matrixfree(Matrix_t **mat);

/*
 * initializes a Matrix_t struct. If the `data` pointer is not NULL, it memcopys
 * the data to the struct's data.
 */
Matrix_t* matrixinit(const uint32_t rows, const uint32_t cols, const enum mat_type type);

/*
 * converts the matrix data to a float array
 */
void matrixtodouble(Matrix_t *mat);

/*
 * converts the matrix data to a double array
 */
void matrixtosingle(Matrix_t *mat);

/*
 * wrapper for malloc() that on fail puts an error message and exits.
 */
void* mmalloc(size_t size);

/*
 * converts the neighbors to matrices and saves it to a .mat file through matrixsave()
 */
void neighborarrsave(Neighbor *neighbors, const uint32_t N, const uint32_t K, const char *filepath);

#endif // KNNSEARCH_H
