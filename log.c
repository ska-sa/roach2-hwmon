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
#include <sys/time.h>
#include "log.h"

struct chipLookup {
	const char *chip;
	const char *name;
	const char *sensor;
	const char *desc;
	const char *unit;
	float mult;
	float div;
};

static const struct chipLookup table[] = {
		/* Adding sensor mappings
           {"chip1", "name1", "sensor1", "desc1", "unit1"},
		   {"chip2", "name2", "sensor2", "desc2", "unit2"}, */
		{"acpitz-virtual-0", "temp1", "hp.temp", "hp temp sensor", "milidegrees", 1, 1},
		{"ad7414-i2c-0-4c", "temp1", "temp.inlet", "inlet temp", "millidegrees", 1, 1},
		{"ad7414-i2c-0-4e", "temp1", "temp.outlet", "outlet temp", "millidegrees", 1, 1},
		{"max1668-i2c-0-18", "temp1", "temp.ambient", "ambient temp", "millidegrees", 1, 1},
		{"max1668-i2c-0-18", "temp2", "temp.ppc", "ppc temp", "millidegrees", 1, 1},
		{"max1668-i2c-0-18", "temp3", "temp.fpga", "fpga temp", "millidegrees", 1, 1},
		{"max16071-i2c-0-50", "in0", "voltage.1v0", "1v voltage rail", "millivolts", 1, 1},
		{"max16071-i2c-0-50", "in1", "voltage.1v5", "1v5 voltage rail", "millivolts", 1, 1},
		{"max16071-i2c-0-50", "in2", "voltage.1v8", "1v8 voltage rail", "millivolts", 1, 1},
		{"max16071-i2c-0-50", "in3", "voltage.2v5", "2v5 voltage rail", "millivolts", 1, 1},
		{"max16071-i2c-0-50", "in4", "voltage.3v3", "3v3 voltage rail", "millivolts", 1, 1},
		{"max16071-i2c-0-50", "in5", "voltage.5v", "5v voltage rail", "millivolts", 1, 1},
		{"max16071-i2c-0-50", "in6", "voltage.12v", "12v voltage rail", "millivolts", 10, 29.6},
		{"max16071-i2c-0-50", "in7", "voltage.3v3aux", "3v3 aux voltage rail", "millivolts", 1, 1},
		{"max16071-i2c-0-50", "curr1", "current.12v", "12v rail current", "milliamps", 1, 1},
		{"max16071-i2c-0-51", "in0", "current.3v3", "1v rail current", "milliamps", 1, 1},
		{"max16071-i2c-0-51", "in1", "current.2v5", "1v5 rail current", "milliamps", 1, 1},
		{"max16071-i2c-0-51", "in2", "current.1v8", "1v8 rail current", "milliamps", 1, 1},
		{"max16071-i2c-0-51", "in3", "current.1v5", "2v5 rail current", "milliamps", 1, 1},
		{"max16071-i2c-0-51", "in4", "current.1v0", "3v3 rail current", "milliamps", 1, 1},
		{"max16071-i2c-0-51", "in6", "voltage.5vaux", "5v aux voltage rail", "millivolts", 1, 1},
		{"max16071-i2c-0-51", "curr1", "current.5v", "5v rail current", "milliamps", 1, 1},
		{"max6650-i2c-0-48", "fan1", "fan.fpga", "fpga fan", "rpm", 1000, 1},
		{"max6650-i2c-0-4b", "fan1", "fan.chs0", "chassis 0 fan", "rpm", 1000, 1},
		{"max6650-i2c-0-1b", "fan1", "fan.chs1", "chassis 1 fan", "rpm", 1000, 1},
		{"max6650-i2c-0-1f", "fan1", "fan.chs2", "chassis 2 fan", "rpm", 1000, 1},
		{NULL, NULL, NULL, NULL, NULL}
};

static struct katcl_line *k = NULL;
static char *app = "roach2hwmon";

/*
static int getAttributes(const char *chip, char *name, const char *sensor,
							const char *description, const char *unit)
{
	int i = 0;

	while (table[i].chip != NULL) {
        if (strcmp(chip, table[i].chip) == 0) {
            if (strcmp(name, table[i].name) == 0) {
			    sensor = table[i].sensor;
			    description = table[i].desc;
			    unit = table[i].unit;
			    return 0;
            }
		}
		i++;
	}

	sensor = NULL;
	description = NULL;
	unit = NULL;

	return 1;
}
*/
static int getSensorIndex(const char *chip, char *name)
{
	int i = 0;

	while (table[i].chip != NULL) {
		if (strcmp(chip, table[i].chip) == 0) {
			if (strcmp(name, table[i].name) == 0) {
				return i;
			}
		}
		i++;
	}

	return -1;
}

