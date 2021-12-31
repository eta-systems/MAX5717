/*******************************************************************************
 * @file        max5717.h
 * @brief       C Library for max5717/max5719 family of Digital Analog 
 *              Converters (DAC)
 * @details     This file implements the functionalities of the DAC.
 * @version     1.0
 * @author      Simon Burkhardt
 * @date        2020.05.22
 * @copyright   (c) 2020 eta systems GmbH
*******************************************************************************/

#ifndef MAX5717_H
#define MAX5717_H

#ifdef  __cplusplus
extern "C" {
#endif

#define STM32F7

#if defined(STM32F1)
    #include "stm32f1xx_hal.h"
    #include "stm32f1xx_hal_spi.h"
#elif defined(STM32F4)
    #include "stm32f4xx_hal.h"
    #include "stm32f4xx_hal_spi.h"
#elif defined(STM32L4)
    #include "stm32l4xx_hal.h"
    #include "stm32l4xx_hal_spi.h"
#elif defined(STM32F3)
    #include "stm32f3xx_hal.h"
    #include "stm32f3xx_hal_spi.h"
#elif defined(STM32F7)
    #include "stm32f7xx_hal.h"
    #include "stm32f7xx_hal_spi.h"
#endif

// #define USE_MAX5717 // 16 bit interface
#define USE_MAX5719 // 20 bit interface

// default reference voltage
#define MAX5717_VREF (4.096f)

#if defined USE_MAX5717
#define MAX571X_DATA_LENGTH 2
#define MAX571X_CODE_MAX (0xFFFF)
#endif

#if defined USE_MAX5719
#define MAX571X_DATA_LENGTH 3
#define MAX571X_CODE_MAX (0x0FFFFF)
#endif

typedef struct {
    SPI_HandleTypeDef *hspix;
    GPIO_TypeDef *csPort;
    uint16_t     csPin;
    GPIO_TypeDef *latchPort;
    uint16_t     latchPin;
    float vref;
} MAX5717_t;

uint32_t MAX5717_VoltageToCode (MAX5717_t *hdac, float volt);
uint8_t  MAX5717_SetVoltage    (MAX5717_t *hdac, float volt);
uint8_t  MAX5717_Init          (MAX5717_t *hdac, SPI_HandleTypeDef *hspi, float vref);
uint8_t  MAX5717_SendCode      (MAX5717_t *hdac, uint32_t code);

#endif  /* MAX5717_H */

