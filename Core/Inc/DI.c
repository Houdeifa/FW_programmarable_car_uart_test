/*
 * DI.c
 *
 *  Created on: Aug 22, 2025
 *      Author: pc
 */

#ifndef INC_DI_C_
#define INC_DI_C_

#include "stdbool.h"

    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
struct{
	eDI_ID id;
	char name[10];


} tDI_Infos;

enum{
	eID_btn_blue,
	eID_Max

} eDI_ID;

bool DI_Init(void);
bool DI_Read(eDI_Input input_index);



#endif /* INC_DI_C_ */
