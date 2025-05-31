#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <float.h>
#include <sys/types.h>
#include <sys/stat.h>

const char *knncmd = "bin/%s/knnsearch";
const char *knnargs = " Datasets/random-numbers-c.mat Result-data/c-knnsearch-result.mat";

int isint(char *s) {
	for (char *ptr = s; *ptr != '\0'; ptr++) {
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
	struct stat sb;
	uint16_t blocks = 0;

	if (argc < 3) {
		fprintf(stderr, "Invalid number of arguments\n");
		return 1;
	}

	if (!isint(argv[2])) {
		fprintf(stderr, "Argument '%s' is not a positive int\n", argv[2]);
		return 1;
	}

	if (argc > 3 && isint(argv[3]))
		blocks = atoi(argv[3]);


	execpath = realpath(argv[0], NULL);
	dirpath = dirname(execpath);
	
	if (chdir(dirpath) || chdir("./../..")) {
		fprintf(stderr, "Error in changing working directory\n");
		free(execpath);
		return 1;
	}

	free(execpath);

	execpath = malloc(10240 * sizeof(char));
	sprintf(execpath, knncmd, argv[1]);

	if (!(stat(execpath, &sb) == 0 && sb.st_mode & S_IXUSR)) {
		fprintf(stderr, "File '%s' is not executable\n", execpath);
		return 2;
	}

	strcat(execpath, knnargs);

	for (int i = 0; i < atoi(argv[2]); i++) {
		printf("Iteration %d out of %d\n", i + 1, atoi(argv[2]));
		if (!(ep = popen(execpath, "r"))) {
			fprintf(stderr, "Failed to execute iteration %d of %s\n", i, execpath);
			free(execpath);
			return 3;
		}
		fscanf(ep, "%lf", &exectime);
		max = exectime > max ? exectime : max;
		min = exectime < min ? exectime : min;
		avg += exectime;
		pclose(ep);
	}

	printf("%s - Iterations:%d\nMax: %.10lf - Min: %.10lf - Avg: %.10lf\n", argv[1], atoi(argv[2]), max, min, (avg / (double) atoi(argv[2])));

	free(execpath);
	return 0;
}
