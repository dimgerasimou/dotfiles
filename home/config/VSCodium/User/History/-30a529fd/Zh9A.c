#include <stdint.h>
#include <stdio.h>
#include <matio.h>
#include <omp.h>

#include "knnsearch.h"

Matrix*
matiogetmat(const char *filepath, const char *varname)
{
	mat_t    *fp  = NULL;
	matvar_t *var = NULL;
	Matrix   *ret;

	if (!(fp = Mat_Open(filepath, MAT_ACC_RDONLY))) {
		fprintf(stderr, "Couldn't open '%s' .mat file\n", filepath);
		exit(1);
	}

	var = Mat_VarRead(fp, varname);
	if (!var) {
		fprintf(stderr, "Couldn't get variable '%s'\n", varname);
		exit(1);
	}

	ret = matrixinit(var->dims[0], var->dims[1]);

	switch (var->data_type) {
	case MAT_T_DOUBLE: {
		double *data = (double*) var->data;
		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				ret->data[i * var->dims[1] + j] = data[j * var->dims[0] + i];
			}
		}
		break;
	}
	case MAT_T_SINGLE: {
		float *data = (float*) var->data;
		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				ret->data[i * var->dims[1] + j] = (double) data[j * var->dims[0] + i];
			}
		}
		break;
	}
	case MAT_T_INT64: {
		int64_t *data = (int64_t*) var->data;
		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				ret->data[i * var->dims[1] + j] = (double) data[j * var->dims[0] + i];
			}
		}
		break;
	}
	case MAT_T_INT32: {
		int32_t *data = (int32_t*) var->data;
		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				ret->data[i * var->dims[1] + j] = (double) data[j * var->dims[0] + i];
			}
		}
		break;
	}
	case MAT_T_INT16: {
		int16_t *data = (int16_t*) var->data;
		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				ret->data[i * var->dims[1] + j] = (double) data[j * var->dims[0] + i];
			}
		}
		break;
	}
	case MAT_T_UINT64: {
		uint64_t *data = (uint64_t*) var->data;
		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				ret->data[i * var->dims[1] + j] = (double) data[j * var->dims[0] + i];
			}
		}
		break;
	}
	case MAT_T_UINT32: {
		uint32_t *data = (uint32_t*) var->data;
		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				ret->data[i * var->dims[1] + j] = (double) data[j * var->dims[0] + i];
			}
		}
		break;
	}
	case MAT_T_UINT16: {
		uint16_t *data = (uint16_t*) var->data;
		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				ret->data[i * var->dims[1] + j] = (double) data[j * var->dims[0] + i];
			}
		}
		break;
	}
	default:
		fprintf(stderr, "Unsupported data type for variable '%s'\n", varname);
		exit(1);
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

	switch (var->data_type) {
	case MAT_T_INT32:
		result = (uint32_t) ((int32_t *)var->data)[0];
		break;
	case MAT_T_INT16:
		result = (uint32_t) ((int16_t *)var->data)[0];
		break;
	case MAT_T_UINT32:
		result = ((uint32_t *)var->data)[0];
		break;
	case MAT_T_UINT16:
		result = (uint32_t) ((uint16_t *)var->data)[0];
		break;
	case MAT_T_DOUBLE:
		result = (uint32_t) ((double *)var->data)[0];
		break;
	case MAT_T_SINGLE:
		result = (uint32_t) ((float *)var->data)[0];
		break;
	default:
		fprintf(stderr, "Variable '%s' has unsupported data type\n", varname);
		exit(1);
	}

	Mat_VarFree(var);
	Mat_Close(fp);
	return result;
}

int
matrixsave(Matrix *idx, Matrix *dst, const char *filepath)
{
	mat_t    *fp   = NULL;
	matvar_t *var  = NULL;

	const size_t dims[2] = { idx->rows, idx->cols };

	if (!(fp = Mat_CreateVer(filepath, NULL, MAT_FT_MAT73))) {
		fprintf(stderr, "Error creating '%s' .mat variable\n", filepath);
		return 1;
	}

	var = Mat_VarCreate("idx", MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, idx->data, MAT_F_DONT_COPY_DATA);

	if (!var) {
		fprintf(stderr, "Error creating the variable 'idx'\n");
		return 1;
	}

	if (Mat_VarWrite(fp, var, MAT_COMPRESSION_NONE) != 0) {
		fprintf(stderr, "Error writing the variable 'idx' to the file '%s'\n", filepath);
		Mat_VarFree(var);
		Mat_Close(fp);
		return 1;
	}

	Mat_VarFree(var);

	var = Mat_VarCreate("dst", MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, dst->data, MAT_F_DONT_COPY_DATA);
	if (!var) {
		fprintf(stderr, "Error creating the variable 'dst'\n");
		return 1;
	}

	if (Mat_VarWrite(fp, var, MAT_COMPRESSION_NONE) != 0) {
		fprintf(stderr, "Error writing the variable 'dst' to the file '%s'\n", filepath);
		Mat_VarFree(var);
		Mat_Close(fp);
		return 1;
	}

	Mat_VarFree(var);
	Mat_Close(fp);
	return 0;
}

Matrix*
matrixget(IOHeader header, const char *varname)
{
	if (header.type == MAT)
		return matiogetmat(header.path, varname);

	die("Error with variable type");
	return NULL;
}

/*
 * loads an unsigned integer from a variable saved in a .mat file.
 */
uint32_t
uintget(IOHeader header, const char *varname)
{
	if (header.type == MAT)
		return matiogetuint(header.path, varname);

	die("Error with variable type");
	return 0;
}