#include <stdio.h>
#include <string.h>

#include "knnsearch.h"

int
isuint(char *s)
{
	for (char *ptr = s; *ptr != '\0'; ptr++) {
		if (*ptr > '9' || *ptr < '0')
			return 0;
	}

	if (!strcmp(s, "0"))
		return 0;

	return 1;
}