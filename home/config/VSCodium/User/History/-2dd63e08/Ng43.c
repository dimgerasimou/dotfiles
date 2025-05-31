#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <float.h>

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

	execpath = malloc((15 + strlen(argv[1])) * sizeof(char));
	sprintf(execpath, "bin/%s/knnsearch", argv[1]);

	for (int i = 0; i < atoi(argv[2]); i++) {
		if (!(ep = popen(execpath, "r"))) {
			fprintf(stderr, "Failed to execute iteration %d of %s\n", i, execpath);
			exit(2);
		}
		fscanf(ep, "%lf", &exectime);
		max = exectime > max ? exectime : max;
		min = exectime < min ? exectime : min;
		avg++;
		pclose(ep);
	}

	return 0;
}
