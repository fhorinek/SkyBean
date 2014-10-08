/*
 * common.cc
 *
 *  Created on: 7.11.2013
 *      Author: horinek
 */

#include "vario.h"

#include "common.h"


//kalman filter parameters
//default 0.001, 15.0, 30.0, 0.0

float ram_kalman_q = 0.001;//0.001;

float ram_kalman_r = 6.0;//15.0;

float ram_kalman_p = 30.0;

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

uint8_t ram_sink_begin = 1;
uint8_t ram_sink_end = 150;
uint8_t ram_lift_begin = 1;
uint8_t ram_lift_end = 150;

uint8_t buzzer_override = true;
uint16_t buzzer_override_tone;

//device CFG
volatile uint8_t ram_buzzer_volume = 4;
uint8_t EEMEM ee_buzzer_volume = 4;

uint8_t ram_lift_cfg = 2;
uint8_t EEMEM ee_lift_cfg = 2;

uint8_t ram_sink_cfg = 2;
uint8_t EEMEM ee_sink_cfg = 2;


uint8_t sleep_now = false;

void LoadEEPROM()
{
	eeprom_busy_wait();

	ram_buzzer_volume = eeprom_read_byte(&ee_buzzer_volume);
	if (ram_buzzer_volume > 4 || ram_buzzer_volume == 0)
		ram_buzzer_volume = 4;

	ram_lift_cfg = eeprom_read_byte(&ee_lift_cfg);
	if (ram_lift_cfg > 5 || ram_lift_cfg == 0)
		ram_lift_cfg = 2;

	ram_sink_cfg = eeprom_read_byte(&ee_sink_cfg);
	if (ram_sink_cfg > 8 || ram_sink_cfg == 0)
		ram_sink_cfg = 2;

	LiftSinkRefresh();

	//NVM EE power reduction mode
	NVM.CTRLB |= 0b00000010;

}
void StoreVolume()
{
	eeprom_busy_wait();
	eeprom_write_byte(&ee_buzzer_volume, ram_buzzer_volume);

	//NVM EE power reduction mode
	NVM.CTRLB |= 0b00000010;
}

void StoreLift()
{
	eeprom_busy_wait();
	eeprom_write_byte(&ee_lift_cfg, ram_lift_cfg);

	//NVM EE power reduction mode
	NVM.CTRLB |= 0b00000010;
}

void StoreSink()
{
	eeprom_busy_wait();
	eeprom_write_byte(&ee_sink_cfg, ram_sink_cfg);

	//NVM EE power reduction mode
	NVM.CTRLB |= 0b00000010;
}


ISR(rtc_overflow_interrupt)
{
	sys_tick_cnt += 1;
}

//1ms system timer for crude timing (button, battery measurement, etc...)
void init_sys_tick()
{
	RtcInit(rtc_1000Hz_ulp, rtc_div1);
	RtcEnableInterrupts(rtc_overflow);
}

uint32_t get_sys_tick()
{
	uint16_t act = RtcGetValue();

	return ((uint32_t)sys_tick_cnt << 16) | act;
}

