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

#ifdef __cplusplus
extern "C" {
#endif

int sense_readChips(void);

int sense_scanChips(void);

#ifdef __cplusplus
}
#endif

#endif /* _SENSE_H_ */
