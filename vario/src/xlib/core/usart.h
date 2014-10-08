#ifndef USART_H_
#define USART_H_

#include "../common.h"
#include "../stdio.h"
#include "gpio.h"

//		name	usart,    port tx	 n
#define usartC0	&USARTC0, portc3, 0
#define usartD0	&USARTD0, portd3, 1



#ifndef XLIB_USE_CORE_USART_LARGEBUFFER
	#define XLIB_USE_CORE_USART_INIT_VAR uint8_t
#else
	#define XLIB_USE_CORE_USART_INIT_VAR uint16_t
#endif

typedef enum xlib_core_usart_events_e //! usart events
{
	usart_event_rxcomplete, //!< execute after RX complete routine
	usart_event_txcomplete	//!< execute after TX complete routine
} xlib_core_usart_events;

#define xlib_core_usart_events_count	2

class Usart;

typedef void (*usart_event_cb_t)(Usart *);

class Usart //! Object for communication using usart modules
{
private:
	USART_t * usart;

	volatile uint8_t * rx_buffer;
	volatile uint8_t * tx_buffer;

	PORT_t * cts_port;
	uint8_t cts_pin;
	uint8_t cts_active;
	XLIB_USE_CORE_USART_INIT_VAR cts_threshold;

	PORT_t * rts_port;
	uint8_t rts_pin;
	uint8_t rts_active;

	uint8_t irq_priority;

	usart_event_cb_t events[xlib_core_usart_events_count];

#ifndef XLIB_USE_CORE_USART_LARGEBUFFER
	volatile uint8_t rx_index, read_index, rx_len, rx_buffer_size;
	volatile uint8_t tx_index, write_index, tx_len, tx_buffer_size;
#else
	volatile uint16_t rx_index, read_index, rx_len, rx_buffer_size;
	volatile uint16_t tx_index, write_index, tx_len, tx_buffer_size;
#endif

public:

	void Init();
	void Init(uint32_t baud);
	void Init(USART_t * usart, PORT_t * port, uint8_t tx, uint8_t n, uint32_t baud);
	void Init(USART_t * usart, PORT_t * port, uint8_t tx, uint8_t n, uint32_t baud, XLIB_USE_CORE_USART_INIT_VAR rx_buffer_size);
	void Init(USART_t * usart, PORT_t * port, uint8_t tx, uint8_t n, uint32_t baud, XLIB_USE_CORE_USART_INIT_VAR rx_buffer_size, XLIB_USE_CORE_USART_INIT_VAR tx_buffer_size);
	void Stop();

	void RxComplete();
	void TxComplete();

	bool isTxBufferEmpty();
	bool isRxBufferEmpty();

	uint8_t Read();
	uint8_t Peek();
	void Write(uint8_t c);

	void FlushTxBuffer();
	void SetInterruptPriority(uint8_t p);

	void SetCtsPin(PORT_t * port, uint8_t pin, uint8_t active, XLIB_USE_CORE_USART_INIT_VAR threshold);
	void SetRtsPin(PORT_t * port, uint8_t pin, uint8_t active);

	void RegisterEvent(xlib_core_usart_events, usart_event_cb_t cb);

	Usart();
	~Usart();

};

#endif /* USART_H_ */
