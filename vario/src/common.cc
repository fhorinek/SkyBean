/*
 * common.cc
 *
 *  Created on: 7.11.2013
 *      Author: horinek
 */

#include "vario.h"

#include "common.h"
#include "profiles.h"

#include <stddef.h>

//kalman filter parameters
//default 0.001, 15.0, 30.0, 0.0
//
//float ram_kalman_q = 0.001;//0.001;
//
//float ram_kalman_r = 6.0;//15.0;
//
//float ram_kalman_p = 30.0;

//Altitude and pressure
float ram_sea_level_Pa = 101325;
float raw_pressure = 0;
float altitude0 = 0;

float temperature = 0;
float pressure = 0;
float climb = 0;

bool meas_new_sample = false;

float ram_climb_noise = 0.1;


//common and system
volatile uint16_t sys_tick_cnt = 0;

uint8_t buzzer_override = true;
uint16_t buzzer_override_tone;

//device CFG
struct profile EEMEM ee_prof[3];
struct configuration EEMEM ee_cfg;

//in ram
profile prof;
configuration cfg;

float ram_sink_begin;
float ram_lift_begin;

uint8_t sleep_now = false;

#define Check(var, min, max, def)	\
			do { \
				if (var < min || var > max) \
					var = def; \
			} while (0);

void LoadEEPROM()
{
	eeprom_busy_wait();

	eeprom_read_block(&cfg, &ee_cfg, sizeof(cfg));

	Check(cfg.buzzer_volume, 1, 4, 4);
	Check(cfg.supress_startup, 0, 1, 0);
	Check(cfg.auto_poweroff, 0, 3600, 60 * 5);
	Check(cfg.serial_output, 0, 10, 0);
	Check(cfg.selected_profile, 0, 2, 0);
	for (uint8_t i = 0; i < 5; i++)
	{
		Check(cfg.lift_steps[i], -1500, 1500, i * 10);
		Check(cfg.sink_steps[i], -1500, 1500, i * -50);
	}
	Check(cfg.kalman_q, 0, 10, 0.001);
	Check(cfg.kalman_r, 0, 10, 6.0);
	Check(cfg.kalman_p, 0, 100, 30.0);
	Check(cfg.int_interval, 12, 50, 25);

	eeprom_read_block(&prof, &ee_prof[cfg.selected_profile], sizeof(prof));
	for (uint8_t i = 0; i < 41; i++)
	{
		//todo: fallback table
		Check(prof.buzzer_freq[i], 0, 2000, 100);
		Check(prof.buzzer_pause[i], 0, 2000, 100);
		Check(prof.buzzer_length[i], 0, 2000, 100);
	}
	Check(prof.lift_treshold, 0, 5, 1);
	Check(prof.sink_treshold, 0, 5, 1);

	Check(prof.enabled, 0, 1, 1);


	LiftSinkRefresh();

	//NVM EE power reduction mode
	NVM.CTRLB |= 0b00000010;

}
void StoreVolume()
{
	eeprom_busy_wait();
	eeprom_update_byte(&ee_cfg.buzzer_volume, cfg.buzzer_volume);

	//NVM EE power reduction mode
	NVM.CTRLB |= 0b00000010;
}

void StoreLift()
{
	eeprom_busy_wait();
	eeprom_update_byte(&ee_prof[cfg.selected_profile].lift_treshold, prof.lift_treshold);

	//NVM EE power reduction mode
	NVM.CTRLB |= 0b00000010;
}

void StoreSink()
{
	eeprom_busy_wait();
	eeprom_update_byte(&ee_prof[cfg.selected_profile].sink_treshold, prof.sink_treshold);

	//NVM EE power reduction mode
	NVM.CTRLB |= 0b00000010;
}

void LoadProfile()
{
	eeprom_busy_wait();
	eeprom_update_byte(&ee_cfg.selected_profile, cfg.selected_profile);

	eeprom_read_block(&prof, &ee_prof[cfg.selected_profile], sizeof(prof));
	for (uint8_t i = 0; i < 41; i++)
	{
		//todo: fallback table
		Check(prof.buzzer_freq[i], 0, 2000, 100);
		Check(prof.buzzer_pause[i], 0, 2000, 100);
		Check(prof.buzzer_length[i], 0, 2000, 100);
	}
	Check(prof.lift_treshold, 0, 5, 1);
	Check(prof.sink_treshold, 0, 5, 1);

	Check(prof.enabled, 0, 1, 1);
}

ISR(rtc_overflow_interrupt)
{
	sys_tick_cnt += 1;
}

//1ms system timer for crude timing (button, battery measurement, etc...)
void sys_tick_init()
{
	RtcInit(rtc_1000Hz_ulp, rtc_div1);
	RtcEnableInterrupts(rtc_overflow);
}

uint32_t sys_tick_get()
{
	uint16_t act = RtcGetValue();

	return ((uint32_t)sys_tick_cnt << 16) | act;
}

