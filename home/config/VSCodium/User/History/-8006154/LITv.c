#include <math.h>
#include <matio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

double g_sigma = 82.;
double g_mi = 40.;

double RandomGenerator(void) {
	return ( (double)(rand()) + 1. )/( (double)(RAND_MAX) + 1. );
}

double normalRandom(void) {
	double y1=RandomGenerator();
	double y2=RandomGenerator();
	return cos(2*3.14*y2)*sqrt(-2.*log(y1));
}

int main(int argc, char *argv[]) {
	mat_t    *fp = NULL;
	matvar_t *var;
	double   *doub = NULL;
	int      *inti = NULL;
	size_t   dims[2];

	srand(time(NULL));
	if (argc < 6)
		return 1;

	if (!(fp = Mat_CreateVer(argv[1], NULL, MAT_FT_MAT5)))
		return 2;

	for (int i = 2; i < argc; i += 4) {
		var = NULL;

		if (argc < i + 2)
			return 3;

		if (!(strcmp(argv[i], "K"))) {
			dims[0] = dims[1] = 1;
			if (!(inti = malloc(dims[0] * dims[1] * sizeof(int))))
				return 4;
			inti[0] = atoi(argv[i + 1]);
			var = Mat_VarCreate(argv[i], MAT_C_INT16, MAT_T_INT16, 2, dims, inti, MAT_F_DONT_COPY_DATA);
			i -= 2;
		} else {
			if (argc < i + 4)
				return 3;

			dims[0] = atoi(argv[i + 2]);
			dims[1] = atoi(argv[i + 3]);

			if (!strcmp(argv[i + 1], "i")) {
				if (!(inti = malloc(dims[0] * dims[1] * sizeof(int))))
					return 4;
				for (size_t i = 0; i < dims[0] * dims[1]; i++)
					inti[i] = rand() % 64 + 1;

				var = Mat_VarCreate(argv[i], MAT_C_INT16, MAT_T_INT16, 2, dims, inti, MAT_F_DONT_COPY_DATA);
			} else if (!strcmp(argv[i + 1], "d")) {
				if (!(doub = malloc(dims[0] * dims[1] * sizeof(double))))
					return 4;
				
				#pragma omp parallel for
				for (size_t i = 0; i < dims[0] * dims[1]; i++)
					doub[i] = normalRandom() * g_sigma + g_mi;
				var = Mat_VarCreate(argv[i], MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, doub, MAT_F_DONT_COPY_DATA);
			}
		}

		if (!var)
			return 5;

		if (Mat_VarWrite(fp, var, MAT_COMPRESSION_NONE) != 0) {
			Mat_VarFree(var);
			Mat_Close(fp);
			return 4;
		}

		Mat_VarFree(var);
		if (doub) {
			free(doub);
			doub = NULL;
		}
		if (inti) {
			free(inti);
			inti = NULL;
		}
	}
	
	Mat_Close(fp);
	return 0;
}
