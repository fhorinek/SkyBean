/*
 * profiles.h
 *
 *  Created on: 9.10.2014
 *      Author: horinek
 */

#ifndef PROFILES_H_
#define PROFILES_H_

#include <avr/io.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define CFG_SERIAL_OUTPUT_NONE	0
#define CFG_SERIAL_OUTPUT_DIGI	1
#define CFG_SERIAL_OUTPUT_DEBUG	2

struct profile
{
	uint8_t name[16];			//16	0

	uint16_t buzzer_freq[41]; 	//82	16
	uint16_t buzzer_pause[41]; 	//82	98
	uint16_t buzzer_length[41]; //82	180

	uint8_t lift_treshold;		//1		262
	uint8_t sink_treshold;		//1		263

	uint8_t enabled;			//1		264

	uint8_t reserved[6];		//6		265
};								//271 *3 = 813

typedef struct profile profile;

struct configuration
{
	uint8_t buzzer_volume;		//1		0		1 - 4
	uint8_t supress_startup;	//1		1		0 || 1

	uint16_t auto_poweroff;		//2		2		0 - 3600 (in sec)

	uint8_t serial_output;		//1		4		N/A
	uint8_t selected_profile;	//1		5		0 - 2

	int16_t lift_steps[5];		//10	6		-1500 - 1500 (in cm/s)
	int16_t	sink_steps[5];		//10	16		-1500 - 1500 (in cm/s) 0 is off

	float kalman_q;				//4		26
	float kalman_r;				//4		30
	float kalman_p;				//4		34

	uint8_t int_interval;		//1		38		12 - 50

	bool fluid_update;			//1		39
	uint8_t reserved[9];		//9		40
};								//49

typedef struct configuration configuration;

void configurator_loop();

#ifdef  __cplusplus
}
#endif

								//862 - spolu
#endif /* PROFILES_H_ */
