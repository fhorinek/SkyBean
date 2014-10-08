/*
 * pc.cc
 *
 *  Created on: 7.11.2013
 *      Author: horinek
 */

#include "pc.h"

extern float temperature;
extern float altitude0;

extern float ram_sea_level_Pa;
extern float ram_kalman_q;
extern float ram_kalman_r;
extern float ram_kalman_p;
extern float raw_pressure;
extern float pressure;
extern float climb;

extern bool meas_new_sample;

Usart usart;

CreateStdIn(usart_in, usart.Read);
CreateStdOut(usart_out, usart.Write);


void pc_init()
{
	GpioSetDirection(portd7, OUTPUT);

	usart.Init(usartD0, 115200, 0, 32);
	usart.SetInterruptPriority(MEDIUM);

	SetStdIO(usart_in, usart_out);
}

//this is for production test only
void pc_step()
{
	printf("%0.2f;%0.2f;%0.3f;%0.3f;\n", raw_pressure, pressure, altitude0, climb);
}

