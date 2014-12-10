/*
 * vario.h
 *
 *  Created on: 4.12.2013
 *      Author: horinek
 */

#ifndef VARIO_H_
#define VARIO_H_

#include "xlib/core/i2c.h"
#include "xlib/core/clock.h"
#include "xlib/core/system.h"
#include "xlib/core/timer.h"

#include "filter.h"
#include "common.h"
#include "bui.h"
#include "buzzer.h"
#include "pc.h"
#include "ms5611.h"
#include "bmp180.h"
#include "battery.h"

//always clean when changing sensor type or clock speed!!!
#ifdef SENSOR_TYPE_MS
	#define SENSOR	MS5611 sensor
#else
	#define SENSOR	BMP180 sensor
#endif

//#define FLUID_LIFT
#define FLUID_LIFT_REFRESH	20
//1 == 20ms

#define FAST_CLOCK
//#define SLOW_CLOCK

//uart debug & timing pin
//#define ENABLE_DEBUG_TIMING
#define ENABLE_DEBUG_UART

//pinout
#define LEDR			porta1
#define LEDG			porta0
#define B_EN			portc3
#define I2C_EN			portd4
#define BUTTON_PIN		portc2
#define DEBUG_PIN		portd6
#define BM_PIN			porta3

//leds
#define LEDR_ON			GpioWrite(LEDR, LOW);
#define LEDR_OFF		GpioWrite(LEDR, HI);
#define LEDG_ON			GpioWrite(LEDG, LOW);
#define LEDG_OFF		GpioWrite(LEDG, HI);

//etc
#define BUTTON_DOWN		(GpioRead(BUTTON_PIN) == LOW)

//auto power off constants
//#define AUTO_TIMEOUT	(60000 * 5) //5min
#define AUTO_THOLD		1 //+-1m == max diff 2m


#endif /* VARIO_H_ */
