/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#include "stdbool.h"

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

typedef enum{
	eID_btn_blue = 0,
	eID_btn_boot,
	eID_LED4,
	eID_LED3,
	eID_LED5,
	eID_LED6,
	eID_DIO_MAX
} eDIO_ID;

typedef enum{
  eSTATE_Uknown = -1,
  eSTATE_Pressed = 0,
  eSTATE_Released,
  eSTATE_Short_Press,
  eSTATE_Long_Press,
} eButton_State;

bool DIO_Init(void);
bool DI_Read(eDIO_ID input_index);
bool DO_Write(eDIO_ID output_index,uint32_t value);
void DIO_Button_Poll(void);
eButton_State DI_Button_GetState(eDIO_ID id);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

