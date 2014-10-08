#include "usart.h"

void usart_deinit()
{
	USART_USART.BAUDCTRLA = 0;
	USART_USART.BAUDCTRLB = 0;
	USART_USART.CTRLC = 0b00000110;
	USART_USART.CTRLB = 0;
	USART_USART.CTRLA = 0;

	USART_PORT.OUT = 0;
	USART_PORT.DIR = 0;

	USART_PORT.REMAP = 0;

	free(usart_rx_buffer);
	free(usart_tx_buffer);
}

void usart_init()
{
	//set port mux
	USART_PORT.REMAP |= 0b00010000;

	//set port dir
	USART_PORT.OUT |= (1 << UART_TX_PIN);
	USART_PORT.DIR |= (1 << UART_TX_PIN);

	//set baudrate
	USART_USART.BAUDCTRLA = BSEL;
	USART_USART.BAUDCTRLB = (BSCALE << 4) | (0x0F & (BSEL >> 8));

	usart_write_index = 0;
	usart_read_index = 0;

	usart_rx_index = 0;
	usart_rx_len = 0;
	usart_tx_index = 0;
	usart_tx_len = 0;

	//create buffer
	usart_rx_buffer = (uint8_t *) malloc(sizeof(uint8_t) * USART_RX_BUFFER_SIZE);
	usart_tx_buffer = (uint8_t *) malloc(sizeof(uint8_t) * USART_TX_BUFFER_SIZE);

	//set format
	USART_USART.CTRLC = USART_CHSIZE_8BIT_gc;

	//enable RX TX
	USART_USART.CTRLB = USART_RXEN_bm | USART_TXEN_bm;

	//enable interupts
	USART_USART.CTRLA = USART_RXCINTLVL_LO_gc | USART_TXCINTLVL_LO_gc;

}

//RX complete
ISR(USART_RXC)
{
	if ((USART_USART.STATUS & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm )) == 0)
	{
		usart_rx_buffer[usart_rx_index++] = USART_USART.DATA;
		if (usart_rx_index == USART_RX_BUFFER_SIZE)
			usart_rx_index = 0;
		if (++usart_rx_len == USART_RX_BUFFER_SIZE)
		{
			usart_rx_len = 0;
			//usart_rx_buffer_ovf = 1;
		}
	}
}

//TX complete
ISR(USART_TXC)
{
	if (usart_tx_len)
	{
		usart_tx_len--;
		USART_USART.DATA = usart_tx_buffer[usart_tx_index++];
		if (usart_tx_index == USART_TX_BUFFER_SIZE)
			usart_tx_index = 0;
	}
}

void usart_putchar(uint8_t data)
{
	//wait until buffer have free space
	while(usart_tx_len == USART_TX_BUFFER_SIZE);
	if (usart_tx_len || !(USART_USART.STATUS & USART_DREIF_bm))
	{
		cli();
		usart_tx_len++;
		usart_tx_buffer[usart_write_index++] = data;
		sei();
		if (usart_write_index == USART_TX_BUFFER_SIZE)
			usart_write_index = 0;
	}
	else
		USART_USART.DATA = data;

}

uint8_t usart_getchar()
{
	char u_data;

	while (usart_rx_len == 0);
	u_data = usart_rx_buffer[usart_read_index++];
	if (usart_read_index == USART_RX_BUFFER_SIZE)
		usart_read_index = 0;
	cli();
	usart_rx_len--;
	sei();

	return u_data;
}

