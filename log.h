/*
 * log.h
 *
 *  Created on: 21 May 2014
 *      Author: rijandn
 */

#include "katcp.h"
#include "katcl.h"

#ifndef _ROACH_LOG_H_
#define _ROACH_LOG_H_

int log_init(void);

int log_message(int loglevel, char *fmt, ...);

void log_cleanup(void);

#endif /* _ROACH_LOG_H_ */
