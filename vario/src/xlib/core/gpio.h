#ifndef GPIO_H_
#define GPIO_H_

#include "../common.h"

//PORTA
#define porta0	&PORTA, 0
#define porta1	&PORTA, 1
#define porta2	&PORTA, 2
#define porta3	&PORTA, 3
#define porta4	&PORTA, 4
#define porta5	&PORTA, 5
#define porta6	&PORTA, 6
#define porta7	&PORTA, 7

//DAC
#define dac0	porta2
#define dac1	porta3

//PORTC 0 - 7
#define portc0	&PORTC, 0
#define portc1	&PORTC, 1
#define portc2	&PORTC, 2
#define portc3	&PORTC, 3
#define portc4	&PORTC, 4
#define portc5	&PORTC, 5
#define portc6	&PORTC, 6
#define portc7	&PORTC, 7

//PORTD 0 - 7
#define portd0	&PORTD, 0
#define portd1	&PORTD, 1
#define portd2	&PORTD, 2
#define portd3	&PORTD, 3
#define portd4	&PORTD, 4
#define portd5	&PORTD, 5
#define portd6	&PORTD, 6
#define portd7	&PORTD, 7

//USARTC0
#define usartC0_rx	&PORTC, 2
#define usartC0_tx	&PORTC, 3

//USARTC1
#define usartC1_rx	&PORTC, 6
#define usartC1_tx	&PORTC, 7

//USARTD0
#define usartD0_rx	&PORTD, 2
#define usartD0_tx	&PORTD, 3

//USARTD1
#define usartD1_rx	&PORTD, 6
#define usartD1_tx	&PORTD, 7

//ISR
#define porta_interrupt		PORTA_INT_vect

#define portc_interrupt		PORTC_INT_vect

#define portd_interrupt		PORTD_INT_vect

#define porte_interrupt		PORTE_INT_vect


//pullup configuration
typedef enum xlib_core_gpio_pull_e
{
	gpio_totem = PORT_OPC_TOTEM_gc,
	gpio_buskeeper = PORT_OPC_BUSKEEPER_gc,
	gpio_pull_down = PORT_OPC_PULLDOWN_gc,
	gpio_pull_up = PORT_OPC_PULLUP_gc,
	gpio_wired_or = PORT_OPC_WIREDOR_gc,
	gpio_wired_and = PORT_OPC_WIREDAND_gc,
	gpio_wired_or_pull = PORT_OPC_WIREDORPULL_gc,
	gpio_wired_and_pull = PORT_OPC_WIREDANDPULL_gc
} xlib_core_gpio_pull;

//interrupts
typedef enum xlib_core_gpio_int_e
{
	gpio_bothedges = PORT_ISC_BOTHEDGES_gc,
	gpio_rising = PORT_ISC_RISING_gc,
	gpio_falling = PORT_ISC_FALLING_gc,
	gpio_level = PORT_ISC_LEVEL_gc,
	gpio_input_disable = PORT_ISC_INPUT_DISABLE_gc
} xlib_core_gpio_int;

typedef enum xlib_core_gpio_intmask_e
{
	gpio_clear,
	gpio_interrupt
} xlib_core_gpio_intmask;

typedef enum xlib_core_gpio_remap_e
{
	gpio_remap_usart  = 0b00010000,
	gpio_remap_timerA = 0b00001000,
	gpio_remap_timerB = 0b00000100,
	gpio_remap_timerC = 0b00000010,
	gpio_remap_timerD = 0b00000001
} xlib_core_gpio_rempa;


void GpioWrite(PORT_t * port, uint8_t pin, uint8_t set);
uint8_t GpioRead(PORT_t * port, uint8_t pin);
void GpioSetPull(PORT_t * port, uint8_t pin, xlib_core_gpio_pull pull);
void GpioSetInvert(PORT_t * port, uint8_t pin, uint8_t inv);
void GpioSetDirection(PORT_t * port, uint8_t pin, uint8_t dir);
void GpioSetInterrupt(PORT_t * port, uint8_t pin, xlib_core_gpio_intmask mask, xlib_core_gpio_int interrupt);
void GpioSetInterrupt(PORT_t * port, uint8_t pin, xlib_core_gpio_intmask mask);

void GpioRemap(PORT_t * port, uint8_t flags);

#endif /* GPIO_H_ */
