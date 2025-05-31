#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xlib.h>

/* constants definition */
const char *cfgpath[] = { "$XDG_CONFIG_HOME", "xrandr", "displaysetup2.conf", NULL};
const char *logpath[] = {"$HOME", "window-manager.log", NULL};
const char *pmtpath[] = { "usr", "local", "bin", "dmenu", NULL};

/* structure definitions */
typedef struct {
	RRMode rid;
	char *id;
	double rate;
	unsigned int primary;
	unsigned int xoffset;
	unsigned int yoffset;
	unsigned int xmode;
	unsigned int ymode;
	unsigned int moderot;
	Rotation rotation;
} CfgMonitor;

typedef struct {
	unsigned int dpi;
	char *name;
	size_t mc;
	CfgMonitor **m;
} CfgScreen;

typedef struct {
	size_t sc;
	CfgScreen **s;
} CfgScreens;

/* function definitions */
static void cleanup(CfgScreens *cs);
static void freemonitor(CfgMonitor *m);
static void freescreen(CfgScreen *s);
static void freescreens(CfgScreens *cs);
static CfgScreens* getcfgscreens(void);
static char* getcfgstring(const char *val);
static FILE* getcfgstream(void);
static void getmonitorconfig(CfgScreen *s, const char *strin, FILE *fp);
static char* getmonitorstring(const char *strin, FILE *fp);
static char* getpath(const char **arr);
static void logstring(const char *string);
static void matchscreens(CfgScreens *cs);
static void newmonitor(CfgScreen *s);
static void newscreen(CfgScreens *ss);
static void parsemonitor(CfgScreen *s, const char *mstring);
static void parsemonitorvalue(CfgScreen *s, const char *key, const char *val);
static void printhelp(void);
static void removescreen(CfgScreens *cs, const int index);
static void setscreen(CfgScreen *s);
static void setup(void);
static void setupemptyscreen(CfgScreen **s);
static void setupmonitor(CfgMonitor *m, XRROutputInfo *output);
static void setupscreen(CfgScreen *s);
static void setupscreensize(CfgScreen *s, const unsigned int retract);
static char* trimwhitespace(char *string);

/* variable definitions */
static Display *dpy;
static Window root;
static XRRScreenResources *resources;

static void
cleanup(CfgScreens *cs)
{
	freescreens(cs);
	XRRFreeScreenResources(resources);
	XCloseDisplay(dpy);
}

static void
freemonitor(CfgMonitor *m)
{
	if (!m)
		return;
	if (m->id)
		free(m->id);
	free(m);
	m = NULL;
}

static void
freescreen(CfgScreen *s)
{
	if (!s)
		return;

	for (size_t j = 0; j < s->mc; j++) {
		freemonitor(s->m[j]);
	}

	free(s->m);
	free(s->name);
	free(s);
	s = NULL;
}

static void
freescreens(CfgScreens *cs)
{
	if (!cs)
		return;

	for (size_t i = 0; i < cs->sc; i ++) {
		freescreen(cs->s[i]);
	}
	
	free(cs);
	cs = NULL;
}

static CfgScreens*
getcfgscreens(void)
{
	FILE *fp;
	CfgScreens *cs;
	char buf[512];
	char *input;
	char *ptr;
	char key[512];
	char val[512];

	if (!(fp = getcfgstream()))
		return NULL;
	
	if (!(cs = malloc(sizeof(CfgScreens)))) {
		char log[512];

		sprintf(log, "ERROR - malloc() failed - %s", strerror(errno));
		logstring(log);
		exit(errno);
	}

	cs->sc = 0;
	cs->s = NULL;

	while (fgets(buf, sizeof(buf), fp)) {
		input = trimwhitespace(buf);
		input[strcspn(input, "\n")] = '\0';
		
		if (input[0] == '#')
			continue;

		if (!strcmp(input, "[screen]")) {
			newscreen(cs);
			continue;
		}

		if (!(ptr = strchr(input, '=')))
			continue;

		strncpy(key, input, (ptr - input));
		key[ptr - input] = '\0';
		strcpy(val, (ptr + 1));

		if (!strcmp(key, "name"))
			cs->s[cs->sc - 1]->name = getcfgstring(val);
		if (!strcmp(key, "dpi"))
			sscanf(val, "%d", &(cs->s[cs->sc - 1]->dpi));
		if (!strcmp(key, "monitor"))
			getmonitorconfig(cs->s[cs->sc - 1], val, fp);
	}

	fclose(fp);
	return cs;
}

