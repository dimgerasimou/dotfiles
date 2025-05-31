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

#endif // KNNSEARCH_H
