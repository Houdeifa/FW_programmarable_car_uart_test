/*
 * task_manager.c
 *
 *  Created on: Aug 28, 2025
 *      Author: pc
 */
#include "task_manager.h"
#include "cmd_manager.h"
#include <string.h>
#include "uart.h"

typedef struct{
    uint32_t program_size;
    uint8_t * program_buffer;
    void (*program_entry)(void);
    void (*program_user_utilty1)(void);
    void (*program_user_utilty2)(void);
    void (*program_user_utilty3)(void);
} tProgramInfos;

static void utility1(){
    UART_Send(eID_UART2,"Utility 1 called from task code.\n\r",35);
}

static void utility2(){
    UART_Send(eID_UART2,"Utility 2 called from task code.\n\r",35);
}

static void utility3(){
    UART_Send(eID_UART2,"Utility 3 called from task code.\n\r",35);
}

#define MAX_PROGRAM_SIZE 0x5000
static tProgramInfos sCurrentProgramInfos __attribute__((section(".infos_buffer")));

static uint8_t sProgramBuffer[MAX_PROGRAM_SIZE] __attribute__((section(".program_buffer")));
typedef enum{
    eSTATE_Error = -2,
    eSTATE_Uknown = -1,
    eSTATE_Init = 0,
    eSTATE_Ready,
    eSTATE_ReceiveTaskCode,
    eSTATE_FinishedReceiveTaskCode,
    eSTATE_CodeVerify,
    eSTATE_ExecuteTask,
    eSTATE_Finished,
}eTASKSM_STATE;

static eTASKSM_STATE sStateMachine;

void TASK_Manager_Init(void){
    sStateMachine = eSTATE_Init;
    sCurrentProgramInfos.program_size = 0;
    sCurrentProgramInfos.program_buffer = sProgramBuffer;
    sCurrentProgramInfos.program_entry = (void (*)(void)) sProgramBuffer + 1; //thumb mode
    sCurrentProgramInfos.program_user_utilty1 = &utility1;
    sCurrentProgramInfos.program_user_utilty2 = &utility2;
    sCurrentProgramInfos.program_user_utilty3 = &utility3;
}

void TASK_Manager_StartReceiveTaskCode(void){
    sStateMachine = eSTATE_ReceiveTaskCode;
    UART_Send(eID_UART2,"Start by sending task size...\n\r",31);
}

void TASK_Manager_Process(void){
    static uint16_t buffer_index = 0;
    static int task_size = -1;
    static int tmp_task_size = 0;
    uint8_t current_char = 0;
    uint64_t receiveTimer = 0;
    if(sStateMachine == eSTATE_Uknown) return;
    switch(sStateMachine)
    {
        case eSTATE_Init:
            //initialization
            sStateMachine = eSTATE_Ready;
            task_size = -1;
            buffer_index = 0;
            tmp_task_size = 0;
            memset(sProgramBuffer,0,sizeof(sProgramBuffer));
            UART_Send(eID_UART2,"Task Manager Initialized\r\n",26);
            break;
        case eSTATE_Ready:
            //wait for task
            task_size = -1;
            buffer_index = 0;
            tmp_task_size = 0;
            memset(sProgramBuffer,0x55,sizeof(sProgramBuffer));
            break;
        case eSTATE_ReceiveTaskCode:
            //process task
            bool data_received = false;
            if(TMR_Expired(receiveTimer) && (task_size != -1)){
                //timeout
                UART_Send(eID_UART2,"\n\rTimeout waiting for task code. Resetting...\n\r",48);
                sStateMachine = eSTATE_Ready;
                CMD_SetToReadyState();
                break;
            }
            if(UART_Receive_timeout(eID_UART2,&current_char,1,0) == true){
                if(task_size == -1){
                    //receiving task size
                    if((current_char >= '0') && (current_char <= '9')){
                        tmp_task_size = (tmp_task_size * 10) + (current_char - '0');
                        UART_Send(eID_UART2,&current_char,1);
                    }else if(current_char == 0x0D){
                        //end of size
                        UART_Send(eID_UART2,"\r\n",2);
                        if((tmp_task_size > 0) && (tmp_task_size <= MAX_PROGRAM_SIZE)){
                            UART_Send(eID_UART2,"Size received. Send the task code...\n\r",38);
                            task_size = tmp_task_size;
                            buffer_index = 0;
                            tmp_task_size = 0;
                            receiveTimer = TMR_Set(200000); //200ms timeout between bytes 
                        }else{
                            UART_Send(eID_UART2,"Invalid size. Resetting...\n\r",29);
                            sStateMachine = eSTATE_Ready;
                            CMD_SetToReadyState();
                        }
                    }else if(current_char == 0x7f){
                        //backspace
                        if(tmp_task_size > 0){
                            tmp_task_size = tmp_task_size / 10;
                            UART_Send(eID_UART2,"\b \b",3);
                        }
                    }else{
                        //invalid character
                        UART_Send(eID_UART2,"\n\rInvalid character in size. Resetting...\n\r",45);
                        sStateMachine = eSTATE_Ready;
                        CMD_SetToReadyState();
                    }
                }else{
                    //receiving task code
                    if(buffer_index < task_size){
                        sProgramBuffer[buffer_index] = current_char;
                        buffer_index++;
                        UART_Send(eID_UART2,&current_char,1);
                        receiveTimer = TMR_Set(200000); //reset timeout
                    }
                    if(buffer_index >= task_size){
                        //task received
                        UART_Send(eID_UART2,"\n\rTask code received. Processing...\n\r",37);
                        sStateMachine = eSTATE_FinishedReceiveTaskCode;
                    }
                }
            }
            break;
        case eSTATE_FinishedReceiveTaskCode:
            //task finished receiving
            //Here you can add code to store the received task code or process it as needed.
            sStateMachine = eSTATE_CodeVerify;
            break;
        case eSTATE_CodeVerify:
            //verify code
            //For simplicity, we assume the code is always valid.
            UART_Send(eID_UART2,"Code verified successfully.\n\r",30);
            sStateMachine = eSTATE_ExecuteTask;
            break;
        case eSTATE_ExecuteTask:
            //process task 
            UART_Send(eID_UART2,"Executing task...\n\r",20);
            //Here you would add code to actually execute the received task code.
            sCurrentProgramInfos.program_entry();
            UART_Send(eID_UART2,"Task execution finished.\n\r",26);
            sStateMachine = eSTATE_Finished;
            break;
        case eSTATE_Finished:
            //task finished
            CMD_SetToReadyState();
            sStateMachine = eSTATE_Ready;
            break;
        case eSTATE_Error:
            //error management
            break;
        default:
            sStateMachine = eSTATE_Uknown;
            break;
    }
}


