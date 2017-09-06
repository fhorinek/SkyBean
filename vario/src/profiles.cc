#include "profiles.h"
#include <avr/eeprom.h>
#include "xlib/core/usart.h"
#include "build_number.h"
#include "cfg.h"

//device CFG
extern profile EEMEM ee_prof[3];
extern configuration EEMEM ee_cfg;

extern profile prof;
extern configuration cfg;

extern Usart usart;

//union helpers
union cfg_helper
{
	configuration * cfg;
	uint8_t * byte;
};

union prof_helper
{
	profile * prof;
	uint8_t * byte;
};

#define CRC_KEY	0xAB

void configurator_loop()
{
	uint16_t timeout = 1000;

	_delay_ms(1000);

	printf("skybean");

	//send build version
	usart.Write((BUILD_NUMBER & 0xFF00) >> 8);
	usart.Write((BUILD_NUMBER & 0x00FF) >> 0);

	union cfg_helper chelper;
	union prof_helper phelper;

	chelper.cfg = &cfg;
	phelper.prof = &prof;

	uint8_t crc = 0;
	uint8_t tmp;

	uint8_t data_buffer[sizeof(cfg) + sizeof(prof) * 3];

	while(timeout > 0)
	{
		if (!usart.isRxBufferEmpty())
		{
			timeout = 1000;

			uint8_t c = usart.Read();

			if (c == 'R')
			{
				LEDG_ON;

				crc = 0;

				eeprom_busy_wait();
				eeprom_read_block(chelper.cfg, &ee_cfg, sizeof(cfg));
				for (uint8_t i = 0; i < sizeof(cfg); i++)
				{
					tmp = chelper.byte[i];
					crc = CalcCRC(crc, CRC_KEY, tmp);
					usart.Write(tmp);
				}

				//load all 3 profiles
				for (uint8_t i = 0; i < 3; i++)
				{
					eeprom_busy_wait();
					eeprom_read_block(phelper.prof, &ee_prof[i], sizeof(prof));

					for (uint16_t j = 0; j < sizeof(prof); j++)
					{
						tmp = phelper.byte[j];
						crc = CalcCRC(crc, CRC_KEY, tmp);
						usart.Write(tmp);
					}
				}

				//send CRC
				usart.Write(crc);

				LEDG_OFF;
			}

			if (c == 'W')
			{
				LEDR_ON;

				crc = 0;

				for (uint16_t i = 0; i < sizeof(data_buffer); i++)
				{
					tmp = usart.Read();
					crc = CalcCRC(crc, CRC_KEY, tmp);
					data_buffer[i] = tmp;
					//progress signaling
					usart.Write('.');
				}

				//read CRC
				tmp = usart.Read();

				//if there was an CRC error interrupt EE write
				if (crc != tmp)
				{
					printf("e");

					break;
				}

				//block_update is not working (why??)
				for (uint16_t i=0; i<sizeof(data_buffer); i++)
				{
					eeprom_busy_wait();
					eeprom_update_byte((uint8_t *)i, data_buffer[i]);
				}

				//all done
				printf("o");
				LEDR_OFF;
			}
		}
		else
		{
			timeout--;
			_delay_ms(1);
		}
	}
	printf("\n\n");

	for (uint16_t i = 0; i < sizeof(data_buffer); i++)
	{
		//progress signaling
		printf("%d ", data_buffer[i]);
	}


	printf("\n\nEE dump\n");
	#define EE_MUL	32
	uint8_t ee_buff[EE_MUL];
	for (uint8_t i = 0; EE_MUL*i < 1024; i++)
	{
		eeprom_busy_wait();
		eeprom_read_block(ee_buff, (const void*)(EE_MUL*i), EE_MUL);
		printf("%04d: ", EE_MUL * i);
		for (uint8_t j=0;j<EE_MUL; j++)
			printf("%02X ", ee_buff[j]);
		printf("\n");
	}

	printf("\n--- cfg is ---\n");
	printf("buzzer_volume %d\n", cfg.buzzer_volume);
	printf("supress_startup %d\n", cfg.supress_startup);
	printf("auto_poweroff %d\n", cfg.auto_poweroff);
	printf("serial_output %d\n", cfg.serial_output);
	printf("selected_profile %d\n", cfg.selected_profile);

	printf("lift_steps: ");
	for (uint8_t i=0; i < 5; i++)
		printf("%3d ", cfg.lift_steps[i]);
	printf("\n");

	printf("sink_steps: ");
	for (uint8_t i=0; i < 5; i++)
		printf("%d ", cfg.sink_steps[i]);
	printf("\n");

	printf("kalman_q %0.4f\n", cfg.kalman_q);
	printf("kalman_r %0.4f\n", cfg.kalman_r);
	printf("kalman_p %0.4f\n", cfg.kalman_p);

	printf("int_interval %d\n", cfg.int_interval);

	for(uint8_t i = 0; i < 3; i++)
	{
		eeprom_busy_wait();
		eeprom_read_block(phelper.prof, &ee_prof[i], sizeof(prof));

		printf("\n--- profile %i ---\n", i);

		printf("name %16s\n", prof.name);

		printf("buzzer_freq:   ");
		for (uint8_t j=0; j < 41; j++)
			printf("%4d ", prof.buzzer_freq[j]);
		printf("\n");

		printf("buzzer_pause:  ");
		for (uint8_t j=0; j < 41; j++)
			printf("%4d ", prof.buzzer_pause[j]);
		printf("\n");

		printf("buzzer_length: ");
		for (uint8_t j=0; j < 41; j++)
			printf("%4d ", prof.buzzer_length[j]);
		printf("\n");

		printf("lift_treshold %d\n", prof.lift_treshold);
		printf("sink_treshold %d\n", prof.sink_treshold);
		printf("enabled %d\n", prof.enabled);
	}

	//NVM EE power reduction mode
	NVM.CTRLB |= 0b00000010;
}
