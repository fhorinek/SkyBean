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
#include "profiles.h"

void LoadEEPROM();

void StoreVolume();
void StoreLift();
void StoreSink();
void LoadProfile();

void sys_tick_init();
uint32_t sys_tick_get();

extern profile prof;
extern configuration cfg;

#endif /* vCOMMON_H_ */
