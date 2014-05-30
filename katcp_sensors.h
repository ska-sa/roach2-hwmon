/*
 * katcp_sensors.h
 *
 *  Created on: 30 May 2014
 *      Author: rijandn
 *
 *      This module creates katcp sensors by which is registered when r2hwmond
 *      is started as 'n subprocess from within tcpborserver.
 *
 */

#ifndef KATCP_SENSORS_H_
#define KATCP_SENSORS_H_

#include "katcl.h"
#include "katcp.h"

#ifdef __cplusplus
extern "C" {
#endif

int katcp_sensors_register(void);

#ifdef __cplusplus
}
#endif

#endif /* KATCP_SENSORS_H_ */
