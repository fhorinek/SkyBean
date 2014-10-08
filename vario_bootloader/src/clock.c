#include "clock.h"
#include "common.h"

void clock_init()
{
	//enable ext xtal
	OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
	OSC.CTRL |= OSC_XOSCEN_bm;

	//wait until xtal is stable
	while (!(OSC.STATUS & OSC_XOSCRDY_bm));

	//  Switch to the ext as the clock source
	CCPIOWrite(&CLK.CTRL, CLK_SCLKSEL_XOSC_gc);
}

void osc_init()
{
	OSC.CTRL |= OSC_RC32MEN_bm;

	while (!(OSC.STATUS & OSC_RC32MRDY_bm));

	CCPIOWrite(&CLK.CTRL, CLK_SCLKSEL_RC32M_gc);
	CCPIOWrite(&CLK.PSCTRL, 0b00000100);
}

void clock_deinit()
{
	OSC.XOSCCTRL = 0;
	OSC.CTRL = OSC_RC2MEN_bm;

	//wait until xtal is stable
	while (!(OSC.STATUS & OSC_RC2MRDY_bm));

	CCPIOWrite(&CLK.CTRL, CLK_SCLKSEL_RC2M_gc);
	CCPIOWrite(&CLK.PSCTRL, 0);
}
