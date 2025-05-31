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

/* arrays.c */
int        isuint(char *s);
void       matrixfree(Matrix **mat);
Matrix*    matrixinit(const uint32_t rows, const uint32_t cols, double *data, int dataColumnMajor);
void       matrixprint(Matrix *mat, const char *name);
void       neighborarrfree(Neighbor ***neighbors, const uint32_t N);
void       neighborarrsave(Neighbor **neighbors, const uint32_t N, const uint32_t K, const char *filepath);
Neighbor** neighborsinit(const uint32_t N, const uint32_t K);
void       qselect(Neighbor *neighbors, int left, int right, int K);
void       qselect_qsort(Neighbor *neighbors, int left, int right);

#endif // KNNSEARCH_H
