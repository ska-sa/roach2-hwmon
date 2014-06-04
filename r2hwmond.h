/*
 * r2hwmond.h
 *
 *  Created on: 30 May 2014
 *      Author: rijandn
 */

#ifndef R2HWMOND_H_
#define R2HWMOND_H_

#define R2HWMOND_SCAN_INTERVAL_S    (5)
#define R2HWMOND_UPDATE_INTERVAL_S  (10)
#define R2HWMOND_SERVER				"localhost:7147"

struct r2hwmond_configuration {
	int daemon;
	int cfgFile;
	const char *sensorCfgFile;
	int scanTime;
	int updateTime;
};

extern struct r2hwmond_configuration r2hwmond_cfg;

#endif /* R2HWMOND_H_ */
