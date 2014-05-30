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

    /* write the log data out */
    while ((ret = write_katcl(k)) == 0);

    if (ret < 0) {
        return -1;
    }

    return ret;
}

int log_sensorlist(void)
{
	int ret = 0;

	if (k == NULL) {
		return -1;
	}

	/* register sensor(s) */
	ret += append_string_katcl(k, KATCP_FLAG_FIRST | KATCP_FLAG_STRING, "#sensor-list");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "dummytemp");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "dummy sensor temperature");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "millidegrees");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "integer");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "0");
	ret += append_string_katcl(k, KATCP_FLAG_LAST | KATCP_FLAG_STRING, "80000");

    /* write the log data out */
    while ((ret = write_katcl(k)) == 0);

	return ret;
}

int log_addsensor(char *name, double min, double max)
{
	int ret = 0;

	if (k == NULL) {
		return -1;
	}

	/* register sensor(s) */
	ret += append_string_katcl(k, KATCP_FLAG_FIRST | KATCP_FLAG_STRING, "#sensor-list");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, name);
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "description");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "millidegrees");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "integer");

	//ret += append_string_katcl(k, KATCP_FLAG_STRING, "0");
	//ret += append_string_katcl(k, KATCP_FLAG_LAST | KATCP_FLAG_STRING, "80000");

	/* multiply with a 1000 since our units are in milli's */
	ret += append_signed_long_katcl(k, KATCP_FLAG_SLONG, (long)(min*1000));
	ret += append_signed_long_katcl(k, KATCP_FLAG_LAST | KATCP_FLAG_SLONG, (long)(max*1000));

    /* write the log data out */
    while ((ret = write_katcl(k)) == 0);

	return ret;
}

int log_sensorstatus(void)
{
	int ret = 0;

	if (k == NULL) {
		return -1;
	}

	/* update sensor status */
	ret += append_string_katcl(k, KATCP_FLAG_FIRST | KATCP_FLAG_STRING, "#sensor-status");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "123456789");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "1");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "dummytemp");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "nominal");
	ret += append_string_katcl(k, KATCP_FLAG_LAST | KATCP_FLAG_STRING, "23000");

    /* write the log data out */
    while ((ret = write_katcl(k)) == 0);

	return ret;
}

int log_update_sensor(char *name, int status, double val)
{
	int ret = 0;

	if (k == NULL) {
		return -1;
	}

	/* update sensor status */
	ret += append_string_katcl(k, KATCP_FLAG_FIRST | KATCP_FLAG_STRING, "#sensor-status");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "123456789");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "1");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, name);

	if (status == KATCP_LEVEL_INFO) {
		ret += append_string_katcl(k, KATCP_FLAG_STRING, "nominal");
	} else if (status == KATCP_LEVEL_WARN){
		ret += append_string_katcl(k, KATCP_FLAG_STRING, "warn");
	} else if (status == KATCP_LEVEL_ERROR){
		ret += append_string_katcl(k, KATCP_FLAG_STRING, "error");
	} else if (status == KATCP_LEVEL_FATAL){
		ret += append_string_katcl(k, KATCP_FLAG_STRING, "failure");
	} else {
		ret += append_string_katcl(k, KATCP_FLAG_STRING, "unknown");
	}

	/* multiply with a 1000 since our units are in milli's */
	ret += append_signed_long_katcl(k, KATCP_FLAG_LAST | KATCP_FLAG_SLONG, (long)(val*1000));

    /* write the log data out */
    while ((ret = write_katcl(k)) == 0);

	return ret;
}

void log_cleanup(void)
{
    if (k != NULL) {
        destroy_katcl(k, 0);
        k = NULL;
    }
}

