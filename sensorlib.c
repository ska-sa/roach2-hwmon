#include <stdlib.h>
#include "sensorlib.h"

#define SENSOR_CONF_PATH        (NULL)


int sensorlib_load(void)
{
    int ret = -1;

    ret = sensors_init(SENSOR_CONF_PATH);
    if (ret) {
        return -1;
    }

    return ret;
}

void sensorlib_unload(void)
{
    sensors_cleanup();
}
