#ifndef USART_H_
#define USART_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "common.h"


//for baud 115200 @ 16MHz
//#define BSEL	983
//#define BSCALE	-7

//for baud 115200 @ 2MHz
#define BSEL	11
#define BSCALE	-7

#define USART_TX_BUFFER_SIZE 128
#define USART_RX_BUFFER_SIZE 128
//#define USART_RX_BUFFER_SIZE 4096

__volatile uint8_t * usart_rx_buffer;
__volatile uint8_t * usart_tx_buffer;

__volatile uint16_t usart_rx_index, usart_read_index, usart_rx_len;
__volatile uint8_t usart_tx_index, usart_write_index, usart_tx_len;

void usart_init();
void usart_deinit();
uint8_t usart_getchar();
void usart_putchar(uint8_t data);

#endif /* USART_H_ */
