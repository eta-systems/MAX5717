/*******************************************************************************
 * @file        max5717.h
 * @brief       C Library for max5717/max5719 family of Digital Analog 
 *              Converters (DAC)
 * @details     This file implements the functionalities of the DAC.
 * @version     1.0
 * @author      Simon Burkhardt
 * @date        2020.05.22
 * @see         https://github.com/hellange/max5719/blob/master/src/main.cpp
 * @copyright   (c) 2020 eta systems GmbH
*******************************************************************************/

#include <stdint.h>
#include "max5717.h"

/**  MSB
  * [ 7  6  5  4  3  2  1  0]:[ 7  6  5  4  3  2  1  0]:[ 7  6  5  4  3  2  1  0]
  *  19 18 17 16 15 14 13 12   11 10 09 08 07 06 05 04   03 02 01 00  x  x  x  x
  */


/**
  * @brief  converts a voltage to a DAC code
  * @param  *hdac pointer to DAC handle
  * @param  colt floating point value of analog output voltage
  */
uint32_t MAX5717_VoltageToCode(MAX5717_t *hdac, float volt)
{
	volt = volt + (hdac->vref / 2.0f); // only for symmetrical output
	float fltCode = 0.0f;
	fltCode = volt * ((float)MAX571X_CODE_MAX) / hdac->vref;
	if(fltCode < 0.0f) fltCode = 0.0f;
	uint32_t code = (uint32_t)fltCode;
	// uint32_t code = (uint32_t)fltCode + (MAX571X_CODE_MAX/2);
	if(code > MAX571X_CODE_MAX) code = MAX571X_CODE_MAX;
	return code;

/*  // why??
	#ifndef MAX5719
	code = (code << 4) & 0x00FFFFF0;
	#endif
*/

	/*
	uint8_t payload[MAX5717_DATA_LENGTH];
	for(uint8_t i=0; (i<MAX5717_DATA_LENGTH) && (i<len); i++){
			payload[i] = 1;
	}
	*/
}

/**
  * @brief  initializes the DAC on the MCU side and sets CS/Latch pins to default state
  * @param  *hdac pointer to DAC handle
  * @param  *hspi pointer to HAL SPI interface handle
  * @param  vref floating point value of analog reference voltage
  * @see    Datasheet 1. 16-Bit Serial Interface Timing Diagram
  */
uint8_t MAX5717_Init(MAX5717_t *hdac, SPI_HandleTypeDef *hspi, float vref)
{
	hdac->vref = vref;
	hdac->hspix = hspi;
	
	// Datasheet 1. 16-Bit Serial Interface Timing Diagram
	HAL_GPIO_WritePin(hdac->csPort, hdac->csPin, GPIO_PIN_SET);        // !CS --> inverting
	HAL_GPIO_WritePin(hdac->latchPort, hdac->latchPin, GPIO_PIN_SET);  // !LATCH --> inverting
	return 0;
}


/**
  * @brief  sets a floating point voltage to the output of the DAC
  * @param  *hdac pointer to DAC handle
  * @param  volt floating point value of analog output voltage
  * @see    Datasheet 1. 16-Bit Serial Interface Timing Diagram
  */
uint8_t MAX5717_SetVoltage(MAX5717_t *hdac, float volt)
{
	uint32_t code = MAX5717_VoltageToCode(hdac, volt);
	MAX5717_SendCode(hdac, code);
	return 0;
}

/**
  * @brief  directly sends a binary code to the DAC
  * @param  *hdac pointer to DAC handle
  * @param  code the outputvalaue represented as code
  * @see    Datasheet 1. 16-Bit Serial Interface Timing Diagram
  */
uint8_t MAX5717_SendCode(MAX5717_t *hdac, uint32_t code)
{
	uint8_t payload[MAX571X_DATA_LENGTH];

#ifdef USE_MAX5719
	uint32_t data2 = code << 4;  // shift by 4, Table 2. 20-bit SPI DAC Register Table
	payload[0] = (uint8_t)((data2 >> 16) & 0xFF);
	payload[1] = (uint8_t)((data2 >>  8) & 0xFF);
	payload[2] = (uint8_t)((data2 >>  0) & 0xFF);
#else
	/** @Todo untested for MAX5717 */
	payload[0] = (uint8_t)((data2 >>  8) & 0xFF);
	payload[1] = (uint8_t)((data2 >>  0) & 0xFF);
#endif
	
	/** @note
	YES! CS must be toggled
	LDAC allows the DACD latch to update asynchronously, by pulling LDAC 
  low after CS goes high.  Hold LDAC high during the data loading sequence.
	*/
	HAL_GPIO_WritePin(hdac->csPort, hdac->csPin, GPIO_PIN_RESET); // chip select
	HAL_SPI_Transmit(hdac->hspix, payload, MAX571X_DATA_LENGTH, 10);  // 2 or 3 bytes__nop();
	HAL_GPIO_WritePin(hdac->csPort, hdac->csPin, GPIO_PIN_SET); // chip un-select

	/** @note 
	here needs to be a delay t_LDPW of at least 20ns 
	here: t_LDPW > 20ns
	f_cpu = 216 MHz --> t_clk = 4.6ns --> use 5 NOP commands to reach 20ns 
	*/

	HAL_GPIO_WritePin(hdac->latchPort, hdac->latchPin, GPIO_PIN_RESET); // latch low
	for(uint32_t i=0; i<128; i++);  // Delay
	HAL_GPIO_WritePin(hdac->latchPort, hdac->latchPin, GPIO_PIN_SET); // latch high
	
	return 0;
}
