/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */

#define UART_TIMEOUT 3

typedef struct{
	eUART_ID id;
	char name[20];
	uint32_t instance;
	uint32_t baud_rate;
	uint32_t oversimpling;
	UART_HandleTypeDef handle;
} tUART_Infos;

static tUART_Infos sUARTTable[] = {
		{.id = eID_UART2,.name = "UART2",.instance=USART2,.baud_rate = 115200,.oversimpling = UART_OVERSAMPLING_16}
};

bool UART_Init(void){

	for(int i = 0;i < eID_DIO_Max ;i++){

		  sUARTTable[i].handle.Instance = sUARTTable[i].instance;
		  sUARTTable[i].handle.Init.BaudRate =  sUARTTable[i].baud_rate;
		  sUARTTable[i].handle.Init.WordLength = UART_WORDLENGTH_8B;
		  sUARTTable[i].handle.Init.StopBits = UART_STOPBITS_1;
		  sUARTTable[i].handle.Init.Parity = UART_PARITY_NONE;
		  sUARTTable[i].handle.Init.Mode = UART_MODE_TX_RX;
		  sUARTTable[i].handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		  sUARTTable[i].handle.Init.OverSampling =  sUARTTable[i].oversimpling;
		  if (HAL_UART_Init(&sUARTTable[i].handle) != HAL_OK)
		  {
		    return false;
		  }
	}
	return true;
}

bool UART_Send(eUART_ID id,char * data,uint32_t len){
	return (HAL_UART_Transmit(&sUARTTable[id].handle,data,len,UART_TIMEOUT) == HAL_OK);
}
bool UART_Receive(eUART_ID id,char * data,uint32_t len){
	return ( HAL_UART_Receive(&sUARTTable[id].handle, data, len, UART_TIMEOUT) == HAL_OK);

}
/* USER CODE END 0 */


/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */
	if(UART_Init() == false){
		Error_Handler();
	}
  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
