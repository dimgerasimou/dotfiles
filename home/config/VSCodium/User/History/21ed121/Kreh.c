/* This is an application menu for dmenu.
 * Works by parsing the .desktop files at \usr\share\applications.
 * Arguments are passed through to dmenu, all applications matching the names to
 * $XDG_CONFIG_HOME/dmenu/ignoreapplications will be ignored from being included in the menu
 * plus all hidden and terminal applications. The application writes all entries and crosschecks
 * them every execution at $XDG_CACHE_HOME/dmenu/applicationlist bringing the most recently
 * launched app at the top of the list.
*/

#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUF_SIZE 1024

typedef struct {
	char *name;
	char *exec;
} Exec;

typedef struct {
	Exec **i;
	size_t ni;
} ExecArray;

const char *ignorepath[] = {"$XDG_CONFIG_HOME", "dmenu", "ignoreapplications", NULL};
const char *cachepath[] = {"$XDG_CACHE_HOME", "dmenu", "applicationlist"};
const char *entriespath[] = {"usr", "share", "applications"};

static void addexecentry(ExecArray *arr, const char *path);
static void deleteexec(Exec *exec);
static void execute(Exec *item);
static void freeexecarray(ExecArray *arr);
static int getdmenuint(const char *menu, char *argv[]);
static char* getentrypath(const char *path, const char *name);
static ExecArray* getexec(void);
static char* getexecstring(ExecArray *arr);
static int getindex(ExecArray *arr, char *argv[]);
static char *getpath(const char **arr);
static int isfile(const char *path);
static int ishiddenentry(FILE *fp);
static void mkparentdir(const char *path);
static void parseexecentry(ExecArray *arr, FILE *fp);
static void removeentry(ExecArray *arr, const char *name);
static void removeignored(ExecArray *arr);
static void setexecorder(ExecArray *arr);
static void swapexec(ExecArray *arr, const size_t indexa, const size_t indexb);
static void trimwhitespace(char *str);
static void writeselection(ExecArray *arr, const int selection);

static void
addexecentry(ExecArray *arr, const char *path)
{
	FILE *fp;

	if (!isfile(path))
		return;

	if (!(fp = fopen(path, "r")))
		return;

	if (ishiddenentry(fp)) {
		fclose(fp);
		return;
	}

	parseexecentry(arr, fp);
	fclose(fp);
}

static void
deleteexec(Exec *exec)
{
	if (!exec)
		return;
	if (exec->exec)
		free(exec->exec);
	if (exec->name)
		free(exec->name);
	free(exec);
}

static void
execute(Exec *item)
{
	switch (fork()) {
	case -1:
		perror("fork() failed");
		exit(errno);

	case 0:
		setsid();
		execl("/bin/sh", "sh", "-c", item->exec, NULL);
		exit(errno);

	default:
		break;
	}
}

static void
freeexecarray(ExecArray *arr)
{
	if (!arr)
		return;
	for (size_t i = 0; i < arr->ni; i++) 
		deleteexec(arr->i[i]);
	free(arr->i);
	free(arr);
}

static int
getdmenuint(const char *menu, char *argv[])
{
	int option;
	int writepipe[2];
	int readpipe[2];
	char buffer[BUF_SIZE];
	char **args;
	char *ptr;
	size_t argc = 0;

	for (char **ptr = argv; *ptr != NULL; ptr++)
		argc++;

	if (!(args = malloc((argc + 1) * sizeof (char*)))) {
		perror("malloc() failed");
		exit(errno);
	}

	args[0] = strdup("dmenu");
	for (size_t i = 1; i < argc; i++) {
		args[i] = strdup(argv[i]);
	}
	args[argc] = NULL;

	option = -EREMOTEIO;
	buffer[0] = '\0';

	if (pipe(writepipe) < 0 || pipe(readpipe) < 0) {
		perror("pipe() failed");
		exit(errno);
	}
	
	switch (fork()) {
	case -1:
		perror("fork() failed");
		exit(errno);

	case 0: /* child - prompt application */
		close(writepipe[1]);
		close(readpipe[0]);

		dup2(writepipe[0], STDIN_FILENO);
		close(writepipe[0]);

		dup2(readpipe[1], STDOUT_FILENO);
		close(readpipe[1]);

		execv("/usr/local/bin/dmenu", args);
		exit(EXIT_FAILURE);

	default: /* parent */
		close(writepipe[0]);
		close(readpipe[1]);

		write(writepipe[1], menu, strlen(menu) + 1);
		close(writepipe[1]);

		wait(NULL);

		read(readpipe[0], buffer, sizeof(buffer));
		close(readpipe[0]);
	}
	ptr = strchr(buffer, '\t');
	if (ptr != NULL)
		sscanf(ptr, "%d", &option);

	for (char **ptr = args; *ptr != NULL; ptr++)
		if (*ptr)
			free(*ptr);
	free(args);
	return option;
}

static char*
getentrypath(const char *path, const char *name)
{
	char *ret;
	size_t len;

	len = (strlen(path) + strlen(name) + 3);
	if (!(ret = malloc(len * sizeof(char))))
		return NULL;

	snprintf(ret, len - 1, "%s/%s", path, name);
	ret[len - 1] = '\0';
	return ret;
}

