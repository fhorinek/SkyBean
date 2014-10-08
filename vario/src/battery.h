/*
 * battery.h
 *
 *  Created on: 16.1.2014
 *      Author: horinek
 */

#ifndef BATTERY_H_
#define BATTERY_H_

#include "xlib/core/adc.h"
#include "common.h"

void battery_init();
void battery_step();

#endif /* BATTERY_H_ */
