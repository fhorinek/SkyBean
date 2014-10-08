#include "timer.h"

extern uint32_t freq_cpu;

void Timer::Init(TC4_t * tc0, PORT_t * port, xlib_core_timer_prescaler pres)
{
	this->tc4 = tc0;
	this->tc5 = 0;

	this->Init(port, pres);
}

void Timer::Init(TC5_t * tc1, PORT_t * port, xlib_core_timer_prescaler pres)
{
	this->tc4 = 0;
	this->tc5 = tc1;

	this->Init(port, pres);
}


void Timer::Init(PORT_t * port, xlib_core_timer_prescaler pres)
{
	this->port = port;
	this->prescaler = pres;

	switch (pres)
	{
		case(timer_div1):
				this->pres = 1;
		break;
		case(timer_div2):
				this->pres = 2;
		break;
		case(timer_div4):
				this->pres = 4;
		break;
		case(timer_div8):
				this->pres = 8;
		break;
		case(timer_div64):
				this->pres = 64;
		break;
		case(timer_div256):
				this->pres = 256;
		break;
		case(timer_div1024):
				this->pres = 1024;
		break;
		default:
				this->pres = 0;
	}

	float f =  freq_cpu / this->pres;
	this->ms_per_fs = (float)(0xFFFF * 1000)/f;
	this->steps_per_ms = 1/(f*1000);

	this->SetInterruptPriority(LOW);
}

void Timer::SetPrescaler(xlib_core_timer_prescaler pres)
{
	this->prescaler = pres;

	if (this->tc4)
	{
		if (this->tc4->CTRLA & 0b00001111) //if running
			this->tc4->CTRLA = pres;
	}
	else
	{
		if (this->tc5->CTRLA & 0b00001111) //if running
			this->tc5->CTRLA = pres;
	}
}

void Timer::Start()
{
	if (this->tc4)
		this->tc4->CTRLA = 0b00001111 & this->prescaler;
	else
		this->tc5->CTRLA = 0b00001111 & this->prescaler;
}

void Timer::Stop()
{
	if (this->tc4)
		this->tc4->CTRLA = 0;
	else
		this->tc5->CTRLA = 0;
}

void Timer::SetMode(xlib_core_timer_mode mode)
{
	if (this->tc4)
		this->tc4->CTRLB = (this->tc4->CTRLB & 0b11111000) | mode;
	else
		this->tc5->CTRLB = (this->tc5->CTRLB & 0b11111000) | mode;
}

void Timer::EnableOutputs(uint8_t flags)
{
	//TCC4.CTRLE = 0b01010000;

	if (this->tc4)
	{
		this->tc4->CTRLE |= flags;
		//do this manualy bacause of possible remap
//		if (flags & timer_A) GpioSetDirection(this->port, 0, OUTPUT);
//		if (flags & timer_B) GpioSetDirection(this->port, 1, OUTPUT);
//		if (flags & timer_C) GpioSetDirection(this->port, 2, OUTPUT);
//		if (flags & timer_D) GpioSetDirection(this->port, 3, OUTPUT);
	}
	else
	{
		this->tc5->CTRLE |= flags;
		//do this manualy bacause of possible remap
//		if (flags & timer_A) GpioSetDirection(this->port, 4, OUTPUT);
//		if (flags & timer_B) GpioSetDirection(this->port, 5, OUTPUT);
	}
}

void Timer::DisableOutputs(uint8_t flags)
{
	if (this->tc4)
	{
		this->tc4->CTRLE &= ~flags;
		//do this manualy bacause of possible remap
//		if (flags & timer_A) GpioSetDirection(this->port, 0, INPUT);
//		if (flags & timer_B) GpioSetDirection(this->port, 1, INPUT);
//		if (flags & timer_C) GpioSetDirection(this->port, 2, INPUT);
//		if (flags & timer_D) GpioSetDirection(this->port, 3, INPUT);
	}
	else
	{
		this->tc5->CTRLE &= ~flags;
		//do this manualy bacause of possible remap
//		if (flags & timer_A) GpioSetDirection(this->port, 4, INPUT);
//		if (flags & timer_B) GpioSetDirection(this->port, 5, INPUT);
	}
}

void Timer::SetTop(uint16_t top)
{
	if (this->tc4)
		this->tc4->PER = top;
	else
		this->tc5->PER = top;
}

void Timer::UpdateTop(uint16_t top)
{
	if (this->tc4)
		this->tc4->PERBUF = top;
	else
		this->tc5->PERBUF = top;
}

void Timer::SetCompare(uint8_t flags, uint16_t compare)
{
	if (this->tc4)
	{
		if (flags & timer_A)
			this->tc4->CCABUF = compare;
		if (flags & timer_B)
			this->tc4->CCBBUF = compare;
		if (flags & timer_C)
			this->tc4->CCCBUF = compare;
		if (flags & timer_D)
			this->tc4->CCDBUF = compare;
	}
	else
	{
		if (flags & timer_A)
			this->tc5->CCABUF = compare;
		if (flags & timer_B)
			this->tc5->CCBBUF = compare;
	}

}

void Timer::EnableInterrupts(uint8_t inter)
{
	register8_t * regA;
	register8_t * regB;

	if (this->tc4)
	{
		regA = &this->tc4->INTCTRLA;
		regB = &this->tc4->INTCTRLB;
	}
	else
	{
		regA = &this->tc5->INTCTRLA;
		regB = &this->tc5->INTCTRLB;
	}

	if (inter & timer_compareA)
		(*regB) |= this->irq_priority << 0;
	if (inter & timer_compareB)
		(*regB) |= this->irq_priority << 2;
	if (inter & timer_compareC)
		(*regB) |= this->irq_priority << 4;
	if (inter & timer_compareD)
		(*regB) |= this->irq_priority << 6;

	if (inter & timer_overflow)
		(*regA) |= this->irq_priority << 0;
	if (inter & timer_error)
		(*regA) |= this->irq_priority << 2;
}

void Timer::ClearOverflowFlag()
{
	if (this->tc4)
		this->tc4->INTFLAGS = TC4_OVFIF_bm;
	else
		this->tc5->INTFLAGS = TC5_OVFIF_bm;
}

void Timer::DisableInterrupts(uint8_t inter)
{
	register8_t * regA;
	register8_t * regB;

	if (this->tc4)
	{
		regA = &this->tc4->INTCTRLA;
		regB = &this->tc4->INTCTRLB;
	}
	else
	{
		regA = &this->tc5->INTCTRLA;
		regB = &this->tc5->INTCTRLB;
	}

	if (inter & timer_compareA)
		(*regB) &= 0b11111100;
	if (inter & timer_compareB)
		(*regB) &= 0b11110011;
	if (inter & timer_compareC)
		(*regB) &= 0b11001111;
	if (inter & timer_compareD)
		(*regB) &= 0b00111111;

	if (inter & timer_overflow)
		(*regA) &= 0b11111100;
	if (inter & timer_error)
		(*regA) &= 0b11110011;
}

uint16_t Timer::GetValue()
{
	if (this->tc4)
		return tc4->CNT;
	else
		return tc5->CNT;
}

void Timer::SetValue(uint16_t value)
{
	if (this->tc4)
		tc4->CNT = value;
	else
		tc5->CNT = value;
}

void Timer::SetInterruptPriority(uint8_t p)
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
}
