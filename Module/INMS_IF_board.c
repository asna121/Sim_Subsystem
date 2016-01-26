/**
    ***********************************************************************
    * @file     module/INMS_IF_board
    * @author   TIKI
    * @version  V0.1
    * @date     26-January-2016
    * @brief    
    ***********************************************************************
    * 
    *
    *
    *
    *
**/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "peripheral.h"
#include "common.h"

#include "INMS_IF_board.h"
#include "Environment.h"



/* Private typedef -----------------------------------------------------------*/

/* This structure is based on Gomspace Protocol*/
typedef struct{
	
uint8_t priCommand;
uint8_t priErrorcode;
uint8_t *priRequestPtr ;
uint8_t *priReplyPtr ;
uint8_t priSizeofReply;
	
} priI2C_WriteActionTypeDef;

/* Private define ------------------------------------------------------------*/
/* Buffer size of priTransferDataBuffer*/
#define TRANSFERBUFFERSIZE 50

/* Private variables ---------------------------------------------------------*/
/* FreeRTOS */
xTaskHandle I2CThreadHandle_2;

/* UART handler declaration */
extern UART_HandleTypeDef UartHandle;

/*I2C communication declaration*/
priI2C_WriteActionTypeDef I2C_transfer_slave_2;

/************************** register in module**************************/
static ifb_current_temperature ifb_test;


/************variable in environment*************************/
//static uint16_t estimated_x_angular_rate;
//static uint16_t estimated_y_angular_rate;
//static uint16_t estimated_z_angular_rate;


/* Private function prototypes -----------------------------------------------*/
static void I2C_2_Slave_Mode(void *argument);
static void Initial_Register(void);
static void Update_Register(void *argument);
//static void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength);


void submain_IFB(I2C_HandleTypeDef *argument)
{
    
    portBASE_TYPE xStatus;
    
    Initial_Register();
    /* For INMS interface board */
	xStatus = xTaskCreate(I2C_2_Slave_Mode,"I2C Slave", configMINIMAL_STACK_SIZE,(void *)argument, 1, &I2CThreadHandle_2);
	
    //if(xStatus == pdTRUE )     prvNewPrintString(" 1111111 ",12);
    
    /* Update Register in Module*/
	xStatus = xTaskCreate(Update_Register,"register update", configMINIMAL_STACK_SIZE, NULL, 3,NULL);
    
    //if(xStatus == pdTRUE )     prvNewPrintString(" 2222222 ",12);
}


/**
  * @brief  INMS I2C Task
  * @param  argument: I2C's Handle
  * @retval None
  */
static void I2C_2_Slave_Mode(void *argument)
{
	
    I2C_HandleTypeDef* I2CHandle;

    uint8_t priTransferDataBuffer[TRANSFERBUFFERSIZE];
    
    uint8_t* default_string = "012345678996543210";
	
    I2CHandle = (I2C_HandleTypeDef*) argument;
  
    prvNewPrintString("ADCS I2C Task\n",13);
  
	for(;;)
  {
		//prvNewPrintString("Hello",5);
		/*##-2- Slave receive request from master ################################*/
    while(HAL_I2C_Slave_Receive_IT(I2CHandle, priTransferDataBuffer, TRANSFERBUFFERSIZE)!= HAL_OK)
        vTaskDelay(1);
    /*  Before starting a new communication transfer, you need to check the current
    state of the peripheral; if it? busy you need to wait for the end of current
    transfer before starting a new one.
    For simplicity reasons, this example is just waiting till the end of the
    transfer, but application may perform other tasks while transfer operation
    is ongoing. */
    while (HAL_I2C_GetState(I2CHandle) != HAL_I2C_STATE_READY)
    {
        vTaskDelay(1);
    }
		
		
		if(priI2C_NewCommand_2==1)
		{
		
			priI2C_NewCommand_2 =0;
			//get command number and command arguments from master device
			I2C_transfer_slave_2.priCommand = priTransferDataBuffer[0];
			I2C_transfer_slave_2.priRequestPtr = priTransferDataBuffer+1;
		
/* the format of register output to master device. It can call interface format of subsystm */
          
			switch(I2C_transfer_slave_2.priCommand)
			{

                /* Channel 1 INMS 5V current*/
                case 0xb0:
					I2C_transfer_slave_2.priSizeofReply = 4;
                    
                    memset(priTransferDataBuffer,I2C_transfer_slave_2.priCommand,TRANSFERBUFFERSIZE);
                
                    priTransferDataBuffer[0] = (ifb_test.IFB_5V_Current & 0xff00)>>8;
                    priTransferDataBuffer[1] = ifb_test.IFB_5V_Current & 0xff;
                    priTransferDataBuffer[2] = 0xff;

                    break;
                /* Channel 0 INMS 3_3V current*/
                case 0x90:
                    
                    I2C_transfer_slave_2.priSizeofReply = 4;
                
                    memset(priTransferDataBuffer,I2C_transfer_slave_2.priCommand,TRANSFERBUFFERSIZE);
                    
                    priTransferDataBuffer[0] = (ifb_test.IFB_3_3V_Current & 0xff00)>>8;
                    priTransferDataBuffer[1] = ifb_test.IFB_3_3V_Current & 0xff;
                    priTransferDataBuffer[2] = 0xff;
                    
                    break;
                /* Channel 2 INMS INMS temperature*/
                case 0xd0:
                    
                    I2C_transfer_slave_2.priSizeofReply = 4;
                
                    memset(priTransferDataBuffer,I2C_transfer_slave_2.priCommand,TRANSFERBUFFERSIZE);
                    
                    priTransferDataBuffer[0] = (ifb_test.IFB_INMS_Temp & 0xff00)>>8;
                    priTransferDataBuffer[1] = ifb_test.IFB_INMS_Temp & 0xff;
                    priTransferDataBuffer[2] = 0xff;
                    
                    break;
                /* Channel 3 INMS INMS Interface Board temperature*/
                case 0xf0:
                    
                    I2C_transfer_slave_2.priSizeofReply = 4;
                    
                    memset(priTransferDataBuffer,I2C_transfer_slave_2.priCommand,TRANSFERBUFFERSIZE);
                    
                    priTransferDataBuffer[0] = (ifb_test.IFB_IFB_Temp & 0xff00)>>8;
                    priTransferDataBuffer[1] = ifb_test.IFB_IFB_Temp & 0xff;
                    priTransferDataBuffer[2] = 0xff;
                    
                    break;
                default:
                    /* send a default string to remind user*/
                    memcpy( (void *)(priTransferDataBuffer), (void *)default_string, 18);
			}
		
		}
    
	}
}
/**
  * @brief  initial register
  * @param  None
  * @retval None
  */
