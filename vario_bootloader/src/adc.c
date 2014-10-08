#include "adc.h"


#define ADCACAL0_offset 0x20
#define ADCACAL1_offset 0x21

void adc_init()
{
	//load callibration
	ADCA.CALL = SP_ReadCalibrationByte(PROD_SIGNATURES_START + ADCACAL0_offset);
	ADCA.CALH = SP_ReadCalibrationByte(PROD_SIGNATURES_START + ADCACAL1_offset);

	//enable 0 1 2 3 adc ch, enable adc
	ADCA.CTRLA = ADC_ENABLE_bm;

	//set resolution 12bit & unsigned mode
	ADCA.CTRLB = ADC_RESOLUTION_12BIT_gc;

	//prepare ref for bandgap & temp
	ADCA.REFCTRL = ADC_BANDGAP_bm | ADC_TEMPREF_bm;

	//Set max freq
	ADCA.PRESCALER = 0b00000000;

	ADCA.REFCTRL = ADC_REFSEL_INT1V_gc | (ADCA.REFCTRL & 0b11001111);

	ADCA.CTRLB = ADC_CONMODE_bm | (ADCA.CTRLB & 0b11101111);

	ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc | (ADCA.CH0.CTRL & 0b11111100);
	ADCA.CH0.MUXCTRL = (2 << 3) | (ADCA.CH0.MUXCTRL & 0b10000111);
}

uint16_t adc_meas()
{
	ADCA.CH0.CTRL |= ADC_CH_START_bm;

	ADCA.CH0.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc;

	while ((ADCA.CH0.INTFLAGS & ADC_CH_IF_bm) == 0);

	ADCA.CH0.INTFLAGS |= ADC_CH_IF_bm;

	return ADCA.CH0.RES;
}

void adc_deinit()
{
	//revert to default state

	ADCA.CTRLA = 0;
	ADCA.CTRLB = 0;
	ADCA.REFCTRL = 0;
	ADCA.PRESCALER = 0;

	ADCA.CALL = 0;
	ADCA.CALH = 0;


	ADCA.CH0.CTRL = 0;
	ADCA.CH0.MUXCTRL = 0;
	ADCA.CH0.INTFLAGS |= ADC_CH_IF_bm;
}
