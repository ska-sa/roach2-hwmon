#include <stdlib.h>
#include <stdio.h>
#include "sensorlib.h"
#include "log.h"
#include "katcp.h"

#define SENSOR_CONF "roach2sensors.conf"


int sensorlib_load(void)
{
    FILE *configFile = NULL;
    int ret = -1;

    configFile = fopen(SENSOR_CONF, "r");

    if (configFile) {
        log_message(KATCP_LEVEL_INFO, "Config file found.\n");
        ret = sensors_init(configFile);
    } else {
        log_message(KATCP_LEVEL_INFO, "No config file, using defaults.\n");
        ret = sensors_init(NULL);
    }

    if (configFile) {
        fclose(configFile);
    }

    if (ret) {
        return -1;
    }

    return ret;
}

void sensorlib_unload(void)
{
    sensors_cleanup();
}
