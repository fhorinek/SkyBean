/*
 * adc.h
 *
 *  Created on: 23.6.2014
 *      Author: horinek
 */

#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#include "sp_driver.h"

#define VDC2ADC(V)	(210 * V + 314)

void adc_init();
void adc_deinit();

uint16_t adc_meas();

#endif /* ADC_H_ */
