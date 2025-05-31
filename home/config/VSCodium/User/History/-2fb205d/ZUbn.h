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
int        matrixsave(Matrix *idx, Matrix *dst, const char *filepath);
void       neighborarrsave(Neighbor **neighbors, const uint32_t N, const uint32_t K, const char *filepath);

/* arrays.c */

#endif // KNNSEARCH_H
