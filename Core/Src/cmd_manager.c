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
	void (*cmdCallback)(void);
} tCMDInfos;

typedef enum{
	eCMD_Help = 0,
	eCMD_Version,
	eCMD_Reset,
	eCMD_TaskCodeReceive,
	eCMD_Max
} eCMD_ID;

static eCMDSM_STATE sStateMachine;

static void help_callback(void);
static void version_callback(void);
static void reset_callback(void);
static void task_code_receive_callback(void);

tCMDInfos sCMDTable[] = {
		{.cmdID = eCMD_Help,.pcmdName = "help",.cmdChars = "AT+h",.cmdlen = 4,.cmdCallback = help_callback},
		{.cmdID = eCMD_Version,.pcmdName = "version",.cmdChars = "AT+v",.cmdlen = 4,.cmdCallback = version_callback},
		{.cmdID = eCMD_Reset,.pcmdName = "reset",.cmdChars = "AT+r",.cmdlen = 4,.cmdCallback = reset_callback},
		{.cmdID = eCMD_TaskCodeReceive,.pcmdName = "task_code_receive",.cmdChars = "AT+t",.cmdlen = 4,.cmdCallback = task_code_receive_callback},
};

static void help_callback(void){
	UART_Send(eID_UART2,"Help command\r\n",14);
	sStateMachine = eSTATE_Ready;
}
static void version_callback(void){
	UART_Send(eID_UART2,"Version 1.0.0\r\n",16);
	sStateMachine = eSTATE_Ready;
}
static void reset_callback(void){
	UART_Send(eID_UART2,"Resetting...\r\n",15);
	sStateMachine = eSTATE_Ready;
}
static void task_code_receive_callback(void){
	UART_Send(eID_UART2,"Receiving the task code ...\r\n",30);
	TASK_Manager_StartReceiveTaskCode();
}

void CMD_SetToReadyState(void){
	sStateMachine = eSTATE_Ready;
}

void CMD_Init(){
	sStateMachine = eSTATE_Uknown;


	sStateMachine = eSTATE_Init;

}
#define CMD_BUFFER_SIZE 10
void CMD_Process(){
	static uint8_t buffer[CMD_BUFFER_SIZE] = {0};
	static uint8_t buffer_index = 0;
	uint8_t current_char = 0;
	uint8_t cmdLen = 0;
	static bool cmd_processed = true;
	if(sStateMachine == eSTATE_Uknown) return;
	switch(sStateMachine)
	{
		case eSTATE_Init:
			//initialization
			UART_Send(eID_UART2,"CMD Manager Initialized\r\n",25);
			buffer_index = 0;
			memset(buffer,0,sizeof(buffer));
			sStateMachine = eSTATE_Ready;
			break;
		case eSTATE_Ready:
			if(UART_Receive_timeout(eID_UART2,&current_char,1,0) == true){
				//command received
				if((current_char == 0x0D) || (buffer_index >= sizeof(buffer))){
					sStateMachine = eSTATE_Busy;
					cmd_processed = false;
					UART_Send(eID_UART2,"\r\n",2);
				}else if(current_char == 0x7f){
					buffer[buffer_index] = 0;
					buffer_index--;
				}
				else{
					buffer[buffer_index] = current_char;
					buffer_index++;
				}
			}
			else{
				//no command
			}
			//wait for command
			break;
		case eSTATE_Busy:
				int found_index = -1;
				if(cmd_processed == true){
					break;
				}
				cmdLen = strlen((char*)buffer);
				for(int i = 0;i < eCMD_Max;i++){
					if((cmdLen == sCMDTable[i].cmdlen) && (strncmp((char*)buffer,sCMDTable[i].cmdChars,cmdLen) == 0)){
						//command found
						found_index = i;
						break;
					}
				}
				if(found_index == -1){
					UART_Send(eID_UART2,"Unknown command\r\n",17);
					sStateMachine = eSTATE_Ready;
				}else{
					if(sCMDTable[found_index].cmdCallback != NULL)
						sCMDTable[found_index].cmdCallback();
				}
				memset(buffer,0,sizeof(buffer));
				buffer_index = 0;
				cmd_processed = true;
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