static char*
getcfgstring(const char *val)
{
	char *ptr;

	if (*(ptr = (char*) val + strlen(val) - 1) != '"')
		return NULL;
	*ptr = '\0';

	if (*(ptr = (char*) val) != '"')
		return NULL;

	return strdup(++ptr);
}

static FILE*
getcfgstream(void)
{
	FILE *fp;
	char *path;

	path = getpath(cfgpath);
	if (access(path, F_OK)) {
		char log[512];

		sprintf(log, "WARN - File: %s does not exist", path);
		logstring(log);
		return NULL;
	}

	if (!(fp = fopen(path, "r"))) {
		char log[512];

		sprintf(log, "ERROR - Failed to open file: %s - %s", path, strerror(errno));
		logstring(log);
		exit(errno);
	}

	return fp;
}

static void
getmonitorconfig(CfgScreen *s, const char *strin, FILE *fp)
{
	char *mstring;

	if (!(mstring = getmonitorstring(strin, fp))) {
		return;
	}

	newmonitor(s);
	parsemonitor(s, mstring);
}

static char*
getmonitorstring(const char *strin, FILE *fp)
{
	size_t str_size;
	size_t ind;
	char *str;
	char *ret;
	int buf;
	int instr;

	str_size = (strlen(strin) >= 16) ? strlen(strin) + 2 : 16;
	if (!(str = malloc(str_size * sizeof(char)))) {
		char log[512];

		sprintf(log, "ERROR - malloc() failed - %s", strerror(errno));
		logstring(log);
		exit(errno);
	}

	strncpy(str, strin, str_size - 1);
	str[str_size - 1] = '\0';

	instr = 0;
	ind   = 0;

	while (!(str[ind] == ']' && !instr)) {
		if (ind >= str_size - 1) {
			str_size *= 2;
			if (!(str = realloc(str, str_size * sizeof(char)))) {
				char log[512];

				sprintf(log, "ERROR - realloc() failed - %s", strerror(errno));
				logstring(log);
				exit(errno);
			}
		}

		switch (str[ind]) {
		case '\n':              /* fallthrough */
		case '\t':
			str[ind] = ' ';
			ind++;
			break;

		case '\0':
			buf = fgetc(fp);
			if (buf == EOF) {
				if (ferror(fp)) {
					char log[512];

					sprintf(log, "ERROR - configuration fgetc() failed - %s", strerror(errno));
					logstring(log);
					free(str);
					exit(errno);
				}
				logstring("ERROR - configuration fgetc() - EOF");
				free(str);
				return NULL;
			}
			str[ind] = (char) buf;
			str[ind + 1] = '\0';
			break;

		case '"':
			ind++;
			instr = !instr;
			break;

		default:
			ind++;
			break;
		}
	}

	str[ind] = '\0';
	ret = strdup(str + 1);

	free(str);
	return ret;
}

static char*
getpath(const char **arr)
{
	char *path;
	char *env;
	size_t spath;

	path = malloc(sizeof(char));
	path[0] = '\0';
	spath = 1;

	for (int i = 0; arr[i] != NULL; i++) {
		if (arr[i][0] == '$') {
			env = getenv(arr[i] + 1);

			if (env) {
				spath += strlen(env);
				if(!(path = realloc(path, spath * sizeof(char)))) {
					char log[512];

					sprintf(log, "ERROR - realloc() failed - %s", strerror(errno));
					logstring(log);
					exit(errno);
				}
				strcat(path, env);
				continue;
			}

			if (!strcmp(arr[i] + 1, "XDG_CONFIG_HOME")) {
				if (!(env = getenv("HOME"))) {
					fprintf(stderr, "ERROR - Failed to get env variable: HOME - %s\n", strerror(errno));
					exit(errno);
				}
				spath += strlen(env) + strlen("/.config");
				if(!(path = realloc(path, spath * sizeof(char)))) {
					char log[512];

					sprintf(log, "ERROR - realloc() failed - %s", strerror(errno));
					logstring(log);
					exit(errno);
				}
				strcat(path, env);
				strcat(path, "/.config");
				continue;
			}

			fprintf(stderr, "ERROR - Failed to get env variable: %s - %s\n", arr[i], strerror(errno));
			exit(errno);
		} else {
			spath += strlen(arr[i]) + 1;
			if(!(path = realloc(path, spath * sizeof(char)))) {
				char log[512];

				sprintf(log, "ERROR - realloc() failed - %s", strerror(errno));
				logstring(log);
				exit(errno);
			}
			strcat(path, "/");
			strcat(path, arr[i]);
		}
	}

	return path;
}

