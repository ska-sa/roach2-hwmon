/*
 * sense.c
 *
 *  Created on: 22 May 2014
 *      Author: rijandn
 */
#include <stdlib.h>
#include "sensord.h"
#include "sense.h"
#include "alarm.h"

#define DO_READ 0
#define DO_SCAN 1
#define DO_SET  2

extern struct ChipLib chipLib;

static int firstRead = 1;

static const char *chipName(const sensors_chip_name *chip)
{
    static char buffer[256];
    if (sensors_snprintf_chip_name(buffer, 256, chip) < 0)
        return NULL;
    return buffer;
}

static int idChip(const sensors_chip_name *chip)
{
    const char *name, *adapter;

    name = chipName(chip);
    if (!name) {
        log_message(KATCP_LEVEL_ERROR, "Error getting chip name");
        return -1;
    }

    log_message(KATCP_LEVEL_INFO, "Chip: %s", name);

    adapter = sensors_get_adapter_name(&chip->bus);
    if (!adapter)
        log_message(KATCP_LEVEL_ERROR, "Error getting adapter name");
    else
        log_message(KATCP_LEVEL_INFO, "Adapter: %s", adapter);

    return 0;
}

static int get_flag(const sensors_chip_name *chip, int num)
{
    double val;
    int ret;

    if (num == -1)
        return 0;

    ret = sensors_get_value(chip, num, &val);
    if (ret) {
        log_message(KATCP_LEVEL_ERROR, "Error getting sensor data: %s/#%d: %s",
              chip->prefix, num, sensors_strerror(ret));
        return -1;
    }

    return (int) (val + 0.5);
}

static int do_features(const sensors_chip_name *chip,
               const FeatureDescriptor *feature, int action)
{
    char *label;
    const char *formatted;
    int i, alrm, beep, ret;
    double val[MAX_DATA];

    /* If only scanning, take a quick exit if alarm is off */
    alrm = get_flag(chip, feature->alarmNumber);
    if (alrm == -1)
        return -1;
    if (action == DO_SCAN && !alrm)
        return 0;

    for (i = 0; feature->dataNumbers[i] >= 0; i++) {
        ret = sensors_get_value(chip, feature->dataNumbers[i],
                    val + i);
        if (ret) {
            log_message(KATCP_LEVEL_ERROR,
                  "Error getting sensor data: %s/#%d: %s",
                  chip->prefix, feature->dataNumbers[i],
                  sensors_strerror(ret));
            return -1;
        }
    }

    /* For scanning and reading, we need extra information */
    beep = get_flag(chip, feature->beepNumber);
    if (beep == -1)
        return -1;

    formatted = feature->format(val, alrm, beep);
    if (!formatted) {
        log_message(KATCP_LEVEL_ERROR, "Error formatting sensor data");
        return -1;
    }

    /* FIXME: It would be more efficient to store the label at
     * initialization time.
     */
    label = sensors_get_label(chip, feature->feature);
    if (!label) {
        log_message(KATCP_LEVEL_ERROR, "Error getting sensor label: %s/%s",
              chip->prefix, feature->feature->name);
        return -1;
    }

    if (action == DO_READ) {
    	if (!firstRead) {
    		/* update katcp sensor-status */
    		if (!log_update_sensor(chipName(chip), label, KATCP_LEVEL_INFO, val[0])) {
    			log_message(KATCP_LEVEL_ERROR, "Error updating sensor status: %s",
    		              label);
    		}
    		//log_message(KATCP_LEVEL_INFO, "  %s: %s", label, formatted);
    	} else {
    		/* update katcp sensor-list */
    		log_addsensor(chipName(chip), label, val[1], val[2]);
    	}
    } else {
    	/* alarm condition occurred */
    	log_update_sensor(chipName(chip), label, KATCP_LEVEL_WARN, val[0]);
        log_message(KATCP_LEVEL_WARN, "Sensor alarm: Chip %s: %s: %s",
              chipName(chip), label, formatted);
        alarm_handler(chipName(chip), label);
    }

    free(label);

    return 0;
}

static int doKnownChip(const sensors_chip_name *chip,
               const ChipDescriptor *descriptor, int action)
{
    const FeatureDescriptor *features = descriptor->features;
    int i, ret = 0;

    if (action == DO_READ) {
        ret = idChip(chip);
        if (ret)
            return ret;
    }

    for (i = 0; features[i].format; i++) {
        ret = do_features(chip, features + i, action);
        if (ret == -1)
            break;
    }

    return ret;
}

static int doChip(const sensors_chip_name *chip, int action)
{
    int ret = 0;
    int index0, chipindex = -1;

    for (index0 = 0; knownChips[index0].features; ++index0) {

        if (knownChips[index0].name == chip) {
            chipindex = index0;
            break;
        }

    }

    if (chipindex >= 0) {
        ret = doKnownChip(chip, &knownChips[chipindex], action);
    }

    return ret;

}

static int doChips(int action)
{
    const sensors_chip_name *chip, *chip_arg;
    int j, i, ret = 0;

    for (j = 0; j < chipLib.numChipNames; j++) {
        chip_arg = &chipLib.chipNames[j];
        i = 0;
        while ((chip = sensors_get_detected_chips(chip_arg, &i))) {
            ret = doChip(chip, action);
            if (ret) {
                return ret;
            }
        }
    }

    return ret;
}

int sense_scanChips(void)
{
    int ret = -1;

    log_message(KATCP_LEVEL_DEBUG, "sensor sweep started");
    ret = doChips(DO_SCAN);
    log_message(KATCP_LEVEL_DEBUG, "sensor sweep finished");

    return ret;
}

int sense_readChips(void)
{
    int ret = -1;

    log_message(KATCP_LEVEL_DEBUG, "sensor read started");
    ret = doChips(DO_READ);
    log_message(KATCP_LEVEL_DEBUG, "sensor read finished");

    if (firstRead) {
    	firstRead = 0;
    }

    return ret;
}