static ExecArray*
getexec(void)
{
	ExecArray *ret;
	char *path;
	DIR *dirp;
	struct dirent *dp;
	const char *basepath = getpath(entriespath);

	if (!(dirp = opendir(basepath))) {
		fprintf(stderr,"opendir() failed for: %s - %s", basepath, strerror(errno));
		exit(errno);
	}

	if (!(ret = malloc(sizeof(ExecArray)))) {
		perror("malloc() failed");
		exit(errno);
	}

	ret->i = NULL;
	ret->ni = 0;

	while ((dp = readdir(dirp))) {
		path = getentrypath(basepath, dp->d_name);
		addexecentry(ret, path);
		free(path);
	}

	closedir(dirp);

	removeignored(ret);
	return ret;
}

static char*
getexecstring(ExecArray *arr)
{
	char *ret = NULL;
	char *temp;
	char buf[BUF_SIZE];
	size_t len = 1;

	if (!(ret = malloc(sizeof(char)))) {
		perror("malloc() failed");
		exit(errno);
	}
	ret[0] = '\0';

	for (size_t i = 0; i < arr->ni; i++) {
		snprintf(buf, sizeof(buf) - 1, "%s\t%zu\n", arr->i[i]->name, i);
		buf[sizeof(buf) - 1] = '\0';
		len += strlen(buf);
		if (!(temp = realloc(ret, len * sizeof(char)))) {
			perror("realloc() failed");
			exit(errno);
		}
		strncat(temp, buf, len - strlen(temp) - 1);
		temp[len - 2] = '\n';
		temp[len - 1] = '\0';
		ret = temp;
	}
	if (ret[len - 2] == '\n')
		ret[len - 2] = '\0';
	return ret;
}

static int
getindex(ExecArray *arr, char *argv[])
{
	char *str = getexecstring(arr);
	int option = getdmenuint(str, argv);

	return option;
}

static char*
getpath(const char **arr)
{
	
	char *path;
	char *env;
	size_t spath = 1;
	
	if (!(path = malloc(sizeof(char)))) {
		perror("malloc() failed");
		exit(errno);
	}
	path[0] = '\0';

	for (int i = 0; arr[i] != NULL; i++) {
		if (arr[i][0] == '$') {
			if ((env = getenv(arr[i] + 1))) {
				spath += strlen(env);
				if(!(path = realloc(path, spath * sizeof(char)))) {
					perror("realloc() failed");
					exit(errno);
				}
				strcat(path, env);
				continue;
			}

			if (!strcmp(arr[i] + 1, "XDG_CONFIG_HOME")) {
				if (!(env = getenv("HOME"))) {
					fprintf(stderr, "ERROR - Failed to get env variable: HOME\n");
					exit(2);
				}
				spath += strlen(env) + strlen("/.config");
				if(!(path = realloc(path, spath * sizeof(char)))) {
					perror("realloc() failed");
					exit(errno);
				}
				strcat(path, env);
				strcat(path, "/.config");
				continue;
			}

			if (!strcmp(arr[i] + 1, "XDG_CACHE_HOME")) {
				if (!(env = getenv("HOME"))) {
					fprintf(stderr, "ERROR - Failed to get env variable: HOME\n");
					exit(2);
				}
				spath += strlen(env) + strlen("/.cache");
				if(!(path = realloc(path, spath * sizeof(char)))) {
					perror("realloc() failed");
					exit(errno);
				}
				strcat(path, env);
				strcat(path, "/.cache");
				continue;
			}

			fprintf(stderr, "ERROR - Failed to get env variable: %s\n", arr[i]);
			exit(2);
		} else {
			spath += strlen(arr[i]) + 1;
			if(!(path = realloc(path, spath * sizeof(char)))) {
				perror("realloc() failed");
				exit(errno);
			}
			strcat(path, "/");
			strcat(path, arr[i]);
		}
	}
	return path;
}

static int
isfile(const char *path)
{
	struct stat st;
	stat(path, &st);
	if (S_ISREG(st.st_mode))
		return 1;
	return 0;
}

static int
ishiddenentry(FILE *fp)
{
	char buf[BUF_SIZE];

	while (fgets(buf, sizeof(buf), fp)) {
		if (strstr(buf, "NotShowIn")      ||
		    strstr(buf, "OnlyShowIn")     ||
		    strstr(buf, "NoDisplay=true") ||
		    strstr(buf, "Terminal=true")) {
			return 1;
		}
	}
	rewind(fp);
	return 0;
}

static void
mkparentdir(const char *path)
{
	struct stat st;
	char *str;
	char *ptr;

	str = strdup(path);
	if ((ptr = strrchr(str, '/')))
		*ptr = '\0';

	if (stat(str, &st) == -1)
		mkdir(str, 0700);

	free(str);
}

