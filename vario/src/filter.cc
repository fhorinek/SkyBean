/*
 * sensor.cc
 *
 *  Created on: 7.11.2013
 *      Author: horinek
 */
#include "filter.h"

extern float temperature;
extern float altitude0;

extern float ram_sea_level_Pa;

extern float raw_pressure;
extern float pressure;
extern float climb;
extern float ram_climb_noise;

//use the 25-th sample as a reference
// 25 -> 0.5 s derivation
// 50 -> 1.0 s derivation
#define ALT_CNT	50
//#define LAST_ALT_CNT	50

float last_alt[ALT_CNT];
uint8_t last_alt_index = 0;

extern SENSOR;

float kalman_state_p;
float kalman_state_k;
float kalman_state_x;

float kalman_update(float measurement)
{
	//prediction update
	//omit x = x
	kalman_state_p += cfg.kalman_q;

	//measurement update
	kalman_state_k = kalman_state_p / (kalman_state_p + cfg.kalman_r);
	kalman_state_x += kalman_state_k * (measurement - kalman_state_x);
	kalman_state_p = (1 - kalman_state_k) * kalman_state_p;

	return kalman_state_x;
}


void filter_init()
{
	//kalman init
	kalman_state_p = cfg.kalman_p;
}

//float x1 = 0;
//float x2 = 0;
//float y1 = 0;
//float y2 = 0;
//
//// p = 0.2000 Hz
//#define B_0		0.000155148423476
//#define B_1		0.000310296846951
//#define B_2		0.000155148423476
//#define A_0		1.0
//#define A_1		-1.96446058021
//#define A_2		0.965081173899


//float butterw(float x)
//{
//	float y;
//
//	y = B_0 * x + B_1 * x1 + B_2 * x2 - A_1 * y1 - A_2 * y2;
//
//	x2 = x1;
//	x1 = x;
//
//	y2 = y1;
//	y1 = y;
//
//	return y;
//}

//float LPfast = 0;
//float LPslow = 0;
//float LPdiff = 0;
//
//float doubleLP(float x)
//{
//	LPfast += (x - LPfast) * 0.1;
//	LPslow += (x - LPslow) * 0.05;
//
//	LPdiff += ((LPslow - LPfast) - LPdiff) * 0.1;
//
//	printf("%0.3f; ", LPdiff);
//
//	return 0;
//}


void filter_step()
{
	//get raw pressure
	raw_pressure = sensor.pressure;

	//filter the pressure
	pressure = kalman_update(raw_pressure);
	//pressure = butterw(raw_pressure);

	//get temperature (we do not need here, it is for the serial output)
	temperature = sensor.temperature;

	//get altitude from pressure
	altitude0 = sensor.GetAltitude(ram_sea_level_Pa, pressure);

	//get the vertical speed
	// - this is difference between current altitude and altitude from the past
	climb = altitude0 - last_alt[(last_alt_index + cfg.int_interval) % cfg.int_interval];

	//Normalize climb in m/s
	// - sample rate need to be divided with derivation constant climb in  m/s
	// - e.g. if you have the difference between the current value and the
	//   value 0.5 seconds ago, it will be if meters per 0.5 second
	//   so you need to multiply it by 2 to get meter per second
	// - sensor reading frequency is 50Hz
	climb *= 50 / cfg.int_interval;

	//remove noise
	if (abs(climb) < abs(ram_climb_noise))
		climb = 0.0;

	//store this altitude to circular buffer
	last_alt[(last_alt_index + cfg.int_interval) % cfg.int_interval] = altitude0;
	//move circulat buffer index
	last_alt_index = (last_alt_index + 1) % cfg.int_interval;
}

