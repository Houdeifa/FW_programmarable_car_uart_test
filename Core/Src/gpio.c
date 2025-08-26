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
#include "tim.h"

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
	eButton_State state;
	eButton_State real_state;
	uint32_t press_time;
	uint64_t pressed_timer;
	uint64_t released_timer;
} tDIO_Infos;


static tDIO_Infos sDIOTable[] = {
		{.id = eID_btn_blue,.name = "Blue Button",.gpio=B1_GPIO_Port,.pin = B1_Pin,.mode = GPIO_MODE_EVT_RISING,.pull = GPIO_NOPULL,.default_value = -1},
		{.id = eID_btn_boot,.name = "Boot1 Button",.gpio=BOOT1_GPIO_Port,.pin = BOOT1_Pin,.mode = GPIO_MODE_INPUT,.pull = GPIO_NOPULL,.default_value = -1},
		{.id = eID_LED4,.name = "LED4",.gpio=GPIOD,.pin = LD4_Pin,.mode = GPIO_MODE_OUTPUT_PP,.pull = GPIO_NOPULL,.speed = GPIO_SPEED_FREQ_LOW,.default_value = -1},
		{.id = eID_LED3,.name = "LED3",.gpio=GPIOD,.pin = LD3_Pin,.mode = GPIO_MODE_OUTPUT_PP,.pull = GPIO_NOPULL,.speed = GPIO_SPEED_FREQ_LOW,.default_value = -1},
		{.id = eID_LED5,.name = "LED5",.gpio=GPIOD,.pin = LD5_Pin,.mode = GPIO_MODE_OUTPUT_PP,.pull = GPIO_NOPULL,.speed = GPIO_SPEED_FREQ_LOW,.default_value = -1},
		{.id = eID_LED6,.name = "LED6",.gpio=GPIOD,.pin = LD6_Pin,.mode = GPIO_MODE_OUTPUT_PP,.pull = GPIO_NOPULL,.speed = GPIO_SPEED_FREQ_LOW,.default_value = -1}
};

eButton_State DI_Button_GetState(eDIO_ID id){
	return sDIOTable[id].state;
}


void DIO_Button_Poll(void){
	for(int i = 0;i < eID_DIO_MAX ;i++){
		if(sDIOTable[i].mode == GPIO_MODE_INPUT || sDIOTable[i].mode == GPIO_MODE_EVT_RISING){
			bool current_state = DI_Read(sDIOTable[i].id);
			switch(sDIOTable[i].state){
			case eSTATE_Uknown:
				if(current_state == true){
					sDIOTable[i].state = eSTATE_Pressed;
					sDIOTable[i].real_state = eSTATE_Pressed;
					sDIOTable[i].pressed_timer = TMR_Now();
				}else{
					sDIOTable[i].state = eSTATE_Released;
					sDIOTable[i].real_state = eSTATE_Released;
					sDIOTable[i].released_timer = TMR_Now();
				}
				break;
			case eSTATE_Pressed:
				if(current_state == false){
					if(sDIOTable[i].real_state != eSTATE_Released){
						sDIOTable[i].released_timer = TMR_Now();
						sDIOTable[i].real_state = eSTATE_Released;
					}
					else if(sDIOTable[i].real_state == eSTATE_Released && TMR_Elapsed(sDIOTable[i].released_timer) > 50000){
						sDIOTable[i].state = eSTATE_Released;
						sDIOTable[i].press_time = sDIOTable[i].released_timer - sDIOTable[i].pressed_timer;
					}
				}else{
					if(TMR_Elapsed(sDIOTable[i].pressed_timer) >= 50000UL){ // 50 ms
						sDIOTable[i].state = eSTATE_Short_Press;
						sDIOTable[i].real_state = eSTATE_Short_Press;
					}
				}
				break;
			case eSTATE_Released:
				if(current_state == true){
					sDIOTable[i].state = eSTATE_Pressed;
					sDIOTable[i].real_state = eSTATE_Pressed;
					sDIOTable[i].pressed_timer = TMR_Now();
				}else{
					sDIOTable[i].released_timer = TMR_Now();
				}
				break;
			case eSTATE_Short_Press:
				if(current_state == false){
					if(sDIOTable[i].real_state != eSTATE_Released){
						sDIOTable[i].released_timer = TMR_Now();
						sDIOTable[i].real_state = eSTATE_Released;
					}
					else if(sDIOTable[i].real_state == eSTATE_Released && TMR_Elapsed(sDIOTable[i].released_timer) > 50000){
						sDIOTable[i].state = eSTATE_Released;
						sDIOTable[i].press_time = sDIOTable[i].released_timer - sDIOTable[i].pressed_timer;
					}
				}else{
					if(TMR_Elapsed(sDIOTable[i].pressed_timer) >= 3000000UL){ // 3 secondes
						sDIOTable[i].state = eSTATE_Long_Press;
						sDIOTable[i].real_state = eSTATE_Long_Press;
					}
				}
				break;
			case eSTATE_Long_Press:
				if(current_state == false){
					if(sDIOTable[i].real_state != eSTATE_Released){
						sDIOTable[i].released_timer = TMR_Now();
						sDIOTable[i].real_state = eSTATE_Released;
					}
					else if(sDIOTable[i].real_state == eSTATE_Released && TMR_Elapsed(sDIOTable[i].released_timer) > 50000){
						sDIOTable[i].state = eSTATE_Released;
						sDIOTable[i].press_time = sDIOTable[i].released_timer - sDIOTable[i].pressed_timer;
					}
				}else{
				}
				break;
			default:
				sDIOTable[i].state = eSTATE_Uknown;
				break;
			}
		}
	}
}
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
	for(int i = 0;i < eID_DIO_MAX ;i++){
		  GPIO_InitTypeDef GPIO_InitStruct = {0};
		  GPIO_InitStruct.Pin = sDIOTable[i].pin;
		  GPIO_InitStruct.Mode = sDIOTable[i].mode;
		  GPIO_InitStruct.Pull = sDIOTable[i].pull;
		  GPIO_InitStruct.Alternate = sDIOTable[i].alternate;
		  GPIO_InitStruct.Speed = sDIOTable[i].speed;
		  sDIOTable[i].state = eSTATE_Uknown;
		  sDIOTable[i].press_time = 0;
		  sDIOTable[i].pressed_timer = 0;
		  sDIOTable[i].released_timer = 0;

		  DIO_GPIO_CLK_ENABLE(sDIOTable[i].gpio);

		  HAL_GPIO_Init(sDIOTable[i].gpio, &GPIO_InitStruct);
		  if(sDIOTable[i].default_value != -1)
			  HAL_GPIO_WritePin(sDIOTable[i].gpio, sDIOTable[i].pin, sDIOTable[i].default_value);
	}
}
bool DI_Read(eDIO_ID input_index){
	if(input_index > eID_DIO_MAX) return false;
	return (HAL_GPIO_ReadPin(sDIOTable[input_index].gpio ,sDIOTable[input_index].pin ) == GPIO_PIN_SET);
}

bool DO_Write(eDIO_ID output_index,uint32_t value){

	if(output_index > eID_DIO_MAX) return false;
	  HAL_GPIO_WritePin(sDIOTable[output_index].gpio , sDIOTable[output_index].pin , value);
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