static void
parseexecentry(ExecArray *arr, FILE *fp)
{
	Exec **temparr;
	char buf[BUF_SIZE];
	char *ptr;
	char *name = NULL;
	char *exec = NULL;

	while (fgets(buf, sizeof(buf), fp)) {
		trimwhitespace(buf);
		if (buf[0] == '#' || buf[0] == '\0')
			continue;
		if (strstr(buf, "[Desktop Action "))
			break;
		if ((ptr = strstr(buf, "Name=")) == buf && !name) {
			ptr = strchr(buf, '=');
			name = strdup(ptr + 1);
			continue;
		}
		if ((ptr = strstr(buf, "Exec=")) == buf && !exec) {
			if ((ptr = strrchr(buf, '%')))
				*ptr = '\0';
			ptr = strchr(buf, '=');
			exec = strdup(ptr + 1);
			continue;
		}
	}

	if (!name || !exec) {
		if (name)
			free(name);
		if (exec)
			free(exec);
		return;
	}

	arr->ni++;
	if (!(temparr = realloc(arr->i, arr->ni * sizeof(Exec*)))) {
		perror("realloc() failed");
		exit(errno);
	}

	if (!(temparr[arr->ni - 1] = malloc(sizeof(Exec)))) {
		perror("malloc() failed");
		exit(errno);
	}

	temparr[arr->ni - 1]->exec = exec;
	temparr[arr->ni - 1]->name = name;
	arr->i = temparr;
}

static void
removeentry(ExecArray *arr, const char *name)
{
	for (size_t i = 0; i < arr->ni; i++) {
		if (!strcmp(arr->i[i]->name, name)) {
			Exec **temp;

			deleteexec(arr->i[i]);
			for (size_t j = i; j < arr->ni - 1; j++) {
				arr->i[j] = arr->i[j + 1];
			}
			arr->ni--;

			if (!(temp = realloc(arr->i, arr->ni * sizeof(Exec*)))) {
				perror("realloc() failed");
				exit(errno);
			}
			arr->i = temp;
			return;
		}
	}
}

static void
removeignored(ExecArray *arr)
{
	FILE *fp;
	char buf[BUF_SIZE];
	
	if(!(fp = fopen(getpath(ignorepath), "r")))
		return;

	while (fgets(buf, sizeof(buf), fp)) {
		trimwhitespace(buf);
		removeentry(arr, buf);
	}

	fclose(fp);
}

static void
setexecorder(ExecArray *arr)
{
	FILE *fp;
	char buf[BUF_SIZE];
	int index = 0;

	if (!(fp = fopen(getpath(cachepath), "r")))
		return;

	while (fgets(buf, sizeof(buf), fp)) {
		trimwhitespace(buf);
		for (size_t i = index; i < arr->ni; i++) {
			if (!strcmp(buf, arr->i[i]->name)) {
				swapexec(arr, index, i);
				index++;
				break;
			}
		}
	}
}

static void
swapexec(ExecArray *arr, const size_t indexa, const size_t indexb)
{
	Exec *temp;

	temp = arr->i[indexa];
	arr->i[indexa] = arr->i[indexb];
	arr->i[indexb] = temp;
}

static void
trimwhitespace(char *str)
{
	char *end;
	char *original = str;

	while(isspace((unsigned char)*str)) str++;

	if(*str == 0) {
		*original = '\0';
		return;
	}

	end = str + strlen(str) - 1;
	while(end > str && isspace((unsigned char)*end)) end--;

	*(end+1) = '\0';

	if (str != original) {
		memmove(original, str, end - str + 2); // +2 to include the null terminator
	}
}

static void
writeselection(ExecArray *arr, const int selection)
{
	char *path;
	char *temp;
	char *ptr;
	char buf[BUF_SIZE];
	int found;
	FILE *fin, *fout;

	temp = getpath(cachepath);
	path = getpath(cachepath);
	if (!(ptr = realloc(temp, (strlen(temp) + 6) * sizeof(char)))) {
		perror("malloc() failed");
		exit(errno);
	}
	strcat(ptr, ".temp");
	temp = ptr;

	mkparentdir(temp);

	if (!(fout = fopen(temp, "w"))) {
		perror("fopen() failed");
		exit(errno);
	}

	fprintf(fout, "%s\n", arr->i[selection]->name);

	if ((fin = fopen(path, "r"))) {
		while (fgets(buf, sizeof(buf), fin)) {
			trimwhitespace(buf);
			found = 0;
			if (!strcmp(buf, arr->i[selection]->name))
				continue;
			for (size_t i = 0; i < arr->ni; i++) {
				if (!strcmp(buf, arr->i[i]->name)) {
					found = 1;
					break;
				}
			}
			if (!found)
				continue;
			fprintf(fout, "%s\n", buf);
		}
	}
	if (fin)
		fclose(fin);
	fclose(fout);

	remove(path);
	rename(temp, path);
}

int
main(int argc, char *argv[])
{
	ExecArray *exec = getexec();
	int selection;

	setexecorder(exec);

	if ((selection = getindex(exec, argv)) < 0)
		return 0;

	execute(exec->i[selection]);
	writeselection(exec, selection);

	freeexecarray(exec);
	return 0;
}
