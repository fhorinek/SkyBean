/*
 * buzzer.h
 *
 *  Created on: 7.11.2013
 *      Author: horinek
 */

#ifndef BUZZER_H_
#define BUZZER_H_


#include "vario.h"

void buzzer_init();
void buzzer_step();

void buzzer_set_tone(uint16_t tone);
void buzzer_set_delay(uint16_t length, uint16_t pause);

#endif /* BUZZER_H_ */
