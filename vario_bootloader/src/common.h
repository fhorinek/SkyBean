#ifndef COMMON_H_
#define COMMON_H_

#include <avr/io.h>


#define VERSION		0x0001

#define USART_USART		USARTD0
#define USART_PORT		PORTD
#define WAIT_TIME		1000
#define USART_RXC		USARTD0_RXC_vect
#define USART_TXC		USARTD0_TXC_vect
#define UART_TX_PIN		7

#define LEDG_OFF		PORTA.OUTSET = 0b00000001;
#define LEDR_OFF		PORTA.OUTSET = 0b00000010;

#define LEDG_ON			PORTA.OUTCLR = 0b00000001;
#define LEDR_ON			PORTA.OUTCLR = 0b00000010;




void CCPIOWrite( volatile uint8_t * address, uint8_t value );

#endif /* COMMON_H_ */
