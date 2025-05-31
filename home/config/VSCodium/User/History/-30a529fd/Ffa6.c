#include <stdint.h>
#include <stdio.h>
#include <matio.h>
#include <hdf5.h>
#include <omp.h>

#include "knnsearch.h"

static Matrix*
matiogetmat(const char *filepath, const char *varname)
{
	mat_t    *fp  = NULL;
	matvar_t *var = NULL;
	Matrix   *ret = NULL;

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
	case MAT_T_DOUBLE: {
		ret = matrixinit(var->dims[0], var->dims[1], DOUBLE);
		double *data = (double*) var->data;

		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				((double*)ret->data)[i * var->dims[1] + j] = data[j * var->dims[0] + i];
			}
		}
		break;
	}
	case MAT_T_SINGLE: {
		ret = matrixinit(var->dims[0], var->dims[1], SINGLE);
		float *data = (float*) var->data;

		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				((float*)ret->data)[i * var->dims[1] + j] = data[j * var->dims[0] + i];
			}
		}
		break;
	}
	case MAT_T_INT64: {
		ret = matrixinit(var->dims[0], var->dims[1], INT64);
		int64_t *data = (int64_t*) var->data;

		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				((int64_t*)ret->data)[i * var->dims[1] + j] = data[j * var->dims[0] + i];
			}
		}
		break;
	}
	case MAT_T_INT32: {
		ret = matrixinit(var->dims[0], var->dims[1], INT32);
		int32_t *data = (int32_t*) var->data;

		#pragma omp parallel for
		for (uint32_t i = 0; i < var->dims[0]; i++) {
			for (uint32_t j = 0; j < var->dims[1]; j++) {
				((int32_t*)ret->data)[i * var->dims[1] + j] = data[j * var->dims[0] + i];
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

static Matrix*
hdf5getmat(const char *filepath, const char *varname)
{
	hid_t file_id, dataset_id, dataspace_id, datatype_id;
	H5T_class_t type_class;
	hsize_t dims[2] = {0, 0};
	herr_t status;
	Matrix *ret;

	if ((file_id = H5Fopen(filepath, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)
		die("Invalid filepath");

	if ((dataset_id = H5Dopen(file_id, varname, H5P_DEFAULT)) < 0) {
		H5Fclose(file_id);
		die("Invalid filepath");
	}

	dataspace_id = H5Dget_space(dataset_id);
	H5Sget_simple_extent_dims(dataspace_id, dims, NULL);

	datatype_id = H5Dget_type(dataset_id);
	type_class = H5Tget_class(datatype_id);

	ret = matrixinit(dims[0], dims[1]);

	switch (type_class) {
	case H5T_INTEGER:
		if (H5Tget_size(datatype_id) == 8) {
			int64_t *temp_data = (int64_t *)malloc(dims[0] * dims[1] * sizeof(int));
			status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, temp_data);
			if (status < 0) {
				free(temp_data);
				die("Error reading hdf5 data");
			}
			for (hsize_t i = 0; i < dims[0] * dims[1]; i++)
				ret->data[i] = (double)temp_data[i];
			free(temp_data);
		} else if (H5Tget_size(datatype_id) == 4) {
			int32_t *temp_data = (int32_t *)malloc(dims[0] * dims[1] * sizeof(int));
			status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, temp_data);
			if (status < 0) {
				free(temp_data);
				die("Error reading hdf5 data");
			}
			for (hsize_t i = 0; i < dims[0] * dims[1]; i++)
				ret->data[i] = (double)temp_data[i];
			free(temp_data);
		} else if (H5Tget_size(datatype_id) == 2) {
			int16_t *temp_data = (int16_t *)malloc(dims[0] * dims[1] * sizeof(short));
			status = H5Dread(dataset_id, H5T_NATIVE_SHORT, H5S_ALL, H5S_ALL, H5P_DEFAULT, temp_data);
			if (status < 0) {
				free(temp_data);
				die("Error reading hdf5 data");
			}
			for (hsize_t i = 0; i < dims[0] * dims[1]; i++)
				ret->data[i] = (double)temp_data[i];
			free(temp_data);
		}
		break;
	case H5T_FLOAT:
		if (H5Tget_size(datatype_id) == 4) {  // 32-bit float
			float *temp_data = (float *)malloc(dims[0] * dims[1] * sizeof(float));
			status = H5Dread(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, temp_data);
			if (status < 0) {
				free(temp_data);
				die("Error reading hdf5 data");
			}
			for (hsize_t i = 0; i < dims[0] * dims[1]; i++)
				ret->data[i] = (double)temp_data[i];
			free(temp_data);
		} else if (H5Tget_size(datatype_id) == 8) {  // 64-bit double
			status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ret->data);
			if (status < 0)
				die("Error reading hdf5 data");
		}
		break;

	case H5T_STRING:
		die("Error reading hdf5 data, variable has type 'string'");
		break;

	default:
		die("Unsupported data type");
	}
	

	H5Tclose(datatype_id);
	H5Sclose(dataspace_id);
	H5Dclose(dataset_id);
	H5Fclose(file_id);
	return ret;
}

static uint32_t
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
	if (header.type == HDF5)
		return hdf5getmat(header.path, varname);

	die("Error with variable type");
	return NULL;
}

uint32_t
uintget(IOHeader header, const char *varname)
{
	if (header.type == MAT)
		return matiogetuint(header.path, varname);

	die("Error with variable type");
	return 0;
}