static void Initial_Register(void)
{
/* the aera to set or initial the value of all register define in this file*/
    
    
    /*inital ID 0xb0 register*/
    memset(&ifb_test,0,sizeof(ifb_current_temperature));
	ifb_test.IFB_5V_Current = 0X1234;

    /*inital ID 0xb0 register*/
}
 /**
  * @brief  update register
  * @param  None
  * @retval None
  */
static void Update_Register(void *argument)
{
    /* for the test */    
    uint8_t buff[6] ={0,0,0,0,0,0};
    
    /* for period */
    portTickType xLastWakeTime;
    //uint8_t period = 0;
	
    /* for queue */
	portBASE_TYPE xStatus;
    
    /*temp of xData*/
    xData temp_queue_data;
    
    /*temp of subsystem data package, please ref to "Environment.h"*/
    uint16_t temp_ref_package = 0;
    xData_IFB_Package_1* temp_package = NULL;
    
	xLastWakeTime = xTaskGetTickCount();
    prvNewPrintString("Update Reg\n",12);
	
    for(;;)
	{
        /* for Queue Test */
        if(xQueue_IFB!=0)
        {
            while(uxQueueMessagesWaiting(xQueue_IFB) != 0)
            {
                /* Peek the IFB Queue */
                xStatus = xQueuePeek(xQueue_IFB, &temp_queue_data ,0);
                temp_ref_package = temp_queue_data.refPackage;
                
                switch (temp_ref_package)
                {
                    case ref_envIFB_Package_1:
                        xStatus = xQueueReceive(xQueue_IFB, &temp_queue_data ,0);
                        
                        if(xStatus == pdPASS)
                        {
                        //prvNewPrintString("Success ",8);
                    
                        temp_package = (xData_IFB_Package_1 *)temp_queue_data.ptrPackage;

                        /*Set register value from Queue*/
                        ifb_test.IFB_5V_Current = (*temp_package).envIFB_5V_Current;
                        ifb_test.IFB_3_3V_Current = (*temp_package).envIFB_3_3V_Current;
                        ifb_test.IFB_INMS_Temp = (*temp_package).envIFB_INMS_Temp;
                        ifb_test.IFB_IFB_Temp = (*temp_package).envIFB_IFB_Temp;

                        /* Print to Screen*/
                        sprintf (buff, "%04X", ifb_test.IFB_5V_Current);
                        prvNewPrintString(buff,6);
                
                        vPortFree(temp_package);
                        }
                        else
                            prvNewPrintString("Fail",4);
                        
                        break;
                    //case ref_envADCS_Package_2:
                    //    break;
                    default:
                        prvNewPrintString("Ref. Package Error ",19);
                }
            }

        }

			
		//vTaskDelayUntil( &xLastWakeTime, (1000/portTICK_RATE_MS) );
        //prvNewPrintString("popout\n",7);
        
        /* Data Update Period*/
        vTaskDelayUntil( &xLastWakeTime, 10 );
	}
}
  

/**
  * @brief  Flushes the buffer
  * @param  pBuffer: buffers to be flushed.
  * @param  BufferLength: buffer's length
  * @retval None
  */
//static void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength)
//{
//  while (BufferLength--)
//  {
//    *pBuffer = 0;
//
//    pBuffer++;
// }
//}
