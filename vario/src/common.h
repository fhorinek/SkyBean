/*
 * common.h
 *
 *  Created on: 7.11.2013
 *      Author: horinek
 */

#ifndef vCOMMON_H_
#define vCOMMON_H_

#include "xlib/common.h"
#include "xlib/core/rtc.h"
#include "xlib/core/timer.h"
#include "xlib/core/usart.h"


void LoadEEPROM();

void StoreVolume();
void StoreLift();
void StoreSink();

void init_sys_tick();
uint32_t get_sys_tick();

#endif /* vCOMMON_H_ */
