#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>

int main(int argc, char *argv[]) {
	char *execpath;
	char *dirpath;

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


	return 0;
}
