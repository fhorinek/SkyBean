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

//device CFG
extern profile EEMEM ee_prof[3];
extern configuration EEMEM ee_cfg;

void pc_cfg()
{
	uint16_t timeout = 1000;

	printf("skybean");

	while(timeout > 0)
	{
		if (!usart.isRxBufferEmpty())
		{
			timeout = 1000;

			uint8_t c = usart.Read();

			if (c == 'R')
			{
				for (uint8_t i = 0; i < sizeof(cfg); i++)
				{
					usart.Write(*((uint8_t *)&cfg + i));
				}
				//load all 3 profiles
				for (uint8_t i = 0; i < 3; i++)
				{
					eeprom_busy_wait();
					eeprom_read_block(&prof, &ee_prof[i], sizeof(prof));

					for (uint16_t j = 0; j < sizeof(prof); j++)
					{
						usart.Write(*((uint8_t *)&prof + j));
					}
				}
			}
			if (c == 'V')
			{
				usart.Write((BUILD_NUMBER & 0xFF00) >> 8);
				usart.Write((BUILD_NUMBER & 0x00FF) >> 0);
			}

		}
		else
		{
			timeout--;
			_delay_ms(1);
		}
	}

	usart.Stop();
	PR.PRPD = 0b01011101; //usart disabled
}

void pc_init()
{
	bool pc_go_to_cfg = false;

	GpioSetDirection(portd7, OUTPUT); //usart TX
	GpioSetDirection(portd6, INPUT);  //usart RX
	GpioSetPull(portd6, gpio_pull_down);

	_delay_ms(1);

	if (GpioRead(portd6) == HIGH)
		pc_go_to_cfg = true;

	GpioSetPull(portd6, gpio_totem);

#ifdef ENABLE_DEBUG_UART
	pc_go_to_cfg = true;
#endif

	if (pc_go_to_cfg)
	{
		PR.PRPD = 0b01001101; //usart enabled

		usart.Init(usartD0, 115200, 8, 32);
		usart.SetInterruptPriority(MEDIUM);

		SetStdIO(usart_in, usart_out);

		pc_cfg();
	}
}

//this is for production test only
void pc_step()
{
	printf("%0.2f;%0.2f;%0.3f;%0.3f;\n", raw_pressure, pressure, altitude0, climb);
}
