#ifndef EVENT_H_
#define EVENT_H_

#include "../common.h"

#define event_source_none				0b00000000

//RTC
#define event_source_rtc_overflow		0b00001000
#define event_source_rtc_compare		0b00001001

//ACA
#define event_source_aca_ch0			0b00010000
#define event_source_aca_ch1			0b00010001
#define event_source_aca_window			0b00010010

//ACB
#define event_source_acb_ch0			0b00010011
#define event_source_acb_ch1			0b00010100
#define event_source_acb_window			0b00010101

//ADC
#define event_source_adc_pipea0			0b00100000
#define event_source_adc_pipea1			0b00100001
#define event_source_adc_pipea2			0b00100010
#define event_source_adc_pipea3			0b00100011
#define event_source_adc_pipeb0			0b00100100
#define event_source_adc_pipeb1			0b00100101
#define event_source_adc_pipeb2			0b00100110
#define event_source_adc_pipeb3			0b00100111

//PORTA 0-7
#define event_source_porta0				0b01010000
#define event_source_porta1				0b01010001
#define event_source_porta2				0b01010010
#define event_source_porta3				0b01010011
#define event_source_porta4				0b01010100
#define event_source_porta5				0b01010101
#define event_source_porta6				0b01010110
#define event_source_porta7				0b01010111

//PORTB 0-7
#define event_source_portb0				0b01011000
#define event_source_portb1				0b01011001
#define event_source_portb2				0b01011010
#define event_source_portb3				0b01011011
#define event_source_portb4				0b01011100
#define event_source_portb5				0b01011101
#define event_source_portb6				0b01011110
#define event_source_portb7				0b01011111

//PORTC 0-7
#define event_source_portc0				0b01100000
#define event_source_portc1				0b01100001
#define event_source_portc2				0b01100010
#define event_source_portc3				0b01100011
#define event_source_portc4				0b01100100
#define event_source_portc5				0b01100101
#define event_source_portc6				0b01100110
#define event_source_portc7				0b01100111

//LEDS
#define event_source_led0				event_source_pwm7
#define event_source_led1				event_source_pwm6
#define event_source_led2				event_source_pwm5
#define event_source_led3				event_source_pwm4

//USART0
#define event_source_usart0_rx_pin		0b11100010
#define event_source_usart0_tx_pin		0b11100011

//USART1
#define event_source_usart1_rx_pin		0b01110110
#define event_source_usart1_tx_pin		0b01110111

//USART2
#define event_source_usart2_rx_pin		0b01111010
#define event_source_usart2_tx_pin		0b01111011

//USART3 - PORTC
#define event_source_usart3_rx_pin		event_source_portc2
#define event_source_usart3_tx_pin		event_source_portc3

//USART4 - PORTC
#define event_source_usart4_rx_pin		event_source_portc6
#define event_source_usart4_tx_pin		event_source_portc7

//USART5 - SPI
#define event_source_usart5_rx_pin		0b01101110
#define event_source_usart5_tx_pin		0b01101111

//SPI
#define event_source_spi_demux_a_pin	0b01101000
#define event_source_spi_demux_b_pin	0b01110011
#define event_source_spi_demux_c_pin	0b01110100
#define event_source_spi_demux_oe_pin	0b01111000

#define event_source_spi_select_pin		0b01101100
#define event_source_spi_mosi_pin		0b01101101
#define event_source_spi_miso_pin		0b01101110
#define event_source_spi_sck_pin		0b01101111

//I2C
#define event_source_i2c_sda_pin		0b01110000
#define event_source_i2c_scl_pin		0b01110001

//MISC
#define event_source_wake_button		0b01101010
#define event_source_bt_reset			0b01111110
#define event_source_bt_shdn			0b01111111

//Freq cpu
#define event_source_freq_cpu_div_1		0b10000000
#define event_source_freq_cpu_div_2		0b10000001
#define event_source_freq_cpu_div_4		0b10000010
#define event_source_freq_cpu_div_8		0b10000011
#define event_source_freq_cpu_div_16	0b10000100
#define event_source_freq_cpu_div_32	0b10000101
#define event_source_freq_cpu_div_64	0b10000110
#define event_source_freq_cpu_div_128	0b10000111
#define event_source_freq_cpu_div_256	0b10001000
#define event_source_freq_cpu_div_512	0b10001001
#define event_source_freq_cpu_div_1024	0b10001010
#define event_source_freq_cpu_div_2048	0b10001011
#define event_source_freq_cpu_div_4096	0b10001100
#define event_source_freq_cpu_div_8192	0b10001101
#define event_source_freq_cpu_div_16384	0b10001110
#define event_source_freq_cpu_div_32768	0b10001111

//TIMER0
#define event_source_timer0_overflow	0b11000000
#define event_source_timer0_error		0b11000001
#define event_source_timer0_compareA	0b11000100
#define event_source_timer0_compareB	0b11000101
#define event_source_timer0_compareC	0b11000110
#define event_source_timer0_compareD	0b11000111

//TIMER1
#define event_source_timer1_overflow	0b11010000
#define event_source_timer1_error		0b11010001
#define event_source_timer1_compareA	0b11010100
#define event_source_timer1_compareB	0b11010101
#define event_source_timer1_compareC	0b11010110
#define event_source_timer1_compareD	0b11010111

//TIMER2
#define event_source_timer2_overflow	0b11100000
#define event_source_timer2_error		0b11100001
#define event_source_timer2_compareA	0b11100100
#define event_source_timer2_compareB	0b11100101
#define event_source_timer2_compareC	0b11100110
#define event_source_timer2_compareD	0b11100111

//TIMER3
#define event_source_timer3_overflow	0b11110000
#define event_source_timer3_error		0b11110001
#define event_source_timer3_compareA	0b11110100
#define event_source_timer3_compareB	0b11110101
#define event_source_timer3_compareC	0b11110110
#define event_source_timer3_compareD	0b11110111

//TIMER4
#define event_source_timer4_overflow	0b11001000
#define event_source_timer4_error		0b11001001
#define event_source_timer4_compareA	0b11001100
#define event_source_timer4_compareB	0b11001101

//TIMER5
#define event_source_timer5_overflow	0b11011000
#define event_source_timer5_error		0b11011001
#define event_source_timer5_compareA	0b11011100
#define event_source_timer5_compareB	0b11011101

//TIMER6
#define event_source_timer6_overflow	0b11101000
#define event_source_timer6_error		0b11101001
#define event_source_timer6_compareA	0b11101100
#define event_source_timer6_compareB	0b11101101


//Event channels
#define event_channel0	&EVSYS.CH0MUX, &EVSYS.CH0CTRL
#define event_channel1	&EVSYS.CH1MUX, &EVSYS.CH1CTRL
#define event_channel2	&EVSYS.CH2MUX, &EVSYS.CH2CTRL
#define event_channel3	&EVSYS.CH3MUX, &EVSYS.CH3CTRL
#define event_channel4	&EVSYS.CH4MUX, &EVSYS.CH4CTRL
#define event_channel5	&EVSYS.CH5MUX, &EVSYS.CH5CTRL
#define event_channel6	&EVSYS.CH6MUX, &EVSYS.CH6CTRL
#define event_channel7	&EVSYS.CH7MUX, &EVSYS.CH7CTRL

void EventSetSource(uint8_t event_channel, uint8_t event_source);


#endif /* EVENT_H_ */
