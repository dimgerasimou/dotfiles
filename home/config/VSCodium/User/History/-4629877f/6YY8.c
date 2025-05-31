#include <matio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

double RandomGenerator(void) {
	return ( (double)(rand()) + 1. )/( (double)(RAND_MAX) + 1. );
}
double normalRandom(void) {
	double y1=RandomGenerator();
	double y2=RandomGenerator();
	return cos(2*3.14*y2)*sqrt(-2.*log(y1));
}

int main(int argc, char *argv[]) {
	double sigma = 82.;
	double Mi = 40.;
	if (argc < 5)
		return 1;

	mat_t *matfp = NULL;
	if (!(matfp = Mat_CreateVer(argv[1], NULL, MAT_FT_MAT5)))
		return 2;

	for (int i = 2; i < argc; i+=3) {
		if (argc < i+3)
			return 3;
		srand(time(NULL));
		
		matvar_t *matvar = NULL;
		size_t dims[2] = { atoi(argv[i+1]), atoi(argv[i+2]) };
		double *arr;

		if (!(arr = malloc(dims[0] * dims[1] * sizeof(double))))
			return 4;

		for (size_t i = 0; i < dims[0] * dims[1]; i++)
			arr[i] = normalRandom() * sigma + Mi;

		matvar = Mat_VarCreate(argv[i], MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, arr, MAT_F_DONT_COPY_DATA);

		if (!matvar)
			return 5;

		if (Mat_VarWrite(matfp, matvar, MAT_COMPRESSION_NONE) != 0) {
			Mat_VarFree(matvar);
			Mat_Close(matfp);
			return 4;
		}
		Mat_VarFree(matvar);

	}
	
	Mat_Close(matfp);
	return 0;
}
