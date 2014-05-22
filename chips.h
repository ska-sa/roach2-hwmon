/*
 * chips.h
 *
 *  Created on: 22 May 2014
 *      Author: rijandn
 */

#ifndef CHIPS_H_
#define CHIPS_H_

#define CHIPS_MAX 16

struct ChipLib {
	int numChipNames;
	sensors_chip_name chipNames[CHIPS_MAX];
};

void chips_parse(void);

int chips_numChipNames(void);

#endif /* CHIPS_H_ */
