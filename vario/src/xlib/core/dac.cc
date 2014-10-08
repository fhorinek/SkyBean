#include "dac.h"

extern uint32_t freq_cpu;

void DacInit()
{
	DacInit(dac_ch0 | dac_ch1 | dac_internal);
}

void DacInit(uint8_t enable)
{
	uint8_t ctrla = DAC_ENABLE_bm;
	uint8_t ctrlb = 0;
	uint8_t timctrl = 0;

	//1us for single
	uint32_t target = 1000000UL;
	uint8_t ch1bit = 0;
	uint8_t x = 1;
	uint8_t i;

	if (enable & dac_internal)
		ctrla |= DAC_IDOEN_bm;

	if (enable & dac_ch0)
	{
		ctrla |= DAC_CH0EN_bm;
		GpioWrite(dac0, OUTPUT);
	}
	if (enable & dac_ch1)
	{
		ctrla |= DAC_CH1EN_bm;
		ctrlb |= DAC_CHSEL1_bm;
		GpioWrite(dac1, OUTPUT);
		//1.5us for dual
		target = 666667UL;
		ch1bit = 1;

		for (i=11; i<=0; i--)
			//lower than  30 us for dual
			if ((uint32_t)freq_cpu / (uint16_t)(1 << (i+4)) >= 33334UL) break;

		timctrl |= 0x0F & i;
	}

	//cpu freq is too low to enable DAC
	if ((uint32_t)freq_cpu < (uint32_t)target) return;

	x = 1;

	for (i=1; i<=7; i++)
	{
		if ((uint32_t)((uint32_t)freq_cpu / (uint32_t)x) <= (uint32_t)target) break;
		x = (1 << i) | ch1bit * (1 << (i-1));
	}

	timctrl |= ((i-1) << 4);

	DACA.CTRLA = ctrla;
	DACA.CTRLB = ctrlb;
//	DACA.TIMCTRL = timctrl;
}

void DacSetReference(xlib_core_dac_reference ref)
{
	DACA.CTRLC |= ref;
}
