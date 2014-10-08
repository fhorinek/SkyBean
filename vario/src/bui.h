/*
 * bui.h
 *
 *  Created on: 10.12.2013
 *      Author: horinek
 */

#ifndef BUI_H_
#define BUI_H_

#include "vario.h"

void bui_init();
void bui_step();

void start_sound(uint8_t sound);

void LiftSinkRefresh();

#define SOUND_NONE	0
#define SOUND_BEEP	1
#define SOUND_ON	2
#define SOUND_OFF	3
#define SOUND_LIFT	4
#define SOUND_SINK	5

#define SOUND_ADC	6
#define SOUND_TIM	7

#endif /* BUI_H_ */
