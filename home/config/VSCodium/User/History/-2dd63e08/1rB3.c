#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <float.h>

const char *knncmd = "bin/%s/knnsearch Datasets/random-numbers-c.mat Result-data/c-knnsearch-result.mat";

int main(int argc, char *argv[]) {
	char *execpath;
	char *dirpath;
	double max = 0, min = DBL_MAX, avg = 0, exectime;
	FILE *ep;

	if (argc != 3)
		return 1;

	execpath = realpath(argv[0], NULL);
	dirpath = dirname(execpath);
	
	if (chdir(dirpath) || chdir("./../..")) {
		perror("Error changing working directory");
		free(execpath);
		return 1;
	}
	free(execpath);

	execpath = malloc(10240 * sizeof(char));
	sprintf(execpath, knncmd, argv[1]);

	for (int i = 0; i < atoi(argv[2]); i++) {
		printf("\033[2K");
		printf("Execution %d out of %d\n", i, atoi(argv[2]));
		if (!(ep = popen(execpath, "r"))) {
			fprintf(stderr, "Failed to execute iteration %d of %s\n", i, execpath);
			return 2;
		}
		fscanf(ep, "%lf", &exectime);
		max = exectime > max ? exectime : max;
		min = exectime < min ? exectime : min;
		avg++;
		pclose(ep);
	}

	printf("%s:\nMax: %12.10lf - Min: %12.10lf - Avg: %12.10lf\n", execpath, max, min, avg / atoi(argv[2]));

	free(execpath);
	return 0;
}
