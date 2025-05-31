#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <float.h>

const char *knncmd = "bin/%s/knnsearch Datasets/random-numbers-c.mat Result-data/c-knnsearch-result.mat";

int isint(char *s) {
	for (char *ptr = s; ptr == NULL; ptr++) {
		if (*ptr > '9' || *ptr < '0')
			return 0;
	}
	if (!strcmp(s, "0"))
		return 0;
	return 1;
}

int main(int argc, char *argv[]) {
	char *execpath;
	char *dirpath;
	FILE *ep;
	double max = 0, min = DBL_MAX;
	double avg = 0, exectime;

	if (argc != 3) {
		fprintf(stderr, "Invalid number of arguments\n");
		return 1;
	}

	execpath = realpath(argv[0], NULL);
	dirpath = dirname(execpath);
	
	if (chdir(dirpath) || chdir("./../..")) {
		fprintf(stderr, "Error changing working directory\n");
		free(execpath);
		return 1;
	}
	free(execpath);

	execpath = malloc(10240 * sizeof(char));
	sprintf(execpath, knncmd, argv[1]);

	for (int i = 0; i < atoi(argv[2]); i++) {
		printf("Iteration %d out of %d\n", i + 1, atoi(argv[2]));
		if (!(ep = popen(execpath, "r"))) {
			fprintf(stderr, "Failed to execute iteration %d of %s\n", i, execpath);
			free(execpath);
			return 2;
		}
		fscanf(ep, "%lf", &exectime);
		max = exectime > max ? exectime : max;
		min = exectime < min ? exectime : min;
		avg += exectime;
		pclose(ep);
	}

	printf("%s - Iterations:%d\nMax: %12.10lf - Min: %12.10lf - Avg: %12.10lf\n", argv[1], atoi(argv[2]), max, min, (avg / (double) atoi(argv[2])));

	free(execpath);
	return 0;
}
