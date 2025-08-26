/*
 * cmd_manager.c
 *
 *  Created on: Aug 26, 2025
 *      Author: pc
 */
#include "cmd_manager.h"
#include <string.h>
#include "uart.h"

typedef enum{
	eSTATE_Error = -2,
	eSTATE_Uknown = -1,
	eSTATE_Init = 0,
	eSTATE_Ready,
	eSTATE_Busy
}eCMDSM_STATE;

typedef struct{
	uint8_t cmdID;
	char pcmdName[20];
	char cmdChars[10];
	uint8_t cmdlen;
} tCMDInfos;

typedef enum{
	eCMD_Help = 0,
	eCMD_Version,
	eCMD_Reset,
	eCMD_Max
} eCMD_ID;

tCMDInfos sCMDTable[] = {
		{.cmdID = eCMD_Help,.pcmdName = "help",.cmdChars = "AT+h",.cmdlen = 4},
		{.cmdID = eCMD_Version,.pcmdName = "version",.cmdChars = "AT+v",.cmdlen = 4},
		{.cmdID = eCMD_Reset,.pcmdName = "reset",.cmdChars = "AT+r",.cmdlen = 4},
};


static eCMDSM_STATE sStateMachine;

void CMD_Init(){
	sStateMachine = eSTATE_Uknown;


	sStateMachine = eSTATE_Init;

}
void CMD_Process(){
	static uint8_t buffer[10] = {0};
	uint8_t cmdLen = 0;
	if(sStateMachine != eSTATE_Init) return;
	switch(sStateMachine)
	{
		case eSTATE_Init:
			//initialization
			sStateMachine = eSTATE_Ready;
			break;
		case eSTATE_Ready:
			if(UART_Receive_timeout(eID_UART2,buffer,10,0) == true){
				//command received
				sStateMachine = eSTATE_Busy;
			}
			else{
				//no command
			}
			//wait for command
			break;
		case eSTATE_Busy:
				bool cmdFound = false;
				cmdLen = strlen((char*)buffer);
				for(int i = 0;i < eCMD_Max;i++){
					if(cmdLen == sCMDTable[i].cmdlen){
						if(strncmp((char*)buffer,sCMDTable[i].cmdChars,cmdLen) == 0){
							//command found
							switch(sCMDTable[i].cmdID){
								case eCMD_Help:
									UART_Send(eID_UART2,"Help command\r\n",14);
									break;
								case eCMD_Version:
									UART_Send(eID_UART2,"Version 1.0.0\r\n",16);
									break;
								case eCMD_Reset:
									UART_Send(eID_UART2,"Resetting...\r\n",15);
									break;
								default:
									break;
							}
							cmdFound = true;
							sStateMachine = eSTATE_Ready;
							break;
						}
					}
				}
				if(cmdFound == false){
					UART_Send(eID_UART2,"Unknown command\r\n",17);
					sStateMachine = eSTATE_Ready;
				}
			//process command
			break;
		case eSTATE_Error:
			//error management
			break;
		default:
			sStateMachine = eSTATE_Uknown;
			break;
	}
}
