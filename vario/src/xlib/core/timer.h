#ifndef TIMER_H_
#define TIMER_H_

#include "../common.h"
#include "gpio.h"

#define timerC4	&TCC4,	&PORTC


#define timerC5	&TCC5,	&PORTC
#define timerD5	&TCD5,	&PORTD


#define timer_A	0b00000001
#define timer_B	0b00000100
#define timer_C	0b00010000
#define timer_D	0b01000000

#define timerC4_overflow_interrupt	TCC4_OVF_vect
#define timerC4_error_interrupt		TCC4_ERR_vect
#define timerC4_compareA_interrupt	TCC4_CCA_vect
#define timerC4_compareB_interrupt	TCC4_CCB_vect
#define timerC4_compareC_interrupt	TCC4_CCC_vect
#define timerC4_compareD_interrupt	TCC4_CCD_vect

#define timerC5_overflow_interrupt	TCC5_OVF_vect
#define timerC5_error_interrupt		TCC5_ERR_vect
#define timerC5_compareA_interrupt	TCC5_CCA_vect
#define timerC5_compareB_interrupt	TCC5_CCB_vect

#define timerD5_overflow_interrupt	TCD5_OVF_vect
#define timerD5_error_interrupt		TCD5_ERR_vect
#define timerD5_compareA_interrupt	TCD5_CCA_vect
#define timerD5_compareB_interrupt	TCD5_CCB_vect

typedef enum xlib_core_timer_prescaler_e
{
	timer_div1 	= 0b0001,
	timer_div2 	= 0b0010,
	timer_div4 	= 0b0011,
	timer_div8 	= 0b0100,
	timer_div64	= 0b0101,
	timer_div256	= 0b0110,
	timer_div1024	= 0b0111,

	timer_event0	= 0b1000,
	timer_event1	= 0b1001,
	timer_event2	= 0b1010,
	timer_event3	= 0b1011,
	timer_event4	= 0b1100,
	timer_event5	= 0b1101,
	timer_event6	= 0b1110,
	timer_event7	= 0b1111,
} xlib_core_timer_prescaler;

typedef enum xlib_core_timer_mode_e
{
	timer_normal		= 0b000,
	timer_freq			= 0b001,
	timer_pwm			= 0b011,
	timer_ds_pwm_t		= 0b101,
	timer_ds_pwm_tb		= 0b110,
	timer_ds_pwm_b		= 0b111
} xlib_core_timer_mode;

typedef enum xlib_core_timer_interrupt_e
{
	timer_compareA = 0b00000001,
	timer_compareB = 0b00000010,
	timer_compareC = 0b00000100,
	timer_compareD = 0b00001000,
	timer_overflow = 0b00010000,
	timer_error	 = 0b00100000
} xlib_core_timer_interrupt;


class Timer
{
private:
	TC4_t * tc4;
	TC5_t * tc5;


	PORT_t * port;
	uint16_t pres;
	float ms_per_fs;
	uint16_t steps_per_ms;
	xlib_core_timer_prescaler prescaler;

	void Init(PORT_t * port, xlib_core_timer_prescaler pres);

	uint8_t irq_priority;

public:

	void Init(TC4_t * tc4, PORT_t * port, xlib_core_timer_prescaler pres);
	void Init(TC5_t * tc5, PORT_t * port, xlib_core_timer_prescaler pres);

	void SetMode(xlib_core_timer_mode mode);
	void EnableOutputs(uint8_t flags);
	void DisableOutputs(uint8_t flags);
	void EnableInterrupts(uint8_t inter);
	void DisableInterrupts(uint8_t inter);

	void Start();
	void Stop();
	uint16_t GetValue();
	void SetValue(uint16_t val);

	void SetTop(uint16_t top);
	void UpdateTop(uint16_t top);

	void SetCompare(uint8_t flag, uint16_t compare);

	void SetPrescaler(xlib_core_timer_prescaler pres);

	void SetInterruptPriority(uint8_t p);
	void ClearOverflowFlag();

	//TODO: not implemented
	float MsPerFs();
	uint16_t StepsPerMs();

};


#endif /* TIMER_H_ */
