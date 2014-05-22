/*
 * log.c
 *
 *  Created on: 21 May 2014
 *      Author: rijandn
 */
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include "log.h"

static struct katcl_line *k = NULL;
static char *app = "roach2hwmon";

int log_init(void)
{
	k = create_katcl(STDOUT_FILENO);

	if (k == NULL) {
		fprintf(stderr, "unable to create katcl message logic.");
		return -1;
	}

	return 0;
}

int log_message(int loglevel, char *fmt, ...)
{
	va_list args;
	int ret = -1;

	if (k == NULL) {
		return -1;
	}

	va_start(args, fmt);
	ret = vlog_message_katcl(k, loglevel, app, fmt, args);
	va_end(args);

	return ret;
}


void log_cleanup(void)
{
	destroy_katcl(k, 0);
}

