#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "knnsearch.h"

static int
isuint(char *s)
{
	for (char *ptr = s; *ptr != '\0'; ptr++) {
		if (*ptr > '9' || *ptr < '0')
			return 0;
	}

	if (!strcmp(s, "0"))
		return 0;

	return 1;
}

static const char*
getextention(const char *path)
{
	const char *slash = strrchr(path, '/');
	const char *filename = (slash) ? slash + 1 : path;

	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) {
		return "";
	}
	return dot + 1;
}

static int
parsefile(const char *path, IOHeader *header)
{
	const char *extention = getextention(path);

	if (!strcmp(extention, "mat"))
		header->type = MAT;
	else if (!strcmp(extention, "h5") || !strcmp(extention, "hdf5"))
		header->type = HDF5;
	else
		return 1;

	header->path = strdup(path);
	return 0;
}

static void
parseargs(int argc, char *argv[], IOHeader *input, IOHeader *output, uint32_t *K, uint32_t *blocksize)
{
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-i")) {
			if (argc < i + 1)
				die("Invalid number of arguments");
			if (parsefile(argv[++i], input))
				die("Invalid filetype");
		} else if (!strcmp(argv[i], "-o")) {
			if (argc < i + 1)
				die("Invalid number of arguments");
			if (parsefile(argv[++i], output))
				die("Invalid filetype");
		} else if (!strcmp(argv[i], "-b")) {
			if (argc < i + 1)
				die("Invalid number of arguments");
			if (isuint(argv[++i]))
				*blocksize = atoi(argv[i]);
		} else if (!strcmp(argv[i], "-k")) {
			if (argc < i + 1)
				die("Invalid number of arguments");
			if (isuint(argv[++i]))
				*K = atoi(argv[i]);
		} else if (!strcmp(argv[i], "-t")) {
			if (argc < i + 1)
				die("Invalid number of arguments");
			if (isuint(argv[++i]))
				omp_set_num_threads(atoi(argv[i]));
		} else {
			die("Invalid argument");
		}
	}

	if (!input->type || !output->type)
		die("Did not specify input - output files");
}

int
main(int argc, char *argv[])
{
	struct timeval t0, t1;
	uint32_t K = 0, blocksize = 0;
	Matrix_t *C = NULL, *Q = NULL;
	Matrix_t **neighbors = NULL;
	IOHeader input = {0, NULL}, output = {0, NULL};

	parseargs(argc, argv, &input, &output, &K, &blocksize);

	if (input.type == HDF5) {
		if (!K)
			die("Enter a K value");
		C = matrixget(input, "train");
		Q = matrixget(input, "test");
	} else if(input.type == MAT) {
		C = matrixget(input, "C");
		Q = matrixget(input, "Q");
		if (!K)
			K = uintget(input, "K");
	} else {
		die("Did not initialize properly");
	}

	printf("%u %u %u\n", C->rows, Q->rows, C->cols);

	gettimeofday(&t0, NULL);

	// benchmarked function here
	neighbors = knnsearch(C, Q, &K, blocksize);

	gettimeofday(&t1, NULL);
	
	printf("%.8g\n", t1.tv_sec - t0.tv_sec + 1E-6 * (t1.tv_usec - t0.tv_usec));

	matrixtosingle(neighbors[1]);

	matrixsave(output, neighbors[0], neighbors[1]);

	matrixfree(&neighbors[0]);
	matrixfree(&neighbors[1]);
	free(neighbors);

	matrixfree(&C);
	matrixfree(&Q);

	return 0;
}
