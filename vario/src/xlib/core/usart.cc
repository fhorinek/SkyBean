#include "usart.h"

//pointers to handle IRQ
Usart * usarts[2];

//CreateStdIn(usart_default_in, usarts[0]->Read);
//CreateStdOut(usart_default_out, usarts[0]->Write);


//from clock.cc
extern uint32_t freq_cpu;

float xlib_core_usart_pow(int8_t exp)
{
    float res = 1;

    if (exp > 0)
        while (exp > 0)
        {
            exp--;
            res = res * 2;
        }
    if (exp < 0)
        while (exp < 0)
        {
            exp++;
            res = res / 2;
        }
    return res;
}

/**
 * Register event fore this module
 *
 * \param event event to trigger callback
 * \param cb callback function
 *
 * \note clear callback using NULL cb parameter
 */
void Usart::RegisterEvent(xlib_core_usart_events event, usart_event_cb_t cb)
{
	this->events[event] = cb;
}

Usart::Usart()
{}

Usart::~Usart()
{}

/**
 * Initialize usart (Quick init)
 *
 * init usart on usb port at baud 9600
 *
 */
void Usart::Init()
{
	this->Init(usartC0, 9600ul);

//	SetStdIO(usart_default_in, usart_default_out);
}

/**
 * Initialize usart (Quick init)
 *
 * \param baud Baudrate
 *
 */
void Usart::Init(uint32_t baud)
{
	this->Init(usartC0, baud);

//	SetStdIO(usart_default_in, usart_default_out);
}


/**
 * Initialize usart
 *
 * \param usart Usart structure
 * \param port Port structure
 * \param tx Number of tx pin
 * \param n Object index for ISR handling
 * \param baud Baudrate
 *
 * \note There are predefined usart modules like usart0, usart1 which contains usart and port structure, tx pin number and module index
 */
void Usart::Init(USART_t * usart, PORT_t * port, uint8_t tx, uint8_t n, uint32_t baud)
{
	this->Init(usart, port, tx, n, baud, BUFFER_SIZE, BUFFER_SIZE);
}

/**
 * Initialize usart
 *
 * \param usart Usart structure
 * \param port Port structure
 * \param tx Number of tx pin
 * \param n Object index for ISR handling
 * \param baud Baudrate
 * \param buffer_size size of Rx/Tx buffer
 *
 * \note There are predefined usart modules like usart0, usart1 which contains usart and port structure, tx pin number and module index
 */
void Usart::Init(USART_t * usart, PORT_t * port, uint8_t tx, uint8_t n, uint32_t baud, XLIB_USE_CORE_USART_INIT_VAR buffer_size)
{
	this->Init(usart, port, tx, n, baud, buffer_size, buffer_size);
}

/**
 * Initialize usart
 *
 * \param usart Usart structure
 * \param port Port structure
 * \param tx Number of tx pin
 * \param n Object index for ISR handling
 * \param baud Baudrate
 * \param rx_buffer_size Size of Rx buffer
 * \param tx_buffer_size Size of Tx buffer
 *
 * \note There are predefined usart modules like usart0, usart1 which contains usart and port structure, tx pin number and module index
 */
