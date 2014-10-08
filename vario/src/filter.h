/*
 * sensor.h
 *
 *  Created on: 7.11.2013
 *      Author: horinek
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include "xlib/core/gpio.h"
#include "xlib/core/timer.h"
#include "xlib/core/usart.h"
#include "xlib/core/system.h"

#include "common.h"
#include "bmp180.h"
#include "ms5611.h"
#include "vario.h"

void filter_init();
void filter_step();

#endif /* SENSOR_H_ */