static const char *getSensor(int index)
{
	if (index >= 0) {
		return table[index].sensor;
	}

	return NULL;
}

static const char *getDescription(int index)
{
	if (index >= 0) {
		return table[index].desc;
	}

	return NULL;
}

static const char *getUnit(int index)
{
	if (index >= 0) {
		return table[index].unit;
	}

	return NULL;
}

static float getMult(int index)
{
	if (index >= 0) {
		return table[index].mult;
	}

	return 1;
}

static float getDiv(int index)
{
	if (index >= 0) {
		return table[index].div;
	}

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

int log_addsensor(const char *chip, char *name, double min, double max)
{
	long minVal = 0, maxVal = 0;
	int sensorIndex = -1;
	int ret = 0;

	if (k == NULL) {
		return -1;
	}

	/* check if the sensor exists in the table */
	sensorIndex = getSensorIndex(chip, name);

	/* register sensor(s) */
	if (sensorIndex >= 0) {
	    ret += append_string_katcl(k, KATCP_FLAG_FIRST | KATCP_FLAG_STRING, KATCP_SENSOR_LIST_INFORM);
	    ret += append_string_katcl(k, KATCP_FLAG_STRING, (char*)getSensor(sensorIndex));
	    ret += append_string_katcl(k, KATCP_FLAG_STRING, (char*)getDescription(sensorIndex));
	    ret += append_string_katcl(k, KATCP_FLAG_STRING, (char*)getUnit(sensorIndex));
	    ret += append_string_katcl(k, KATCP_FLAG_STRING, "integer");

	    /* multiply with a 1000 since our units are in milli's */
	    minVal = (min * 1000) * getMult(sensorIndex) / getDiv(sensorIndex);
	    maxVal = (max * 1000) * getMult(sensorIndex) / getDiv(sensorIndex);

	    ret += append_signed_long_katcl(k, KATCP_FLAG_SLONG, minVal);
	    ret += append_signed_long_katcl(k, KATCP_FLAG_LAST | KATCP_FLAG_SLONG, maxVal);

        /* write the log data out */
        while ((ret = write_katcl(k)) == 0);
	}

	return ret;
}

int log_update_sensor(const char *chip, char *name, int status, double val)
{
	struct timeval now;
	int sensorIndex = -1;
	long value = 0;
    int ret = 0;

	if (k == NULL) {
		return -1;
	}

	/* check if the sensor exists in the table */
	sensorIndex = getSensorIndex(chip, name);

	if (sensorIndex >= 0) {
        gettimeofday(&now, NULL);

	    /* update sensor status */
	    ret += append_string_katcl(k, KATCP_FLAG_FIRST | KATCP_FLAG_STRING, KATCP_SENSOR_STATUS_INFORM);
        ret += append_unsigned_long_katcl(k, KATCP_FLAG_ULONG, (unsigned long)now.tv_sec);
/*
#if KATCP_PROTOCOL_MAJOR_VERSION >= 5
        ret += append_args_katcl(k, 0, "%lu.%03d", now.tv_sec, milli);
#else
        ret += append_args_katcl(k, 0, "%lu%03d", now.tv_sec, milli);
#endif
*/
	    ret += append_string_katcl(k, KATCP_FLAG_STRING, "1");
	    ret += append_string_katcl(k, KATCP_FLAG_STRING, (char*)getSensor(sensorIndex));

	    if (status == KATCP_STATUS_NOMINAL) {
		    ret += append_string_katcl(k, KATCP_FLAG_STRING, "nominal");
	    } else if (status == KATCP_STATUS_WARN) {
		    ret += append_string_katcl(k, KATCP_FLAG_STRING, "warn");
	    } else if (status == KATCP_STATUS_ERROR) {
		    ret += append_string_katcl(k, KATCP_FLAG_STRING, "error");
	    } else if (status == KATCP_STATUS_FAILURE) {
		    ret += append_string_katcl(k, KATCP_FLAG_STRING, "failure");
	    } else {
		    ret += append_string_katcl(k, KATCP_FLAG_STRING, "unknown");
	    }

	    /* multiply with a 1000 since our units are in milli's */
	    value = (long)((val * 1000) / getMult(sensorIndex) * getDiv(sensorIndex));
	    ret += append_signed_long_katcl(k, KATCP_FLAG_LAST | KATCP_FLAG_SLONG, value);

        /* write the log data out */
        while ((ret = write_katcl(k)) == 0);
    }

	return ret;
}

void log_cleanup(void)
{
    if (k != NULL) {
        destroy_katcl(k, 0);
        k = NULL;
    }
}

