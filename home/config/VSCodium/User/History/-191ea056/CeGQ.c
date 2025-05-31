#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../include/colorscheme.h"
#include "../include/common.h"

const char *MONTHS[] = {"January",    "February", "March",    "April",
                        "May",        "June",     "July",     "August",
                        "Semptember", "October",  "November", "December"};

const int   DAYS_IN_MONTH[] = {31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const char *CAL_PATH[]      = {"usr", "bin", "firefox", NULL};
const char *CAL_ARGS[]      = {"firefox", "--new-window", "https://calendar.google.com", NULL};

static int
get_first_day(int mday, int wday)
{
	while (mday > 7)
		mday -= 7;

	while (mday > 1) {
		mday--;
		wday--;
		if (wday == -1)
			wday = 6;
	}

	wday--;
	if (wday == -1)
		wday = 6;

	return wday;
}

static int
get_month_days(const int m, const int y)
{
	if (m != 1)
		return DAYS_IN_MONTH[m];
	if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)
		return 29;

	return 28;
}

static char*
get_calendar(const int mday, const int wday, const int m, const int y)
{
	int  firstday;
	int  monthdays;
	char day[64];
	char calendar[512];
	char *ret;

	firstday  = get_first_day(mday, wday);
	monthdays = get_month_days(m, y);

	strcpy(calendar, "Mo Tu We Th Fr <span color='#F38BA8'>Sa Su</span>\n");

	for (int i = 0; i < firstday; i++)
		strcat(calendar, "   ");

	for (int i = 1; i <= monthdays; i++) {
		if (i == mday)
			sprintf(day, "<span color='black' bgcolor='#F38BA8'>%2d</span> ", i);
		else if (firstday == 5 || firstday == 6)
			sprintf(day, "<span color='#F38BA8'>%2d</span> ", i);
		else
			sprintf(day, "%2d ", i);

		if (firstday == 7) {
			firstday = 0;
			strcat(calendar, "\n");
		}

		strcat(calendar, day);
		firstday++;
	}

	ret = malloc((strlen(calendar) + 1) * sizeof(char));
	strcpy(ret, calendar);
	return ret;
}

static char*
get_summary(const int m, const int y)
{
	char *ret;
	char summary[16];
	int  total_size;

	sprintf(summary, "%s %d", MONTHS[m], y);
	total_size = (20 + strlen(summary)) / 2;

	ret = malloc((total_size + 1) * sizeof(char));
	sprintf(ret, "%*s", total_size, summary);
	return ret;
}

static void
exec_calendar(const int mday, const int wday, const int m, const int y)
{
	char *body;
	char *summary;

	body    = get_calendar(mday, wday, m, y);
	summary = get_summary(m, y);

	notify(summary, body, "calendar", NOTIFY_URGENCY_NORMAL, 0);

	free(body);
	free(summary);
}

static void
executebutton(const int mday, const int wday, const int m, const int y)
{
	char *env;
	char *path;

	if (!(env = getenv("BLOCK_BUTTON")))
		return;

	switch (env[0] - '0') {
	case 1:
		exec_calendar(mday, wday, m, y);
		return;

	case 3:
		path = get_path((char**) CAL_PATH, 1);

		forkexecv(path, (char **) CAL_ARGS, "dwmblocks-date");

		free(path);
		return;

	default:
		break;
	}
}

int
main()
{
	time_t    currentTime = time(NULL);
	struct tm *localTime  = localtime(&currentTime);

	executebutton(localTime->tm_mday, localTime->tm_wday, localTime->tm_mon,
	              localTime->tm_year + 1900);

	printf(CLR_1 "   %02d/%02d" NRM "\n", localTime->tm_mday, ++localTime->tm_mon);

	return 0;
}
