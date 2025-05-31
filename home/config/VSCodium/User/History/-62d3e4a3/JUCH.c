#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "knnsearch.h"


void
parseargs(int argc, char *argv[], IOHeader *input, IOHeader *output, uint32_t *K)
{
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-i")) {
			if (argc < i + 2)
				die("Invalid number of arguments");

			if (!strcmp(argv[++i], "mat"))
				input->type = MAT;
			else if
				(!strcmp(argv[i], "hdf5")) input->type = HDF5;
			else
				die("Invalid arguments");

			input->path = strdup(argv[++i]);
		} else if (!strcmp(argv[i], "-o")) {
			if (argc < i + 2)
				die("Invalid number of arguments");

			if (!strcmp(argv[++i], "mat"))
				output->type = MAT;
			else if
				(!strcmp(argv[i], "hdf5")) output->type = HDF5;
			else
				die("Invalid arguments");

			output->path = strdup(argv[++i]);
		} else if (!strcmp(argv[i], "-b")) {
			if (argc < i + 1)
				die("Invalid number of arguments");
			if (isuint(argv[++i]))
				g_blocksize = atoi(argv[i]);
		} else if (!strcmp(argv[i], "-k")) {
			if (argc < i + 1)
				die("Invalid number of arguments");
			if (isuint(argv[++i]))
				*K = atoi(argv[i]);
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
	Neighbor *neighbors;
	uint32_t K = 0;
	Matrix   *C = NULL;
	Matrix   *Q = NULL;
	IOHeader input = {0, NULL}, output = {0, NULL};

	parseargs(argc, argv, &input, &output, &K);

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

	if (!g_blocksize)
		dynamic_blocksize(sizeof(double), C->cols, C->rows * Q->rows);

	gettimeofday(&t0, NULL);

	// benchmarked function here
	neighbors = knnsearch(C, Q,&K);

	gettimeofday(&t1, NULL);
	
	printf("%.8g\n", t1.tv_sec - t0.tv_sec + 1E-6 * (t1.tv_usec - t0.tv_usec));

	neighborarrsave(neighbors, Q->rows, K, argv[2]);
	
	free(neighbors);
	matrixfree(&C);
	matrixfree(&Q);
	return 0;
}
