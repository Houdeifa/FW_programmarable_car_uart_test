/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

typedef struct{
	eDIO_ID id;
	char name[20];
	uint32_t gpio;
	uint32_t pin;
	uint32_t mode;
	uint32_t pull;
	uint32_t speed;
	uint32_t alternate;
	int32_t default_value;
} tDIO_Infos;



static tDIO_Infos sDIOTable[] = {
		{.id = eID_btn_blue,.name = "Blue Button",.gpio=B1_GPIO_Port,.pin = B1_Pin,.mode = GPIO_MODE_EVT_RISING,.pull = GPIO_NOPULL,.default_value = -1},
		{.id = eID_btn_boot,.name = "Boot1 Button",.gpio=BOOT1_GPIO_Port,.pin = BOOT1_Pin,.mode = GPIO_MODE_INPUT,.pull = GPIO_NOPULL,.default_value = -1},
		{.id = eID_LED4,.name = "LED4",.gpio=GPIOD,.pin = LD4_Pin,.mode = GPIO_MODE_OUTPUT_PP,.pull = GPIO_NOPULL,.speed = GPIO_SPEED_FREQ_LOW,.default_value = -1},
		{.id = eID_LED3,.name = "LED3",.gpio=GPIOD,.pin = LD3_Pin,.mode = GPIO_MODE_OUTPUT_PP,.pull = GPIO_NOPULL,.speed = GPIO_SPEED_FREQ_LOW,.default_value = -1},
		{.id = eID_LED5,.name = "LED5",.gpio=GPIOD,.pin = LD5_Pin,.mode = GPIO_MODE_OUTPUT_PP,.pull = GPIO_NOPULL,.speed = GPIO_SPEED_FREQ_LOW,.default_value = -1},
		{.id = eID_LED6,.name = "LED6",.gpio=GPIOD,.pin = LD6_Pin,.mode = GPIO_MODE_OUTPUT_PP,.pull = GPIO_NOPULL,.speed = GPIO_SPEED_FREQ_LOW,.default_value = -1}
};

void DIO_GPIO_CLK_ENABLE(uint32_t sGPIO){

	switch(sGPIO){
	case (uint32_t)GPIOA:
		  __HAL_RCC_GPIOA_CLK_ENABLE();
		break;
	case (uint32_t)GPIOB:
		  __HAL_RCC_GPIOB_CLK_ENABLE();
		break;
	case (uint32_t)GPIOC:
		  __HAL_RCC_GPIOC_CLK_ENABLE();
		break;
	case (uint32_t)GPIOD:
		  __HAL_RCC_GPIOD_CLK_ENABLE();
		break;
	case (uint32_t)GPIOE:
		  __HAL_RCC_GPIOE_CLK_ENABLE();
		break;
	case (uint32_t)GPIOF:
		  __HAL_RCC_GPIOF_CLK_ENABLE();
		break;
	case (uint32_t)GPIOG:
		  __HAL_RCC_GPIOG_CLK_ENABLE();
		break;
	case (uint32_t)GPIOH:
		  __HAL_RCC_GPIOH_CLK_ENABLE();
		break;
	}
}

bool DIO_Init(void){
	for(int i = 0;i < eID_DIO_Max ;i++){
		  GPIO_InitTypeDef GPIO_InitStruct = {0};
		  GPIO_InitStruct.Pin = sDIOTable[i].pin;
		  GPIO_InitStruct.Mode = sDIOTable[i].mode;
		  GPIO_InitStruct.Pull = sDIOTable[i].pull;
		  GPIO_InitStruct.Alternate = sDIOTable[i].alternate;
		  GPIO_InitStruct.Speed = sDIOTable[i].speed;

		  DIO_GPIO_CLK_ENABLE(sDIOTable[i].gpio);

		  HAL_GPIO_Init(sDIOTable[i].gpio, &GPIO_InitStruct);
		  if(sDIOTable[i].default_value != -1)
			  HAL_GPIO_WritePin(sDIOTable[i].gpio, sDIOTable[i].pin, sDIOTable[i].default_value);
	}
}
bool DI_Read(eDIO_ID input_index){
	if(input_index > eID_Max) return false;
	return (HAL_GPIO_ReadPin(sDIOTable[input_index].gpio ,sDIOTable[input_index].pin ) == GPIO_PIN_SET);
}

bool DO_Write(eDIO_ID output_index,uint32_t value){

	if(input_index > eID_Max) return false;
	  HAL_GPIO_WritePin(sDIOTable[input_index].gpio , sDIOTable[input_index].pin , value);
}
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
     PC3   ------> I2S2_SD
     PB10   ------> I2S2_CK
*/
void MX_GPIO_Init(void)
{
	DIO_Init();

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
