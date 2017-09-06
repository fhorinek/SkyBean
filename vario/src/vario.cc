/*
 * vario.cc
 *
 *  Created on: 18.10.2013
 *      Author: horinek
 */

#include "vario.h"

SENSOR;

I2c i2c;
Timer meas_timer;

extern float temperature;
extern float raw_pressure;

extern Timer timer_buzzer_tone;
extern Timer timer_buzzer_delay;

void mark()
{
	GpioSetDirection(LEDR, OUTPUT);
	GpioSetDirection(LEDG, OUTPUT);
	LEDR_OFF
	LEDG_OFF

	while(1)
	{
		LEDR_OFF
		LEDG_ON
		_delay_ms(100);
		LEDR_ON
		LEDG_OFF
		_delay_ms(100);
	}
}

void init_low_power()
{
	PR.PRGEN = 0b10000011;

	PR.PRPA = 0b00000111;
	//stop port B DAC AC
	PR.PRPC = 0b00011100;

	//stop port D TWI USART1 USART0 SPI HIRES TC1 TC0
	PR.PRPD = 0b01011101;

	//NVM EE power reduction mode
	NVM.CTRLB |= 0b00000010;

	//disable digital buffer for bat adc
	PORTA.PIN2CTRL |= 0b00000111;
}

void total_low_power()
{
	PR.PRGEN = 0b10000111;

	//Disable all peripherals
	PR.PRPA = 0b00000111;
	PR.PRPC = 0b01011111;
	PR.PRPD = 0b01011111;

	//LDO SHDN
	GpioSetDirection(B_EN, OUTPUT);
	GpioWrite(B_EN, LOW);

	//Stop timers
	meas_timer.Stop();
	timer_buzzer_tone.Stop();
	timer_buzzer_delay.Stop();

	//other outputs
	GpioSetDirection(I2C_EN, INPUT);
	GpioSetDirection(BM_PIN, INPUT);

	//leds
	LEDR_OFF;
	LEDG_OFF;

	//NVM EE power reduction mode
	NVM.CTRLB |= 0b00000010;
}

void Setup()
{
	//For debug only
#ifdef FAST_CLOCK
	ClockSetSource(x32MHz);
#endif

	GpioSetDirection(LEDR, OUTPUT);
	GpioSetDirection(LEDG, OUTPUT);

	LoadEEPROM();

	init_low_power();

	EnableInterrupts();

	//port remap
	GpioRemap(&PORTD, gpio_remap_usart);
	GpioRemap(&PORTC, gpio_remap_timerA | gpio_remap_timerB | gpio_remap_timerC | gpio_remap_timerD);

	//configure uart & communicate with the
	pc_init();

	//enable i2c pull-ups
	GpioSetDirection(I2C_EN, OUTPUT);
	GpioWrite(I2C_EN, HIGH);

	//B_EN
	GpioSetDirection(B_EN, OUTPUT);
	GpioWrite(B_EN, HIGH);

	//wait to stabilize
	_delay_ms(200);

#ifdef SENSOR_TYPE_MS
	//PS -> high to enable i2c
	GpioSetDirection(porta6, OUTPUT);
	GpioWrite(porta6, HIGH);

	//CSB -> high to enable i2c addres to low
	GpioSetDirection(porta7, OUTPUT);
	GpioWrite(porta7, HIGH);
#endif

	i2c.InitMaster(i2cC, 100000ul, 8, 8);

	sys_tick_init();

	battery_init();

	bui_init();

	buzzer_init();

	filter_init();

	sensor.Init(&i2c, MS5611_ADDRESS_CSB_HI);

#ifdef ENABLE_DEBUG_UART
	sensor.CheckID();
#endif

#ifdef SLOW_CLOCK
	meas_timer.Init(timerD5, timer_div4); //at 2MHz
#endif

#ifdef FAST_CLOCK
	meas_timer.Init(timerD5, timer_div64); //at 32MHz
#endif

	meas_timer.SetTop(20 * 500); //50Hz
	meas_timer.SetCompare(timer_A, 500 * 5); //5ms - temp end

	meas_timer.EnableInterrupts(timer_overflow | timer_compareA);
	meas_timer.Start();

#ifdef ENABLE_DEBUG_TIMING
	GpioSetDirection(DEBUG_PIN, OUTPUT);
#endif
}


