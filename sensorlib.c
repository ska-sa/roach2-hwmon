#include <stdlib.h>
#include <stdio.h>
#include "sensorlib.h"
#include "log.h"
#include "katcp.h"

#define DEFAULT_SENSOR_CONF "roach2sensors.conf"


int sensorlib_load(const char* cfgFilename)
{
    FILE *configFile = NULL;
    int ret = -1;

    if (cfgFilename) {
    	configFile = fopen(cfgFilename, "r");
    } else {
    	configFile = fopen(DEFAULT_SENSOR_CONF, "r");
    }

    if (configFile) {
        log_message(KATCP_LEVEL_DEBUG, "Sensor config file found.\n");
        ret = sensors_init(configFile);
    } else {
        log_message(KATCP_LEVEL_DEBUG, "Sensor config file not found, using defaults.\n");
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