static void
logstring(const char *string)
{
	if (!string)
		return;

	FILE *fp;
	char *path;
	time_t rawtime;
	struct tm *timeinfo;

	path = getpath(logpath);

	if (!(fp = fopen(path, "a"))) {
		fprintf(stderr, "ERROR - Failed to open in append mode, path: %s - %s\n", path, strerror(errno));
		exit(errno);
	}

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d xrandr-setup\n%s\n\n", timeinfo->tm_year+1900,
		timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, string);
	
	fclose(fp);
	free(path);
}

static void
matchscreens(CfgScreens *cs)
{
	XRROutputInfo *output;
	size_t mc;
	char **id;
	unsigned int match;

	if (!cs)
		return;

	id = NULL;
	mc = 0;

	for (int i = 0; i < resources->noutput; i++) {
		output = XRRGetOutputInfo(dpy, resources, resources->outputs[i]);
		if (!output->connection) {
			mc++;
			id = realloc(id, mc * sizeof(char*));
			id[i] = strdup(output->name);
		}
		XRRFreeOutputInfo(output);
	}

	for (size_t i = cs->sc - 1; i < cs->sc; i--) {
		if (cs->s[i]->mc != mc) {
			removescreen(cs, i);
			continue;
		}
		for (size_t j = 0; j < mc; j++) {
			match = 0;
			for (size_t k = 0; k < mc; k++) {
				if (!strcmp(cs->s[i]->m[j]->id, id[k]))
					match++;
			}
			if (match != 1) {
				removescreen(cs, i);
				break;
			}
		}
	}
}

static void
newmonitor(CfgScreen *s)
{
	CfgMonitor *m;

	s->mc++;
	if (!(s->m = realloc(s->m, s->mc * sizeof(CfgMonitor*)))) {
		char log[512];

		sprintf(log, "ERROR - realloc() failed - %s", strerror(errno));
		logstring(log);
		exit(errno);
	}

	if (!(m = malloc(sizeof(CfgMonitor)))) {
		char log[512];

		sprintf(log, "ERROR - malloc() failed - %s", strerror(errno));
		logstring(log);
		exit(errno);
	}
	s->m[s->mc - 1] = m;

	m->id       = NULL;
	m->primary  = 0;
	m->xoffset  = 0;
	m->yoffset  = 0;
	m->xmode    = 0;
	m->ymode    = 0;
	m->rate     = 0.0;
	m->moderot  = 0;
	m->rotation = RR_Rotate_0;
}

static void
newscreen(CfgScreens *ss)
{
	CfgScreen *s;

	ss->sc++;
	if (!(ss->s = realloc(ss->s, ss->sc * sizeof(CfgScreen*)))) {
		char log[512];

		sprintf(log, "ERROR - realloc() failed - %s", strerror(errno));
		logstring(log);
		exit(errno);
	}

	if (!(s = malloc(sizeof(CfgScreen)))) {
		char log[512];

		sprintf(log, "ERROR - malloc() failed - %s", strerror(errno));
		logstring(log);
		exit(errno);
	}

	ss->s[ss->sc - 1] = s;
	s->dpi = 0;
	s->mc = 0;
	s->m = NULL;
	s->name = NULL;
}