void Usart::Init(USART_t * usart, PORT_t * port, uint8_t tx, uint8_t n, uint32_t baud, XLIB_USE_CORE_USART_INIT_VAR rx_buffer_size, XLIB_USE_CORE_USART_INIT_VAR tx_buffer_size)
{
	//TODO:why is 1M not working?
	uint32_t new_baud;

	uint16_t bsel; //12 bit
	int8_t bscale; //-7 - 7

	uint16_t bsel_best = 0;
	int8_t bscale_best = 0;
	float error = 1;

	uint8_t mul = 16;

	uint8_t i;

	//reset events
	for (i=0;i<xlib_core_usart_events_count;i++)
		this->events[i] = NULL;

	if (baud > freq_cpu/16)
		mul = 8;

	for (bscale = 7; bscale>=-7; bscale--)
	{
		if (bscale >= 0)
		{
			bsel = (uint16_t)((float)freq_cpu / (xlib_core_usart_pow(bscale)*mul*(float)baud) - 1);
			new_baud = (uint32_t)((uint32_t)freq_cpu/(xlib_core_usart_pow(bscale)*mul*((uint16_t)bsel + 1)));
		}
		else
		{
			bsel = (uint16_t)((float)(1 / xlib_core_usart_pow(bscale)) * (float)(freq_cpu / (mul*(float)baud) - 1));
			new_baud = (uint32_t)((uint32_t)freq_cpu/(mul*(xlib_core_usart_pow(bscale)*((uint16_t)bsel) + 1)));
		}

		if (error > abs(1 - (float)((float)new_baud / (float)baud)))
		{
			bsel_best = bsel;
			bscale_best = bscale;
			error = abs(1 - (float)((float)new_baud / (float)baud));
		}
	}

//	GpioSetDirection(port, tx, OUTPUT);
//	GpioWrite(port, tx, HIGH);

	this->usart = usart;

	//set baudrate
	this->usart->BAUDCTRLA = bsel_best;
	this->usart->BAUDCTRLB = (bscale_best << 4) | (0x0F & (bsel_best >> 8));

	//reset CTRL
	this->usart->CTRLB = 0;
	this->usart->CTRLA = 0;

	//set doublespeed operation
	if (mul == 8)
		this->usart->CTRLB |= USART_CLK2X_bm;

	//init buffers
	if ((this->rx_buffer_size = rx_buffer_size))
		//this->rx_buffer = (uint8_t *) malloc(this->rx_buffer_size * sizeof(uint8_t));
		this->rx_buffer = new uint8_t[this->rx_buffer_size];
	if ((this->tx_buffer_size = tx_buffer_size))
		//this->tx_buffer = (uint8_t *) malloc(this->tx_buffer_size * sizeof(uint8_t));
		this->tx_buffer = new uint8_t[this->tx_buffer_size];

	//reset counters
	this->write_index = 0;
	this->read_index = 0;
	this->rx_index = 0;
	this->rx_len = 0;
	this->tx_index = 0;
	this->tx_len = 0;

	//set format
	this->usart->CTRLC = USART_CHSIZE_8BIT_gc;

	//enable RX & int
	if (this->rx_buffer_size)
	{
		this->usart->CTRLB |= USART_RXEN_bm;
	}
	//enable RX & int
	if (this->tx_buffer_size)
	{
		this->usart->CTRLB |= USART_TXEN_bm;
	}

	//disable RTS CTS
	this->cts_port = NULL;
	this->rts_port = NULL;

	this->irq_priority = LOW;
	this->SetInterruptPriority(LOW);

	usarts[n] = this;
}

/**
 * Send all data remaning in tx buffer, stop usart module, disable Rx/Tx, free buffers
 */
void Usart::Stop()
{
	//wait to send all data
	this->FlushTxBuffer();

	//clear int and RX TX(data->rx_index)++
	this->usart->CTRLB = 0;
	this->usart->CTRLA = 0;

	//free buffers
	if (this->rx_buffer_size)
		delete[] this->rx_buffer;
	if (this->tx_buffer_size)
		delete[] this->tx_buffer;
}

/**
 * Standard RX complete interrupt handler
 */
void Usart::RxComplete()
{
	if ((this->usart->STATUS & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm )) == 0)
	{
		this->rx_buffer[(this->rx_index)++] = this->usart->DATA;
		if (this->rx_index == this->rx_buffer_size)
			this->rx_index = 0;
		if (++(this->rx_len) == this->rx_buffer_size)
			this->rx_len = 0;

		//handle CTS
		if (this->cts_port)
		{
			//buffer pass threshold deactivate CTS
			if (this->rx_len >= this->cts_threshold)
				GpioWrite(this->cts_port, this->cts_pin, !this->cts_active);
		}
	}

	if (this->events[usart_event_rxcomplete] != NULL)
		this->events[usart_event_rxcomplete](this);
}

/**
 * Standard TX complete interrupt handler
 */
void Usart::TxComplete()
{
	if (this->rts_port)
	{
		if (GpioRead(this->rts_port, this->rts_pin) == !this->rts_active)
			return;
	}

	if (this->tx_len)
	{
		(this->tx_len)--;
		this->usart->DATA = this->tx_buffer[(this->tx_index)++];
		if (this->tx_index == this->tx_buffer_size)
			this->tx_index = 0;
	}

	if (this->events[usart_event_txcomplete] != NULL)
		this->events[usart_event_txcomplete](this);
}

/**
 * Test if TX buffer is empty (all data in TX buffer transmitted)
 *
 * \return true if TX buffer is empty
 */
