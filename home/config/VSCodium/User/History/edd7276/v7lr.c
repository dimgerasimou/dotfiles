#include <stdio.h>
#include <matio.h>

void freeMatioData(mat_t **fp1, mat_t **fp2, matvar_t **var1, matvar_t **var2) {
	if (*var1) {
		Mat_VarFree(*var1);
		*var1 = NULL;
	}
	if (*var2) {
		Mat_VarFree(*var2);
		*var2 = NULL;
	}
	if (*fp1) {
		Mat_Close(*fp1);
		*fp1 = NULL;
	}
	if (*fp2) {
		Mat_Close(*fp2);
		*fp2 = NULL;
	}
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		fprintf(stderr, "Invalid arguments.\n");
		return 1;
	}

	mat_t *fp1 = Mat_Open(argv[1], MAT_ACC_RDONLY);
	mat_t *fp2 = Mat_Open(argv[2], MAT_ACC_RDONLY);

	if (!fp1 || !fp2) {
		fprintf(stderr, "Could not open .mat files\n");
		freeMatioData(&fp1, &fp2, NULL, NULL);
		return 1;
	}

	for (int i = 3; i < argc; i++) {
		matvar_t *var1 = NULL, *var2 = NULL;
		size_t nelems = 0;
		int match = 0;

		var1 = Mat_VarRead(fp1, argv[i]);
		var2 = Mat_VarRead(fp2, argv[i]);

		if (!var1 || !var2) {
			fprintf(stderr, "Could not read the selected variable: '%s'\n", argv[i]);
			freeMatioData(&fp1, &fp2, &var1, &var2);
			return 1;
		}

		if (var1->data_type != var2->data_type || var1->nbytes != var2->nbytes) {
			printf("'%s' matrices' data types or size do not match\n", argv[i]);
			freeMatioData(NULL, NULL, &var1, &var2);
			continue;
		}


		nelems = var1->nbytes / var1->data_size;

		if (var1->data_type == MAT_T_DOUBLE) {
			double *d1 = (double*) var1->data;
			double *d2 = (double*) var2->data;
			for (size_t i = 0; i < nelems; i++) {
				if (d1[i] == d2[i])
					continue;
				if ((d1[i] - d2[i] < 0.0001) || (d2[i] - d1[i] < 0.0001)) {
					match = 1;
					continue;
				}
				match = 2;
				break;
			}
		}

		switch (match) {
		case 0:
			printf("'%s' matrices are a perfect match\n", argv[i]);
			break;
		case 1:
			printf("'%s' matrices are a match up to 0.0001 precision\n", argv[i]);
			break;
		default:
			printf("'%s' matrices are not a match\n", argv[i]);
			break;
		}

		freeMatioData(NULL, NULL, &var1, &var2);
	}

	freeMatioData(&fp1, &fp2, NULL, NULL);
	return 0;
}
