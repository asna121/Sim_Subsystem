/**
    ***********************************************************************
    * @file     module/EPS_NanoPower_P31U
    * @author   TIKI
    * @version  V0.1
    * @date     24-November-2015
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

#include "EPS_NanoPower_P31U.h"
#include "Environment.h"



/* Private typedef -----------------------------------------------------------*/
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
static eps_get_hk hk_test;
static eps_get_version version_test;

static eps_set_pv_volt pv_volt_test;
static eps_set_output output_test;

/************variable in environment*************************/
//static uint16_t envEPS_Battery_Voltage;

/* Private function prototypes -----------------------------------------------*/
static void I2C_2_Slave_Mode(void *argument);
static void Initial_Register(void);
static void Update_Register(void *argument);
static void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength);


void submain_EPS(I2C_HandleTypeDef *argument)
{
    
    portBASE_TYPE xStatus;
    
    Initial_Register();
    /* For GomSpace NanoPower P31U */
	xStatus = xTaskCreate(I2C_2_Slave_Mode,"I2C Slave", configMINIMAL_STACK_SIZE,(void *)argument, 1, &I2CThreadHandle_2);
	
    //if(xStatus == pdTRUE )     prvNewPrintString(" 1111111 ",12);
    
    /* Update Register in Module*/
	xStatus = xTaskCreate(Update_Register,"register update", configMINIMAL_STACK_SIZE, NULL, 3,NULL);
    
    //if(xStatus == pdTRUE )     prvNewPrintString(" 1111111 ",12);
}


/**
  * @brief  EPS I2C Task
  * @param  argument: I2C's Handle
  * @retval None
  */
static void I2C_2_Slave_Mode(void *argument)
{
	
    I2C_HandleTypeDef* I2CHandle;

    uint8_t priTransferDataBuffer[TRANSFERBUFFERSIZE];
    
    uint8_t* default_string = "012345678996543210";
	
    I2CHandle = (I2C_HandleTypeDef*) argument;
  
    prvNewPrintString("EPS I2C Task\n",13);
  
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
				/* Node 0x08*/
				case 0x08:
					I2C_transfer_slave_2.priErrorcode = 0;
					//reply data
				
					I2C_transfer_slave_2.priSizeofReply = 2+43;
				
					priTransferDataBuffer[0] = I2C_transfer_slave_2.priCommand;
					priTransferDataBuffer[1] = I2C_transfer_slave_2.priErrorcode;
					memcpy( (void *)(priTransferDataBuffer+2), (void *)&hk_test, 43);
				break;
				
				/* Node 0x09*/
				case 0x09:
					output_test.output_byte = *(I2C_transfer_slave_2.priRequestPtr);
			
					//print result
					//HAL_UART_Transmit(&UartHandle, &output_test, 1 , 1000);
			
					/* Flush Tx/Rx buffers */
					Flush_Buffer(priTransferDataBuffer,TRANSFERBUFFERSIZE);
				break;
        default:
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
    //inital all the member
    memset(&hk_test,0,sizeof(eps_get_hk));
    memset(&version_test,0,sizeof(eps_get_version));
    
    memset(&pv_volt_test,0,sizeof(eps_set_pv_volt));
    memset(&output_test,0,sizeof(eps_set_output));
	

    /*inital Node 0x08 register*/
	hk_test.pc = 0X1234;
/* 
    chkparam_t hk_test = 
	{
	{0x0100,0x0302,0x0504},
	0x0706,
	0x0908,
	0x0B0A,
	{0x0D0C,0x0F0E,0x1110,0x1312},
	{0x1514,0x1716},
	{0x1918,0x1B1A,0x1D1C,0x1F1E,0x2120,0x2322},
	0x24,
	0x2625,
	0x2827,
	0x29,
	0x2A
	};
*/
    
}
 /**
  * @brief  initial register
  * @param  None
  * @retval None
  */
static void Update_Register(void *argument)
{
    /* for the test */    
    uint8_t buff[6] ={0,0,0,0,0,0};
	
	/* for period */
	portTickType xLastWakeTime;
	
	/* for queue */
	portBASE_TYPE xStatus;
    
	/*temp of xData*/
    xData temp_queue_data;
    
    /*temp of subsystem data package, please ref to "Environment.h"*/
    uint16_t temp_ref_package = 0;
    xData_EPS_Package_1* temp_package = NULL;

    
	xLastWakeTime = xTaskGetTickCount();
    prvNewPrintString("Update Reg\n",12);
	
    for(;;)
	{  
        /* for Queue Test */
        if(xQueue_EPS!=0)
        {
            while(uxQueueMessagesWaiting(xQueue_EPS) != 0)
            {
                 /* Peek the IEPS Queue */
                xStatus = xQueuePeek(xQueue_EPS, &temp_queue_data ,0);
                temp_ref_package = temp_queue_data.refPackage;

                switch (temp_ref_package)
                {
                    case ref_envEPS_Package_1:
                        xStatus = xQueueReceive(xQueue_EPS, &temp_queue_data ,0);
                        
                        if(xStatus == pdPASS)
                        {
                        //prvNewPrintString("Success ",8);
                    
                        temp_package = (xData_EPS_Package_1 *)temp_queue_data.ptrPackage;

                        /*Set register value from Queue*/
                        hk_test.bv = (*temp_package).envEPS_Battery_Voltage;

                        /* Print to Screen*/
                        sprintf (buff, "%04X", hk_test.bv);
                        prvNewPrintString(buff,6);
                
                        vPortFree(temp_package);
                        }
                        else
                            prvNewPrintString("Fail",4);
                        
                        break;
                    default:
                        prvNewPrintString("Ref. Package Error ",19);
                }
                
            }
        }

			
		//vTaskDelayUntil( &xLastWakeTime, (1000/portTICK_RATE_MS) );
        //prvNewPrintString(output_test,7);
            
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
static void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    *pBuffer = 0;

    pBuffer++;
  }
}
