#include <stdint.h>
#include <stdio.h>
#include <matio.h>

#include "knnsearch.h"

Matrix*
matiogetmat(const char *filepath, const char *varname)
{
	mat_t    *fp  = NULL;
	matvar_t *var = NULL;
	Matrix   *ret;
	double   *data;

	if (!(fp = Mat_Open(filepath, MAT_ACC_RDONLY))) {
		fprintf(stderr, "Couldn't open '%s' .mat file\n", filepath);
		exit(1);
	}

	var = Mat_VarRead(fp, varname);
	if (!var || var->data_type != MAT_T_DOUBLE) {
		fprintf(stderr, "Couldn't get variable '%s'\n", varname);
		exit(1);
	}

	ret = matrixinit(var->dims[0], var->dims[1]);
	data = (double*) var->data;

	for (uint32_t i = 0; i < var->dims[0]; i++) {
		for (uint32_t j = 0; j < var->dims[1]; j++) {
			ret->data[i][j] = data[var->dims[0] * j + i];
		}
	}
	
	Mat_VarFree(var);
	Mat_Close(fp);
	return ret;
}

uint32_t
matiogetuint(const char *filepath, const char *varname)
{
	mat_t    *fp  = NULL;
	matvar_t *var = NULL;
	int      result = 0;


	if (!(fp = Mat_Open(filepath, MAT_ACC_RDONLY))) {
		fprintf(stderr, "Couldn't open '%s' .mat file\n", filepath);
		exit(1);
	}

	var = Mat_VarRead(fp, varname);
	if (!var) {
		fprintf(stderr, "Couldn't get variable '%s'\n", varname);
		exit(1);
	}

	if (var->data_type == MAT_T_INT32) {
		result = ((int32_t *)var->data)[0];
	} else if (var->data_type == MAT_T_INT16) {
		result = ((int16_t *)var->data)[0];
	} else {
		fprintf(stderr, "Varabile '%s', wrong filetype\n", varname);
		exit(1);
	}

	Mat_VarFree(var);
	Mat_Close(fp);
	return result;
}

int
matrixsave(Matrix *idx, Matrix *dst, const char *filepath)
{
	mat_t    *fp  = NULL;
	matvar_t *var = NULL;
	double   *data;

	const size_t dims[2] = { idx->rows, idx->cols };
	data = mmalloc(dims[0] * dims[1] * sizeof(double));

	if (!(fp = Mat_CreateVer(filepath, NULL, MAT_FT_MAT5))) {
		fprintf(stderr, "Error creating '%s' .mat variable\n", filepath);
		free(data);
		return 1;
	}

	for (uint32_t i = 0; i < dims[0]; i++) {
		for (uint32_t j = 0; j < dims[1]; j++) {
			data[dims[0] * j + i] = idx->data[i][j];
		}
	}

	var = Mat_VarCreate("idx", MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, idx->data, MAT_F_DONT_COPY_DATA);

	if (!var) {
		fprintf(stderr, "Error creating the variable 'idx'\n");
		free(data);
		return 1;
	}

	if (Mat_VarWrite(fp, var, MAT_COMPRESSION_NONE) != 0) {
		fprintf(stderr, "Error writing the variable 'idx' to the file '%s'\n", filepath);
		Mat_VarFree(var);
		Mat_Close(fp);
		free(data);
		return 1;
	}

	Mat_VarFree(var);

	var = Mat_VarCreate("dst", MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, dst->data, MAT_F_DONT_COPY_DATA);
	if (!var) {
		fprintf(stderr, "Error creating the variable 'dst'\n");
		free(data);
		return 1;
	}

	if (Mat_VarWrite(fp, var, MAT_COMPRESSION_NONE) != 0) {
		fprintf(stderr, "Error writing the variable 'dst' to the file '%s'\n", filepath);
		Mat_VarFree(var);
		free(data);
		Mat_Close(fp);
		return 1;
	}

	Mat_VarFree(var);
	Mat_Close(fp);
	free(data);
	return 0;
}