static void
parsemonitorvalue(CfgScreen *s, const char *key, const char *val)
{
	if (!strcmp(key, "id"))
		s->m[s->mc - 1]->id = getcfgstring(val);
	if (!strcmp(key, "primary"))
		s->m[s->mc - 1]->primary = (!strcmp(val, "true") || !strcmp(val, "True")) ? 1 : 0;
	if (!strcmp(key, "xoffset")) 
		sscanf(val, "%d", &(s->m[s->mc - 1]->xoffset));
	if (!strcmp(key, "yoffset"))
		sscanf(val, "%d", &(s->m[s->mc - 1]->yoffset));
	if (!strcmp(key, "xmode")) 
		sscanf(val, "%d", &(s->m[s->mc - 1]->xmode));
	if (!strcmp(key, "ymode"))
		sscanf(val, "%d", &(s->m[s->mc - 1]->ymode));
	if (!strcmp(key, "rate")) 
		sscanf(val, "%lf", &(s->m[s->mc - 1]->rate));
	if (!strcmp(key, "rotation")) {
		if (!strcmp(val, "normal"))
			s->m[s->mc - 1]->rotation = RR_Rotate_0;
		else if (!strcmp(val, "inverted"))
			s->m[s->mc - 1]->rotation = RR_Rotate_180;
		else if (!strcmp(val, "left"))
			s->m[s->mc - 1]->rotation = RR_Rotate_270;
		else if (!strcmp(val, "right"))
			s->m[s->mc - 1]->rotation = RR_Rotate_90; 
	}
}

static void
parsemonitor(CfgScreen *s, const char *mstring)
{
	char mstr[2048];
	char str[1024];
	char key[512];
	char val[512];
	char *tok;
	char *ptr;

	strcpy(mstr, mstring);
	tok = strtok(mstr, ",");
	while (tok) {
		ptr = trimwhitespace(tok);
		strcpy(str, ptr);

		if (str[0] == '#') {
			tok = strtok(NULL, ",");
			continue;
		}

		if (!(ptr = strchr(str, '='))) {
			tok = strtok(NULL, ",");
			continue;
		}

		strncpy(key, str, (ptr - str));
		key[ptr - str] = '\0';
		strcpy(val, (ptr + 1));
		parsemonitorvalue(s, key, val);
		tok = strtok(NULL, ",");
	}
}

static void
printhelp(void)
{
	printf("xrandr-setup\nThis is an application for setting xRandR using premade configuration files for your screens\n");
	printf("\nUsage:\n");
	printf("\t'-h' or '--help'      prints this menu\n");
	printf("\t'-a' or '--auto'      sets up with a basic screen ignoring the config files\n");
	printf("\t'-s' or '--select'    prompts through the selected applications for a layout (the prompt args are passed through argv)\n");
}

static void
removescreen(CfgScreens *cs, const int index)
{
	freescreen(cs->s[index]);
	for (size_t i = index; i < cs->sc - 1; i++)
		cs->s[i] = cs->s[i + 1];
	cs->sc--;
}

static void
setscreen(CfgScreen *s)
{
	XRROutputInfo *output;
	XRRCrtcInfo *crtc;

	for (size_t i = 0; i < s->mc; i++) {
		if (s->m[i]->rid == 0) {
			setupemptyscreen(&s);
			logstring("WARN - Configuration error");
			break;
		}
	}

	setupscreensize(s, 0);

	for (int i = 0; i < resources->noutput; i++) {
		output = XRRGetOutputInfo(dpy, resources, resources->outputs[i]);
		for (size_t j = 0; j < s->mc; j++) {
			if (!strcmp(s->m[j]->id, output->name)) {
				crtc = XRRGetCrtcInfo(dpy, resources, output->crtc);
				XRRSetCrtcConfig(dpy, resources, output->crtc,
				                 crtc->timestamp, s->m[j]->xoffset, s->m[j]->yoffset,
				                 s->m[j]->rid, s->m[j]->rotation, crtc->outputs,
				                 crtc->noutput);
				XRRFreeCrtcInfo(crtc);
				if (s->m[j]->primary)
					XRRSetOutputPrimary(dpy, root, resources->outputs[i]);
			}
		}
		XRRFreeOutputInfo(output);
	}

	setupscreensize(s, 1);
}

static void
setup(void)
{
	dpy = XOpenDisplay(NULL);
	
	if (dpy == NULL) {
		fprintf(stderr, "Failed to start X\n");
		exit(errno);
	}

	root = XDefaultRootWindow(dpy);
	resources = XRRGetScreenResources(dpy, root);
}

static void
setupemptyscreen(CfgScreen **s)
{
	XRROutputInfo *output;

	if (*s) {
		freescreen(*s);
		*s = NULL;
	}

	*s = malloc(sizeof(CfgScreen));
	(*s)->mc = 0;
	(*s)->m = NULL;
	(*s)->dpi = 0;
	(*s)->name = NULL;

	for (int i = 0; i < resources->noutput; i++) {
		output = XRRGetOutputInfo(dpy, resources, resources->outputs[i]);
		if (!output->connection) {
			newmonitor(*s);
			(*s)->m[(*s)->mc - 1]->id = strdup(output->name);
		}
		XRRFreeOutputInfo(output);
	}
}

