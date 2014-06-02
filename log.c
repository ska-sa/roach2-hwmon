/*
 * log.c
 *
 *  Created on: 21 May 2014
 *      Author: rijandn
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "log.h"

struct chipLookup {
	char *chip;
	char *name;
	char *sensor;
	char *desc;
	char *unit;
};

static const struct chipLookup table[] = {
		{"chip1", "name1", "sensor1", "desc1", "unit1"},
		{"chip2", "name2", "sensor2", "desc2", "unit2"},
		{"ad7414-i2c-0-4c", "temp1", "temp.inlet", "inlet temp", "millidegrees"},
		{"ad7414-i2c-0-4e", "temp1", "temp.outlet", "outlet temp", "millidegrees"},
		{"max1668-i2c-0-18", "temp1", "temp.ambient", "ambient temp", "millidegrees"},
		{"max1668-i2c-0-18", "temp2", "temp.ppc", "ambient temp", "millidegrees"},
		{"max1668-i2c-0-18", "temp3", "temp.fpga", "ambient temp", "millidegrees"},
		{"max16071-i2c-0-50", "in0", "voltage.1v0", "1v voltage rail", "millivolts"},
		{"max16071-i2c-0-50", "in1", "voltage.1v5", "1v5 voltage rail", "millivolts"},
		{"max16071-i2c-0-50", "in2", "voltage.1v8", "1v8 voltage rail", "millivolts"},
		{"max16071-i2c-0-50", "in3", "voltage.2v5", "2v5 voltage rail", "millivolts"},
		{"max16071-i2c-0-50", "in4", "voltage.3v3", "3v3 voltage rail", "millivolts"},
		{"max16071-i2c-0-50", "in5", "voltage.5v", "5v voltage rail", "millivolts"},
		{"max16071-i2c-0-50", "in6", "voltage.12v", "12v voltage rail", "millivolts"},
		{"max16071-i2c-0-50", "in7", "voltage.3v3aux", "3v3 aux voltage rail", "millivolts"},
		{"max16071-i2c-0-50", "curr1", "current.12v", "12v rail current", "milliamps"},
		{"max16071-i2c-0-51", "in0", "current.3v3", "1v voltage rail", "millivolts"},
		{"max16071-i2c-0-51", "in1", "current.2v5", "1v5 voltage rail", "millivolts"},
		{"max16071-i2c-0-51", "in2", "current.1v8", "1v8 voltage rail", "millivolts"},
		{"max16071-i2c-0-51", "in3", "current.1v5", "2v5 voltage rail", "millivolts"},
		{"max16071-i2c-0-51", "in4", "current.1v0", "3v3 voltage rail", "millivolts"},
		{"max16071-i2c-0-51", "in5", "ppc.sys.error", "5v voltage rail", "millivolts"},
		{"max16071-i2c-0-51", "in6", "voltage.5vaux", "5v aux voltage rail", "millivolts"},
		{"max16071-i2c-0-51", "curr1", "current.5v", "5v rail current", "milliamps"},
		{"max6650-i2c-0-48", "fan1", "fan.fpga", "fpga fan", "rpm"},
		{"max6650-i2c-0-4b", "fan1", "fan.chs0", "chassis 0 fan", "rpm"},
		{"max6650-i2c-0-1b", "fan1", "fan.chs1", "chassis 1 fan", "rpm"},
		{"max6650-i2c-0-1f", "fan1", "fan.chs2", "chassis 2 fan", "rpm"},
		{NULL, NULL, NULL, NULL, NULL}
};

static struct katcl_line *k = NULL;
static char *app = "roach2hwmon";

int getAttributes(const char *chip, char *name, char *sensor,
							char *description, char *unit)
{
	int i = 0;

	while (table[i].chip != NULL) {
		if (!(strcmp(chip, table[i].chip) && strcmp(name, table[i].name))) {
			sensor = table[i].sensor;
			description = table[i].desc;
			unit = table[i].unit;
			return 0;
		}
		i++;
	}

	sensor = name;
	description = "no description";
	unit = "unspecified";

	return 1;
}

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

int log_addsensor(const char *chip, char *name, double min, double max)
{
	int ret = 0;
	char namebuff[256];
	char descbuff[256];
	char unitbuff[256];

	if (k == NULL) {
		return -1;
	}

	/* populate sensor attributes */
	getAttributes(chip, name, namebuff, descbuff, unitbuff);

	/* register sensor(s) */
	ret += append_string_katcl(k, KATCP_FLAG_FIRST | KATCP_FLAG_STRING, "#sensor-list");
	ret += append_string_katcl(k, KATCP_FLAG_STRING, namebuff);
	ret += append_string_katcl(k, KATCP_FLAG_STRING, descbuff);
	ret += append_string_katcl(k, KATCP_FLAG_STRING, unitbuff);
	ret += append_string_katcl(k, KATCP_FLAG_STRING, "integer");

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

int log_update_sensor(const char *chip, char *name, int status, double val)
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

