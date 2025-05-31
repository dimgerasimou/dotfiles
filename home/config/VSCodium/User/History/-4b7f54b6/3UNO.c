#include <stdint.h>
#include <stdio.h>
#include <matio.h>
#include <hdf5.h>
#include <omp.h>

#include "knnsearch.h"

static Matrix_t*
matiogetmat(const char *filepath, const char *varname)
{
	mat_t    *fp  = NULL;
	matvar_t *var = NULL;
	Matrix_t *ret = NULL;

	if (!(fp = Mat_Open(filepath, MAT_ACC_RDONLY)))
		die("Failed to open .mat file", filepath);

	if (!(var = Mat_VarRead(fp, varname)))
		die("Failed to get variable", varname);

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
		die("Unsupported data type for variable", varname);
	}
	
	Mat_VarFree(var);
	Mat_Close(fp);
	return ret;
}

static Matrix_t*
hdf5getmat(const char *filepath, const char *varname)
{
	hid_t file_id, dataset_id, dataspace_id, datatype_id;
	H5T_class_t type_class;
	hsize_t dims[2] = {0, 0};
	herr_t status;
	Matrix_t *ret = NULL;

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
	printf("%lu %lu\n", dims[0], dims[1]);

	switch (type_class) {
	case H5T_INTEGER:
		if (H5Tget_size(datatype_id) == 8) {
			ret = matrixinit(dims[0], dims[1], INT64);
			status = H5Dread(dataset_id, H5T_NATIVE_LONG, H5S_ALL, H5S_ALL, H5P_DEFAULT, (int64_t *) ret->data);
			if (status < 0)
				die("Error reading hdf5 data");
		} else if (H5Tget_size(datatype_id) == 4) {
			ret = matrixinit(dims[0], dims[1], INT32);
			status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, (int32_t *) ret->data);
			if (status < 0)
				die("Error reading hdf5 data");
		} else if (H5Tget_size(datatype_id) == 2) {
			ret = matrixinit(dims[0], dims[1], INT16);
			status = H5Dread(dataset_id, H5T_NATIVE_SHORT, H5S_ALL, H5S_ALL, H5P_DEFAULT, (int16_t *) ret->data);
			if (status < 0)
				die("Error reading hdf5 data");
		}
		break;
	case H5T_FLOAT:
		if (H5Tget_size(datatype_id) == 4) {
			ret = matrixinit(dims[0], dims[1], SINGLE);
			status = H5Dread(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, (float *) ret->data);
			if (status < 0)
				die("Error reading hdf5 data");
		} else if (H5Tget_size(datatype_id) == 8) {
			ret = matrixinit(dims[0], dims[1], DOUBLE);
			status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (double *) ret->data);
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

static void
matiosave(const char *path, Matrix_t *idx, Matrix_t *dst)
{
	mat_t    *fp   = NULL;
	matvar_t *var  = NULL;
	void     *dst_temp;
	int32_t   *idx_temp;

	const size_t dims[2] = { idx->rows, idx->cols };

	if (!(fp = Mat_CreateVer(path, NULL, MAT_FT_MAT73))) {
		fprintf(stderr, "Error creating '%s' .mat variable\n", path);
		exit(1);
	}

	idx_temp = mmalloc(idx->rows * idx->cols * sizeof(int32_t));
	dst_temp = mmalloc(dst->rows * dst->cols * matrixtypesize(dst->type));
	
	switch(dst->type) {
	case DOUBLE:
		#pragma omp parallel for
		for (uint32_t i = 0; i < idx->rows; i++) {
			for (uint32_t j = 0; j < idx->cols; j++) {
				idx_temp[j * idx->rows + i] = (double) ((int32_t*) idx->data)[i * idx->cols + j] + 1;
				((double*) dst_temp)[j * idx->rows + i] = ((double*) dst->data)[i * idx->cols + j];
			}
		}
		var = Mat_VarCreate("distances", MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, dst_temp, MAT_F_DONT_COPY_DATA);
		break;

	case SINGLE:
		#pragma omp parallel for
		for (uint32_t i = 0; i < idx->rows; i++) {
			for (uint32_t j = 0; j < idx->cols; j++) {
				idx_temp[j * idx->rows + i] = ((int32_t*) idx->data)[i * idx->cols + j] + 1;
				((float*) dst_temp)[j * idx->rows + i] = ((float*) dst->data)[i * idx->cols + j];
			}
		}
		var = Mat_VarCreate("distances", MAT_C_SINGLE, MAT_T_SINGLE, 2, dims, dst_temp, MAT_F_DONT_COPY_DATA);
		break;

	default:
		var = 0;
		die("Invalid 'distances' matrix type");
	}

	if (!var) {
		fprintf(stderr, "Error creating the variable 'distances'\n");
		exit(1);
	}

	if (Mat_VarWrite(fp, var, MAT_COMPRESSION_NONE) != 0) {
		fprintf(stderr, "Error writing the variable 'distances' to the file '%s'\n", path);
		Mat_VarFree(var);
		Mat_Close(fp);
		exit(1);
	}

	Mat_VarFree(var);

	var = Mat_VarCreate("neighbors", MAT_C_INT32, MAT_T_INT32, 2, dims, idx_temp, MAT_F_DONT_COPY_DATA);

	if (!var) {
		fprintf(stderr, "Error creating the variable 'neighbors'\n");
		exit(1);
	}

	if (Mat_VarWrite(fp, var, MAT_COMPRESSION_NONE) != 0) {
		fprintf(stderr, "Error writing the variable 'neighbors' to the file '%s'\n", path);
		Mat_VarFree(var);
		Mat_Close(fp);
		exit(1);
	}

	Mat_VarFree(var);
	Mat_Close(fp);
	free(dst_temp);
	free(idx_temp);
}

static void
hdf5save(const char *path, const Matrix_t *idx, const Matrix_t *dst)
{
	hid_t file_id, dataset_id, dataspace_id, datatype_id;
	herr_t status;
	hsize_t dims[2] = {idx->rows, idx->cols};

	if ((file_id = H5Fcreate(path, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0)
		die("Error creating HDF5 file");
	
	switch (dst->type) {
	case INT64:
		datatype_id = H5T_NATIVE_LONG;
		break;
	case INT32:
		datatype_id = H5T_NATIVE_INT;
		break;
	case INT16:
		datatype_id = H5T_NATIVE_SHORT;
		break;
	case SINGLE:
		datatype_id = H5T_NATIVE_FLOAT;
		break;
	case DOUBLE:
		datatype_id = H5T_NATIVE_DOUBLE;
		break;
	default:
		H5Fclose(file_id);
		die("Unsupported matrix data type");
	}

	dataspace_id = H5Screate_simple(2, dims, NULL);
	if (dataspace_id < 0) {
		H5Fclose(file_id);
		die("Error creating HDF5 dataspace");
	}

	dataset_id = H5Dcreate(file_id, "distances", datatype_id, dataspace_id,
	                       H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	if (dataset_id < 0) {
		H5Sclose(dataspace_id);
		H5Fclose(file_id);
		die("Error creating HDF5 dataset");
	}

	status = H5Dwrite(dataset_id, datatype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, dst->data);
	if (status < 0) {
		H5Dclose(dataset_id);
		H5Sclose(dataspace_id);
		H5Fclose(file_id);
		die("Error writing data to HDF5 file");
	}

	H5Dclose(dataset_id);

	dataset_id = H5Dcreate(file_id, "neighbors", H5T_NATIVE_INT, dataspace_id,
	                       H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	if (dataset_id < 0) {
		H5Sclose(dataspace_id);
		H5Fclose(file_id);
		die("Error creating HDF5 dataset");
	}

	status = H5Dwrite(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, idx->data);
	if (status < 0) {
		H5Dclose(dataset_id);
		H5Sclose(dataspace_id);
		H5Fclose(file_id);
		die("Error writing data to HDF5 file");
	}

	H5Dclose(dataset_id);
	H5Sclose(dataspace_id);
	H5Fclose(file_id);
}

void
matrixsave(IOHeader header, Matrix_t *idx, Matrix_t *dst)
{
	if (header.type == MAT)
		matiosave(header.path, idx, dst);
	if (header.type == HDF5)
		hdf5save(header.path, idx, dst);
}

Matrix_t*
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
