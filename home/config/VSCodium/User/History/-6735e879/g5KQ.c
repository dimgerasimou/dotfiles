#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/colorscheme.h"
#include "../include/common.h"

#define BUFFER_SIZE 64
#define BATTERY_C

#include "../include/config.h"

const char *batteryIconList[]  = { CLR_1" ", CLR_3" ", CLR_2" ", CLR_2" ", CLR_2" " };

#ifdef POWER_MANAGEMENT
const char *optimusManagerCommand = "optimus-manager --status";
const char *modeTextList[]        = { "Unmanaged", "Integrated", "Hybrid", "Nvidia" };
const char *modeIconList[]        = { "battery", "intel", "deepin-graphics-driver-manager", "nvidia" };

static unsigned int
get_mode(void)
{
	char buf[BUFFER_SIZE];
	FILE         *ep  = NULL;
	unsigned int  ret = 0;

	if (!(ep = popen(optimusManagerCommand, "r"))) {
		logwrite("popen() failed for", optimusManagerCommand, LOG_WARN, "dwmblocks-battery");
		return 0;
	}

	while (fgets(buf, sizeof(buf), ep))
		if (strstr(buf, "Current"))
			break;

	if (strstr(buf, "integrated"))
		ret = 1;
	else if (strstr(buf, "hybrid"))
		ret = 2;
	else if (strstr(buf, "nvidia"))
		ret = 3;

	pclose(ep);

	return ret;
}

static void
notification(const char *capacity, const char *status)
{
	char         *body = NULL;
	unsigned int  mode = get_mode();

	strapp(&body, "Battery capacity: ");
	strapp(&body, capacity);
	strapp(&body, "%\nBattery status:   ");
	strapp(&body, status);
	strapp(&body, "\nOptimus manager:  ");
	strapp(&body, modeTextList[mode]);

	notify("Power", body, (char*) modeIconList[mode], NOTIFY_URGENCY_NORMAL, 1);

	free(body);
}

#else /* POWER_MANAGEMENT */

static void
notification(const char *capacity, const char *status)
{
	char *body = NULL;

	strapp(&body, "Battery capacity: ");
	strapp(&body, capacity);
	strapp(&body, "%\nBattery status:   ");
	strapp(&body, status);

	notify("Power", body, "battery", NOTIFY_URGENCY_NORMAL, 1);

	free(body);
}

#endif /* POWER_MANAGEMENT */

static char*
uitoa(const unsigned int num)
{
	char    *ret     = NULL;
	size_t  digits   = 0;
	int     sn_check = 0;

	for (unsigned int i = num; i > 0; i = i/10)
		digits++;
	if (!digits)
		digits++;

	if (!(ret = malloc((digits + 1) * sizeof(char)))) {
		logwrite("malloc() failed. Returned NULL pointer", NULL, LOG_ERROR, "dwmblocks-battery");
		return NULL;
	}

	sn_check = snprintf(ret, digits + 1, "%u", num);

	if (sn_check < 0 || sn_check > (int) digits) {
		logwrite("snprintf() failed. Buffer overflow", NULL, LOG_ERROR, "dwmblocks-battery");
		return NULL;
	}

	return ret;
}

static void
exec_block_button(const unsigned int capacity, const char *status)
{
	char *env = NULL;

	if (!(env = getenv("BLOCK_BUTTON")))
		return;

	switch (atoi(env)) {
	case 1:
	{
		char *cap_str = uitoa(capacity);

		if (!cap_str)
			break;

		notification(cap_str, status);
		
		free(cap_str);
		break;
	}
	default:
		break;
	}
}

static unsigned int
get_capacity(void)
{
	FILE         *fp  = NULL;
	unsigned int  ret = 0;
	char *path = NULL;

	strapp(&path, bat_path);
	strapp(&path, "/capacity");

	if (!(fp = fopen(path, "r")))
		logwrite("fopen() failed for file", path, LOG_FATAL, "dwmblocks-battery");

	fscanf(fp, "%u", &ret);
	fclose(fp);
	free(path);

	return ret;
}

static char*
get_status(void)
{
	char buffer[BUFFER_SIZE];
	FILE *fp  = NULL;
	char *path = NULL;

	strapp(&path, bat_path);
	strapp(&path, "/status");

	if (!(fp = fopen(path, "r")))
		logwrite("fopen() failed for file", path, LOG_FATAL, "dwmblocks-battery");

	fgets(buffer, sizeof(buffer), fp);
	buffer[BUFFER_SIZE - 1] = '\0';

	fclose(fp);
	free(path);

	trimtonewl(buffer);

	return strdup(buffer);
}

int
main(void)
{
	char         *status   = get_status();
	unsigned int  capacity = get_capacity();

	exec_block_button(capacity, status);

	if(!strcmp(status, "Charging")) {
		printf(CLR_3 BG_1" "NRM"\n");
		free(status);
		return EXIT_SUCCESS;
	}

	printf(BG_1"%s"NRM"\n", batteryIconList[lround(capacity/25.0)]);

	free(status);
	return 0;
}
