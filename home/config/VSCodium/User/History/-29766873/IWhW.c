#include <libnotify/notification.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "../include/colorscheme.h"
#include "../include/common.h"

#define KERNEL_C

#include "../include/config.h"

#define BUF_SIZE 64



static unsigned int
getupdates(const char *cmd)
{
	FILE *ep;
	char buffer[BUF_SIZE];
	int  counter = 0;

	if (!(ep = popen(cmd, "r"))) {
		logwrite("popen() failed for command", cmd, LOG_WARN, "dwmblocks-kernel");
		return 0;
	}

	while(fgets(buffer, sizeof(buffer), ep))
		counter++;

	pclose(ep);
	return counter;
}

static char*
uitoa(const unsigned int num)
{
	size_t digits = 0;
	char   *ret;

	for (unsigned int i = num; i > 0; i = i/10)
		digits++;
	if (!digits)
		digits++;

	if (!(ret = malloc((digits + 1) * sizeof(char))))
		logwrite("malloc() failed", "Returned NULL pointer", LOG_ERROR, "dwmblocks-kernel");

	snprintf(ret, digits + 1, "%u", num);
	return ret;
}

static void
execbutton(unsigned int  *aur, unsigned int *pm)
{
	char *env;
	
	if (!(env = getenv("BLOCK_BUTTON")))
		return;

	switch (atoi(env)) {
	case 1:
	{
		char *body = NULL;
		NotifyNotification *notification = NULL;

		notification = newnotify("Packages", "Getting update info..", "tux", NOTIFY_URGENCY_NORMAL, 1);

		*aur = getupdates(aur_updates_cmd);
		*pm  = getupdates(pm_updates_cmd);

		strapp(&body, "󰏖 Pacman Updates: ");
		strapp(&body, uitoa(*pm));
		strapp(&body, "\n AUR Updates: ");
		strapp(&body, uitoa(*aur));

		updatenotify(notification, "Packages", body, "tux", NOTIFY_URGENCY_NORMAL, 1);
		free(body);
		freenotify(notification);
		break;
	}

	case 3:
		forkexecvp((char**) update_cmd_args, "dwmblocks-kernel");
		break;

	default:
		break;
	}
}

int
main(void)
{
	struct utsname buf;
	char*          release;
	unsigned int   aur = 0;
	unsigned int   pm  = 0;

	execbutton(&aur, &pm);

	aur = aur == 0 ? getupdates(aur_updates_cmd) : aur;
	pm  = pm == 0  ? getupdates(pm_updates_cmd)  : pm;

	if (uname(&buf))
		logwrite("Failed in allocatting utsname struct", NULL, LOG_FATAL, "dwmblocks-kernel");

	release = strtok(buf.release, "-");

	if ((aur + pm) > 0)
		printf(CLR_12"  󰏖 %d  %s"NRM"\n", aur + pm, release);
	else
		printf(CLR_12"   %s"NRM"\n", release);

	return 0;
}
