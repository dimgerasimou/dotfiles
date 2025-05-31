#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>

int main(int argc, char *argv[]) {
	char *execpath;
	char *dirpath;

	if (argc != 2)
		return 1;

	execpath = realpath(argv[0], NULL);
	dirpath = dirname(execpath);
	
	if (chdir(dirpath) || chdir("./..")) {
		perror("Error changing working directory");
		free(execpath);
		return 1;
	}

	printf("Changed working directory to: %s\n", getcwd());
	free(execpath);

	return 0;
}