bool Usart::isTxBufferEmpty()
{
	return (this->tx_len == 0);
}

/**
 * Test if RX buffer is empty (all received data read from RX buffer)
 *
 * \return true if RX buffer is empty
 */
bool Usart::isRxBufferEmpty()
{
	return (this->rx_len == 0);
}

/**
 * Block until all data in TX buffer is transmitted
 */
void Usart::FlushTxBuffer()
{
	//wait to send all data
	while (this->tx_len);// || !this->usart->STATUS & USART_TXCIF_bm);
}

/**
 * Set interrupt priority for this usart module
 *
 * \param p interrupt priority
 */
void Usart::SetInterruptPriority(uint8_t p)
{
	switch (p)
	{
	case(HIGH):
		this->irq_priority = 0b11;
	break;
	case(MEDIUM):
		this->irq_priority = 0b10;
	break;
	case(LOW):
		this->irq_priority = 0b01;
	break;
	}

	this->usart->CTRLA &= 0b11000011;

	if (this->rx_buffer_size)
		this->usart->CTRLA |= this->irq_priority << 4;

	if (this->tx_buffer_size)
		this->usart->CTRLA |= this->irq_priority << 2;
}

/**
 * Write character to TX buffer
 *
 * \param c character to write
 */
void Usart::Write(uint8_t c)
{
	while (this->tx_len == this->tx_buffer_size);

	if (this->tx_len || !(this->usart->STATUS & USART_DREIF_bm))
	{
		cli();
		this->tx_len++;
		this->tx_buffer[(this->write_index)++] = c;
		sei();
		if (this->write_index == this->tx_buffer_size)
			this->write_index = 0;
	}
	else
		this->usart->DATA = c;
}

/**
 * Read character from RX buffer
 *
 * \return character form RX buffer
 * \note this function will block if there is no character in RX buffer (use isRxBufferEmpty or Peek for non blocking behavior)
 */
uint8_t Usart::Read()
{
	uint8_t c;

	while (this->rx_len == 0);

	c = this->rx_buffer[(this->read_index)++];
	if (this->read_index == this->rx_buffer_size)
		this->read_index = 0;

	cli();
	(this->rx_len)--;
	sei();

	//Buffer freed -> CTS active
	if (this->cts_port != NULL)
	{
		if (this->rx_len < this->cts_threshold)
			GpioWrite(this->cts_port, this->cts_pin, this->cts_active);
	}

	return c;
}

/**
 * Return first byte from RX buffer but do not increse read index
 *
 * \return first byte from RX buffer
 */
uint8_t Usart::Peek()
{
	// Edit Tomas 26.7 - commented out next line and added new one
	while (this->rx_len == 0);

	return this->rx_buffer[this->read_index];
}


/**
 * Set CTS pin for this usart module
 *
 * \param port pin port
 * \param pin pin number
 * \param active level of active CTS signal
 * \param threshold if count of bytes in RX buffer is higher than threshold CTS pin will be set to non active level
 */
void Usart::SetCtsPin(PORT_t * port, uint8_t pin, uint8_t active, XLIB_USE_CORE_USART_INIT_VAR threshold)
{
	this->cts_port = port;
	this->cts_pin = pin;
	this->cts_active = active;
	this->cts_threshold = threshold;

	GpioSetDirection(this->cts_port, this->cts_pin, OUTPUT);

	if (this->rx_len >= this->cts_threshold)
		GpioWrite(this->cts_port, this->cts_pin, !this->cts_active);
	else
		GpioWrite(this->cts_port, this->cts_pin, this->cts_active);
}

/**
 * TODO: not implemented
 */
void Usart::SetRtsPin(PORT_t * port, uint8_t pin, uint8_t active)
{
	this->rts_port = port;
	this->rts_pin = pin;
	this->rts_active = active;
}


//static IRQ handlers
void UsartRxInteruptHandler(uint8_t n)
{
	usarts[n]->RxComplete();
}

void UsartTxInteruptHandler(uint8_t n)
{
	usarts[n]->TxComplete();
}

//usart3
ISR(USARTC0_RXC_vect){UsartRxInteruptHandler(0);}
ISR(USARTC0_TXC_vect){UsartTxInteruptHandler(0);}
//usart6
ISR(USARTD0_RXC_vect){UsartRxInteruptHandler(1);}
ISR(USARTD0_TXC_vect){UsartTxInteruptHandler(1);}
