/*
 * log.h
 *
 *  Created on: 21 May 2014
 *      Author: rijandn
 */

#ifndef _ROACH_LOG_H_
#define _ROACH_LOG_H_

#include "katcp.h"
#include "katcl.h"

#ifdef __cplusplus
extern "C" {
#endif

int log_init(void);

int log_message(int loglevel, char *fmt, ...);

int log_sensorlist(void);

int log_addsensor(const char *chip, char *name, double min, double max);

int log_sensorstatus(void);

int log_update_sensor(const char *chip, char *name, int status, double val);

inline void log_disable(void);

void log_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* _ROACH_LOG_H_ */
