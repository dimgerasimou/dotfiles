#ifndef KNNSEARCH_H
#define KNNSEARCH_H

#include <stdint.h>
#include <stdlib.h> 

/* struct definitions*/

typedef enum  {
	MAT = 1,
	HDF5 = 2,
} io_type;

typedef enum {
	DOUBLE,
	SINGLE,
	INT64,
	INT32,
	INT16,
} mat_type;

typedef struct {
	io_type type;
	char    *path;
} IOHeader;

typedef struct {
	void     *data;
	uint32_t rows;
	uint32_t cols;
	mat_type type;
} Matrix_t;

/* matio.c */

/*
 * loads a matrix from a variable saved in a file.
 */
Matrix_t* matrixget(IOHeader header, const char *varname);

/*
 * saves the neighbors' proerties matrices to a .mat file
 */
void matrixsave(IOHeader header, Matrix_t *idx, Matrix_t *dst);

/* arrays.c */

/*
 * 
 */
void die(const char *message, const char *argument);

/*
 * wrapper to free Matrix structs. (pointer pass by reference)
 */
void matrixfree(Matrix_t **mat);

/*
 * returns the size of the given type
 */
size_t matrixtypesize(const mat_type type);

/*
 * initializes a Matrix_t struct.
 */
Matrix_t* matrixinit(const uint32_t rows, const uint32_t cols, const mat_type type);

/*
 * converts the matrix data to a double array
 */
void matrixtodouble(Matrix_t *mat);

/*
 * converts the matrix data to a float array
 */
void matrixtosingle(Matrix_t *mat);

/*
 * wrapper for malloc() that on fail puts an error message and exits.
 */
void* mmalloc(size_t size);

Matrix_t** knnsearch(Matrix_t *C, Matrix_t *Q, uint32_t *K, uint32_t blocksize);

#endif // KNNSEARCH_H
