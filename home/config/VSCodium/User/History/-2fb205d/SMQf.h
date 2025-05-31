#ifndef KNNSEARCH_H
#define KNNSEARCH_H

#include <stdint.h>

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
Matrix*    matiogetmat(const char *filepath, const char *varname);
uint32_t   matiogetuint(const char *filepath, const char *varname);

#endif // KNNSEARCH_H
