/*
 * alarmhandler.c
 *
 *  Created on: 23 May 2014
 *      Author: rijandn
 */
#include <string.h>
#include "alarm.h"
#include "log.h"

int alarm_handler(const char *chipName, const char *label)
{
	/* fpga temp alarm triggered */
	if (!(strcmp(chipName, "max1668-i2c-0-18") && strcmp(label, "temp3"))) {
		log_message(KATCP_LEVEL_WARN, "fpga alarm triggered...unloading fpga");
	}

	return 0;
}