//first period
ISR(timerD5_overflow_interrupt)
{
	//Because F*ck you xmega32E5
	meas_timer.ClearOverflowFlag();

#ifdef ENABLE_DEBUG_TIMING
	GpioWrite(DEBUG_PIN, HI);
#endif

	sensor.ReadPressure();
	sensor.StartTemperature();

	sensor.CompensateTemperature();
	sensor.CompensatePressure();

#ifdef ENABLE_DEBUG_TIMING
	GpioWrite(DEBUG_PIN, LO);
#endif
}

//second period
ISR(timerD5_compareA_interrupt)
{
#ifdef ENABLE_DEBUG_TIMING
	GpioWrite(DEBUG_PIN, HI);
#endif


	sensor.ReadTemperature();
	sensor.StartPressure();

	filter_step();

	buzzer_step();


#ifdef ENABLE_DEBUG_UART
	pc_step();
#endif

	bui_step();

	battery_step();

#ifdef ENABLE_DEBUG_TIMING
	GpioWrite(DEBUG_PIN, LO);
#endif
}

extern uint8_t sleep_now;

int main()
{
	Setup();

#ifdef ENABLE_DEBUG_UART
	printf("\n\n***starting***\n\n");
	printf("RST.STATUS %02X\n", RST.STATUS);
	if (RST.STATUS & 0b00100000)
		printf("Software\n");
	if (RST.STATUS & 0b00010000)
		printf("PDI\n");
	if (RST.STATUS & 0b00001000)
		printf("WDT\n");
	if (RST.STATUS & 0b00000100)
		printf("BOR\n");
	if (RST.STATUS & 0b00000010)
		printf("External\n");
	if (RST.STATUS & 0b00000001)
		printf("Power On\n");
	RST.STATUS = 0b00111111;
#endif

	while(1)
	{
		//call sleep command outside the ISR!
		while (sleep_now)
		{

		#ifdef SENSOR_TYPE_MS
			//disable all outputs for MS sensor
			GpioSetDirection(porta6, INPUT);
			GpioSetDirection(porta7, INPUT);
		#endif

		#ifdef ENABLE_DEBUG_TIMING
			GpioSetDirection(DEBUG_PIN, INPUT);
		#endif

		#ifdef ENABLE_DEBUG_UART
			printf("\n\n***power down***\n\n");
			_delay_ms(5);
		#endif

			total_low_power();

			RtcDisableInterrupts(rtc_overflow);

			//set waking interrupt
			GpioSetInterrupt(BUTTON_PIN, gpio_interrupt, gpio_falling);

			//sleep
			SystemPowerDown();


		#ifdef ENABLE_DEBUG_UART
			//production test: for fast reprogram
			SystemReset();
		#endif


		#ifdef ENABLE_DEBUG_UART
			printf("\n\n***waked up***\n\n");
		#endif

			//waking up now
			//clear flags
			PORTC.INTFLAGS = 0xFF;
			//clear interrupt
			GpioSetInterrupt(BUTTON_PIN, gpio_clear);

			uint8_t wake_cnt = 0;
			for (uint8_t i = 0; i< 100; i++)
			{
				if (BUTTON_DOWN)
					wake_cnt++;
				else
					break;

				_delay_ms(10); //cca 1sec
			}

		#ifdef ENABLE_DEBUG_UART
			printf("? %d\n", wake_cnt);
		#endif

			//if the button was pressed 100 cycles. Restart
			//this is only way to start up the device
			if (wake_cnt == 100)
				SystemReset();
		}

		//nothing to do lets sleep a while
		SystemPowerIdle();
	}
}
