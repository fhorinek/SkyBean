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
Usart gps_usart;

CreateStdIn(usart_in, usart.Read);
CreateStdOut(usart_out, usart.Write);

char gps_buff[256];
uint8_t gps_buff_index = 0;

void pc_init()
{
	bool pc_go_to_cfg = false;

	SetStdIO(usart_in, usart_out);

	//Set up pin for programmer sensing
	GpioSetDirection(portd7, OUTPUT); //usart TX
	GpioSetDirection(portd6, INPUT);  //usart RX
	GpioSetPull(portd6, gpio_pull_down);

	_delay_ms(1);

	//Is programmer connected?
	if (GpioRead(portd6) == HIGH)
		pc_go_to_cfg = true;

	//return to default pin configuration
	GpioSetPull(portd6, gpio_totem);

	if (pc_go_to_cfg)
	{
		//start uart
		PR.PRPD = 0b01001101; //usart enabled
		usart.Init(usartD0, 115200, 8, 32);
		usart.SetInterruptPriority(MEDIUM);

		//communicate with the app
		configurator_loop();

		if (cfg.serial_output == 0)
		{
			//disable uart
			usart.Stop();
			GpioSetDirection(portd7, INPUT); //usart TX

			PR.PRPD = 0b01011101; //usart disabled
		}
	}

	if (cfg.serial_output > 0 && !pc_go_to_cfg)
	{
		//start uart
		GpioSetDirection(portd7, OUTPUT); //usart TX
		PR.PRPD = 0b01001101; //usart enabled
		usart.Init(usartD0, 115200, 8, 32);
		usart.SetInterruptPriority(MEDIUM);
	}

	if (cfg.serial_output == CFG_SERIAL_OUTPUT_DEBUG)
	{
		gps_usart.Init(usartC0, 9, 250, 0);
		gps_usart.SetInterruptPriority(MEDIUM);
	}
}

int checksum(char *s)
{
    int c = 0;

    while(*s)
        c ^= *s++;

    return c;
}

void pc_step()
{
	if (cfg.serial_output == CFG_SERIAL_OUTPUT_DEBUG)
	{
		//this is for production test only
		printf("%0.2f;%0.2f;%0.3f;%0.3f;\n", raw_pressure, pressure, altitude0, climb);
	}

	if (cfg.serial_output == CFG_SERIAL_OUTPUT_KOBO)
	{
		while (!gps_usart.isRxBufferEmpty())
		{
			uint8_t c = gps_usart.Read();
			gps_buff[gps_buff_index++] = c;
			if (c == '\n')
			{
				gps_buff[gps_buff_index] = 0;
				printf("%s", gps_buff);
				gps_buff_index = 0;
			}

		}
		char buff[512];

		sprintf(buff, "$D,%0.2f,%0.3f,,,%0.1f,,,,,,,", climb * 10, pressure, temperature);
		printf("%s*%02X\n", buff, checksum(buff));
	}
}
