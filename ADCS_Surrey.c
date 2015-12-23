/**
    ***********************************************************************
    * @file     module/ADCS_Surrey
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

#include "ADCS_Surrey.h"
#include "Environment.h"



/* Private typedef -----------------------------------------------------------*/
typedef struct{
	
uint8_t priCommand;
uint8_t priErrorcode;
uint8_t *priRequestPtr ;
uint8_t *priReplyPtr ;
uint8_t priSizeofReply;
	
} priI2C_WriteActionTypeDef;

/*for Surrey bundle v1.0*/

/**
Estimated X,Y,Z angular rate. Formatted value is obtained using the formula: 
(formatted value) [deg/s] = (raw channel value)*0.001
**/
#pragma pack(push, 1)
typedef struct ADCS_ESTIMATED_ANGULAR_RATES {
	uint16_t x_rate;		    //Estimated X Angular Rate [deg/s]
	uint16_t y_rate;		    //Estimated Y Angular Rate [deg/s]
	uint16_t z_rate;		    //Estimated Z Angular Rate [deg/s]
} adcs_estimated_angular_rates;
#pragma pack(pop)

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

/* register in module*/
static adcs_estimated_angular_rates angular_test;

/* Private function prototypes -----------------------------------------------*/
static void I2C_2_Slave_Mode(void *argument);
static void Initial_Register(void);
static void Update_Register(void *argument);
//static void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength);



/************variable in environment*************************/
//static uint16_t estimated_x_angular_rate;
//static uint16_t estimated_y_angular_rate;
//static uint16_t estimated_z_angular_rate;


void submain_ADCS(I2C_HandleTypeDef *argument)
{
    
    portBASE_TYPE xStatus;
    
    Initial_Register();
    /* For GomSpace NanoPower P31U */
	xStatus = xTaskCreate(I2C_2_Slave_Mode,"I2C Slave", configMINIMAL_STACK_SIZE,(void *)argument, 1, &I2CThreadHandle_2);
	
    //if(xStatus == pdTRUE )     prvNewPrintString(" 1111111 ",12);
    
    /* Update Register in Module*/
	xStatus = xTaskCreate(Update_Register,"register update", configMINIMAL_STACK_SIZE, NULL, 3,NULL);
    
    //if(xStatus == pdTRUE )     prvNewPrintString(" 2222222 ",12);
}


/**
  * @brief  ADCS I2C Task
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
				//prvNewPrintString("Chuck",5);
    }
		
		
		if(priI2C_NewCommand_2==1)
		{
		
			priI2C_NewCommand_2 =0;
			//get command number and command arguments from master device
			I2C_transfer_slave_2.priCommand = priTransferDataBuffer[0];
			I2C_transfer_slave_2.priRequestPtr = priTransferDataBuffer+1;
		
	
			switch(I2C_transfer_slave_2.priCommand)
			{
				case 0x92:
					I2C_transfer_slave_2.priErrorcode = 0;
					//reply data
				
					I2C_transfer_slave_2.priSizeofReply = 2+6;
				
					priTransferDataBuffer[0] = I2C_transfer_slave_2.priCommand;
					priTransferDataBuffer[1] = I2C_transfer_slave_2.priErrorcode;
					memcpy( (void *)(priTransferDataBuffer+2), (void *)&angular_test, 6);
				break;
                default:
                    priTransferDataBuffer[0] = 0xff;
					priTransferDataBuffer[1] = 0xEE;
                    memcpy( (void *)(priTransferDataBuffer+2), (void *)default_string, 18);
                    
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
    memset(&angular_test,0,sizeof(adcs_estimated_angular_rates));
	

    angular_test.x_rate = 0X1234;   
}
 /**
  * @brief  initial register
  * @param  None
  * @retval None
  */
static void Update_Register(void *argument)
{
    /* for the test */    
	portTickType xLastWakeTime;
    uint8_t buff[6] ={0,0,0,0,0,0};
    
    //uint8_t period = 0;
	
	portBASE_TYPE xStatus;
    
    xData temp_queue_data;
    //uint16_t* temp_int = NULL;
    
    xData_ADCS_Package* temp_package = NULL;
			
	//prvNewPrintString("Hey",3);
	xLastWakeTime = xTaskGetTickCount();
    prvNewPrintString("Update Reg\n",12);
	
    for(;;)
	{
        /* for Queue Test */
        if(xQueue_ADCS!=0)
        {
            if(uxQueueMessagesWaiting(xQueue_ADCS) != 0)
            {
    
                xStatus = xQueueReceive(xQueue_ADCS, &temp_queue_data ,0);
                if(xStatus == pdPASS)
                {
                    //prvNewPrintString("Success ",8);
                    
                    temp_package = (xData_ADCS_Package *)temp_queue_data.ptrRegister;

                    /*Set register value from Queue*/
                    angular_test.x_rate = (*temp_package).envADCS_Estimated_Angular_X;
                    angular_test.y_rate = (*temp_package).envADCS_Estimated_Angular_Y;
                    angular_test.z_rate = (*temp_package).envADCS_Estimated_Angular_Z;
                    /* Print to Screen*/
                    sprintf (buff, "%d", angular_test.y_rate);
                    prvNewPrintString(buff,6);
                
                    free(temp_package);
                }
                else
                    prvNewPrintString("Fail",4);

            }
            else
            {
                prvNewPrintString("Empty ",6);
                break;
            }

        }

			
		//vTaskDelayUntil( &xLastWakeTime, (1000/portTICK_RATE_MS) );
        //prvNewPrintString("popout\n",7);
            
        vTaskDelayUntil( &xLastWakeTime, 1000 );
        
        //period = (++period)%60;
        
        /* Print to Screen*/
        //sprintf (buff, "%02d", period);
        //prvNewPrintString(buff,2);
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