static void
setupmonitor(CfgMonitor *m, XRROutputInfo *output)
{
	XRRModeInfo *mode;
	double rate;
	char str1[16];
	char str2[16];

	if (!m->xmode) {
		for (int i = 0; i < output->nmode; i++) {
			for (int j = 0; j < resources->nmode; j++) {
				if (output->modes[i] == resources->modes[j].id) {
					mode = &resources->modes[j];
					if (mode->width > m->xmode)
						m->xmode = mode->width;
				}
			}
		}
	}

	if (!m->ymode) {
		for (int i = 0; i < output->nmode; i++) {
			for (int j = 0; j < resources->nmode; j++) {
				if (output->modes[i] == resources->modes[j].id) {
					mode = &resources->modes[j];
					if (mode->width != m->xmode)
						continue;
					if (mode->height > m->ymode)
						m->ymode = mode->height;
				}
			}
		}
	}

	if (m->rate == 0.0) {
		for (int i = 0; i < output->nmode; i++) {
			for (int j = 0; j < resources->nmode; j++) {
				if (output->modes[i] == resources->modes[j].id) {
					mode = &resources->modes[j];
					if ((mode->width != m->xmode) || (mode->height!= m->ymode))
						continue;
					rate = (double) mode->dotClock / (double) (mode->hTotal * mode->vTotal);
					if (rate > m->rate)
						m->rate = rate;
				}
			}
		}
	}

	/* check if the final monitor mode is valid */
	for (int i = 0; i < output->nmode; i++) {
		for (int j = 0; j < resources->nmode; j++) {
			if (output->modes[i] == resources->modes[j].id) {
				mode = &resources->modes[j];
				if ((mode->width != m->xmode) || (mode->height!= m->ymode))
					continue;
				sprintf(str1, "%.0lf", m->rate);
				sprintf(str2, "%.0lf", (double) mode->dotClock / (double) (mode->hTotal * mode->vTotal));
				if (!strcmp(str1, str2)) {
					m->rid = mode->id;
					return;
				}
			}
		}
	}
}

static void
setupscreen(CfgScreen *s)
{
	XRROutputInfo *output;

	for (int i = 0; i < resources->noutput; i++) {
		output = XRRGetOutputInfo(dpy, resources, resources->outputs[i]);
		if (!output->connection) {
			for (size_t i = 0; i < s->mc; i++) {
				if (!strcmp(s->m[i]->id, output->name)) {
					setupmonitor(s->m[i], output);
					break;
				}
			}
		}
		XRRFreeOutputInfo(output);
	}
}

static void
setupscreensize(CfgScreen *s, unsigned int retract)
{
	XRRScreenSize *scr;
	unsigned int temp;
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int mmWidth;
	unsigned int mmHeight;
	int nsizes;
	double dpi;
	
	for (size_t i = 0; i < s->mc ; i++) {
		if ((s->m[i]->rotation == RR_Rotate_90 || s->m[i]->rotation == RR_Rotate_270) && !s->m[i]->moderot) {
			temp = s->m[i]->xmode;
			s->m[i]->xmode = s->m[i]->ymode;
			s->m[i]->ymode= temp;
			s->m[i]->moderot++;
		}
	}

	for (size_t i = 0; i < s->mc ; i++) {
		if (s->m[i]->yoffset + s->m[i]->ymode > height)
			height = s->m[i]->yoffset + s->m[i]->ymode;
		if (s->m[i]->xoffset + s->m[i]->xmode > width)
			width = s->m[i]->xoffset + s->m[i]->xmode;
	}

	scr = XRRSizes(dpy, 0, &nsizes);

	if (s->dpi)
		dpi = (double) s->dpi;
	else
		dpi = (25.4 * scr[0].height) / scr[0].mheight;
	
	mmWidth  = (int) ((25.4 * width) / dpi);
	mmHeight = (int) ((25.4 * height) / dpi);

	if (!retract) {
		if (scr[0].width > (int) width) {
			width = scr[0].width;
			mmWidth = scr[0].mwidth;
		}
		
		if (scr[0].height > (int) height) {
			height = scr[0].height;
			mmHeight = scr[0].mheight;
		}
	}
	XRRSetScreenSize(dpy, root, width, height, mmWidth, mmHeight);
}

