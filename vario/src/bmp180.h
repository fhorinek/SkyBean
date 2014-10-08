/*
 * BMP180.H
 *
 *  Created on: 29.11.2013
 *      Author: horinek
 */

#ifndef BMP180_H_
#define BMP180_H_

#include "xlib/core/i2c.h"
#include "xlib/core/usart.h"
#include "common.h"

#define BMP180_ADDRESS				0x77
#define BMP180_ADDRESS_CSB_LO		0b1110111

#define BMP180_ACCEPT_TEMP_DELTA	1000


// verify req
#define BMP180_CHIP_ID_REG		0xD0
#define BMP180_CHIP_ID			0x55

// registers of the device
#define BMP180_CONTROL			0xF4
#define BMP180_RESET			0xE0
#define BMP180_READ				0xF6
#define BMP180_PRESSURE_MEAS	0x34
#define BMP180_TEMPERATURE_MEAS	0x2E

// OSR (Over Sampling Ratio) constants
#define BMP180_OSR_ULP 			0x00
#define BMP180_OSR_STD 			0x01
#define BMP180_OSR_HIGH			0x02
#define BMP180_OSR_ULTRA		0x03

#define BMP180_PROM 			0xAA


class BMP180
{
public:
	int16_t Calibration_AC1;
	int16_t Calibration_AC2;
	int16_t Calibration_AC3;
	uint16_t Calibration_AC4;
	uint16_t Calibration_AC5;
	uint16_t Calibration_AC6;
	int16_t Calibration_B1;
	int16_t Calibration_B2;
	int16_t Calibration_MB;
	int16_t Calibration_MC;
	int16_t Calibration_MD;

	uint8_t press_osr;

	int16_t param_b5;

	I2c * i2c;

	float temperature;
	float pressure;

	uint32_t raw_temperature;
	uint32_t raw_pressure;

	void Init(I2c * i2c, uint8_t address);
	void ReadPROM();

	uint8_t CheckID();

	void Write(uint8_t adr, uint8_t cmd);
	uint16_t Read16(uint8_t cmd);
	uint32_t Read24(uint8_t cmd);

	void StartPressure();
	void StartTemperature();

	void ReadPressure();
	void ReadTemperature();

	void CompensatePressure();
	void CompensateTemperature();

	float GetAltitude(float currentSeaLevelPressureInPa, float pressure);
};



#endif /* BMP180_H_ */
