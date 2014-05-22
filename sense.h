/*
 * sense.h
 *
 *  Created on: 22 May 2014
 *      Author: rijandn
 */

#ifndef _SENSE_H_
#define _SENSE_H_

#include "lib/sensors.h"
#include "lib/error.h"
#include "chips.h"
#include "log.h"

int sense_readChips(void);

int sense_scanChips(void);

#endif /* _SENSE_H_ */