static char*
trimwhitespace(char *string)
{
	char *end;

	end = string + strlen(string) - 1;
	while (end > string && isspace((unsigned char)*end)) end--;
	end[1] = '\0';
	while (*string && isspace((unsigned char)*string)) string++;

	return string;
}

static int
promptoption(const char *menu, char *argv[])
{
	int option;
	int writepipe[2];
	int readpipe[2];
	char buffer[512];
	char *path;
	char **args;
	char *name = NULL;
	char *ptr;
	size_t argc = 0;

	path = getpath(pmtpath);
	for (char **ptr = (char**) &pmtpath; *ptr != NULL; ptr++) 
		name = *ptr;

	for (char **ptr = argv; *ptr != NULL; ptr++)
		argc++;

	args = malloc((argc + 2) * sizeof (char*));
	args[0] = strdup(name);
	for (size_t i = 0; i < argc; i++) {
		args[i+1] = strdup(argv[i]);
	}
	args[argc+1] = NULL;

	option = -EREMOTEIO;
	buffer[0] = '\0';

	if (pipe(writepipe) < 0 || pipe(readpipe) < 0) {
		logstring("Failed to initialize pipes");
		return -ESTRPIPE;
	}
	
	switch (fork()) {
		case -1:
			logstring("fork() failed");
			return -ECHILD;

		case 0: /* child - xmenu */
			close(writepipe[1]);
			close(readpipe[0]);

			dup2(writepipe[0], STDIN_FILENO);
			close(writepipe[0]);

			dup2(readpipe[1], STDOUT_FILENO);
			close(readpipe[1]);
			
			execv(path, args);
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
	free(path);

	return option;
}

static int
getinputscreen(CfgScreens *cs, char *argv[])
{
	size_t pstrs;
	char *pstr;
	char buf[512];
	int ret;

	if (cs->sc < 1)
		return -1;

	if (!(pstr = malloc(sizeof(char)))) {
		char log[512];

		sprintf(log, "ERROR - malloc() failed - %s", strerror(errno));
		logstring(log);
		exit(errno);
	}

	pstr[0] = '\0';
	pstrs = 1;

	for (size_t i = 0; i < cs->sc; i++) {
		ret = snprintf(buf, sizeof(buf), "%s\t%zu\n", cs->s[i]->name, i);
		
		if ((size_t) ret > sizeof(buf) - 1 || ret < 0) {
			logstring("ERROR - snprintf() failed - buffer overflow (or encoding error)");
			free(pstr);
			exit(errno);
		}

		pstrs += strlen(buf);

		if (!(pstr = realloc(pstr, pstrs * sizeof(char)))) {
			char log[512];

			sprintf(log, "ERROR - realloc() failed - %s", strerror(errno));
			logstring(log);
			free(pstr);
			exit(errno);
		}
		strcat(pstr, buf);
	}
	if (pstr[pstrs - 2] == '\n')
		 pstr[pstrs - 2] = '\0';
	
	return promptoption(pstr, argv);
}

int
main(int argc, char *argv[])
{
	CfgScreens *cs;
	CfgScreen *s;
	int selscreen = 0;

	setup();

	cs = getcfgscreens();
	matchscreens(cs);

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--auto") || !strcmp(argv[i], "-a")) {
			selscreen = -1;
			break;
		} else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
			printhelp();
			cleanup(cs);
			return 0;
		} else if (!strcmp(argv[i], "--select") || !strcmp(argv[i], "-s")) {
			selscreen = getinputscreen(cs, &argv[i+1]);
			break;
		} else {
			fprintf(stderr, "xrandr-setup - invalid arguments. Execute with --help for usage\n");
			cleanup(cs);
			return 1;
		}
	}

	if (cs && selscreen >=0 && cs->sc > 0) {
		s = cs->s[selscreen];
	} else {
		if (cs) {
			freescreens(cs);
			cs = NULL;
		}
		cs = malloc(sizeof(CfgScreens));
		cs->sc = 0;
		cs->s = NULL;

		newscreen(cs);
		s = cs->s[0];
		setupemptyscreen(&s);
	}
	
	setupscreen(s);
	setscreen(s);
	cleanup(cs);
	return 0;
}