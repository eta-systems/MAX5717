# MAX5717 C Library for STM32 HAL
C library for MAX5717 / MAX5719 16 and 20 bit DAC (for STM32 HAL) 

This library is made for the STM32 HAL (Hardware Abstraction Library) platform. The example code is for STM32CubeMX and Keil uVision 5 IDE.

> **MAX5717/19 Datasheet [maximintegrated.com](https://datasheets.maximintegrated.com/en/ds/MAX5717-MAX5719.pdf)**

---

### Usage

```c
/* USER CODE BEGIN Includes */
#include "max5717.h"           // include the library
```
```c
/* USER CODE BEGIN PV */
MAX5717_t dac1;
/* USER CODE END PV */
```

```c
int main(void)
{
    /* USER CODE BEGIN 2 */

    dac1.csPort = SPI4_CS_GPIO_Port;
    dac1.csPin = SPI4_CS_Pin;
    dac1.latchPort = SPI4_LATCH_GPIO_Port;
    dac1.latchPin = SPI4_LATCH_Pin;
    
    MAX5717_Init(&dac1, &hspi4, 4.0965f);

    float Ts = 0.0001f;
    float f = 50;
    float A = 4.096f/2.0f;
    uint32_t k = 0;
    float volts = 0.0f;

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

        volts = A*(1+ sin(2*PI*f*(float)k*Ts));
        MAX5717_SetVoltage(&dac1, volts);
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
        k++;
    }
  /* USER CODE END 3 */

}

```

---

### Restrictions

Please note that the code was tested for MAX5719 but not for MAX5717.

---

### License

Apache 2.0

Copyright (c) 2022 eta Systems GmbH

