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

void log_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* _ROACH_LOG_H_ */
