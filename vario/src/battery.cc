/*
 * battery.cc
 *
 *  Created on: 16.1.2014
 *      Author: horinek
 */

#include "battery.h"
#include "vario.h"

void battery_init()
{
	AdcAInit(adc_int1V);
	AdcASetMode(adc_signed);

	AdcPipeSetSource(pipea0, ext_porta2);
}


#define BATT_IDLE	0
#define BATT_START	1
#define BATT_RESULT	2

uint8_t battery_meas_state = BATT_START;
int16_t adc_raw = 0;

//210*x + 314
#define VDC2ADC(V)	(210 * V + 314)

//#define BATT_THOLD	VDC2ADC(2.4)
//#define BATT_THOLD	VDC2ADC(2.3)
#define BATT_THOLD	VDC2ADC(2.0)

uint32_t battery_next_meas = 0;

uint16_t meas_acc = 0;
uint8_t meas_cnt = 0;

#define MEAS_AVG 	10

extern uint16_t sound_duration;

void battery_step()
{
	//block loop if sound is running
	if (sound_duration > 0)
		return;

	if (battery_next_meas > sys_tick_get())
		return;

	switch (battery_meas_state)
	{
	case(BATT_IDLE):
		adc_raw = meas_acc/MEAS_AVG;
		//printf("adc %u\n", adc_raw);
		if (adc_raw < BATT_THOLD)
		{
			start_sound(SOUND_ADC);
		#ifdef ENABLE_DEBUG_UART
			printf("battery off\n");
		#endif
		}

		battery_meas_state = BATT_START;
		//measure every minute
		battery_next_meas = sys_tick_get() + 60000;
		meas_acc = 0;
		meas_cnt = 0;

	break;
	case(BATT_START):
		//enable ADC
		PR.PRPA = 0b00000101;
		ADCA.CTRLA |= 0b00000001;

		GpioSetDirection(BM_PIN, OUTPUT);
		GpioWrite(BM_PIN, HIGH);
		AdcPipeStart(pipea0);

		battery_meas_state = BATT_RESULT;
	break;
	case(BATT_RESULT):
		GpioSetDirection(BM_PIN, INPUT);
		meas_acc += AdcPipeValue(pipea0);

		//disable ADC
		ADCA.CTRLA &= ~(0b00000001);
		PR.PRPA = 0b00000111;

		meas_cnt++;
		if (meas_cnt >= MEAS_AVG)
			battery_meas_state = BATT_IDLE;
		else
			battery_meas_state = BATT_START;
	break;
	}
}
