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

//for nanopower P31 v6.1
#pragma pack(push, 1)
typedef struct EPS_GET_HK {
	uint16_t pv[3];		    //Photo-voltac input voltage [mV]
	uint16_t pc;		    //Total photo current [mV]
	uint16_t bv;		    //Battery voltage [mV]
	uint16_t sc;		    //Total system current [mA]
	int16_t  temp[4];	    //Temp. of boost coverters (1,2,3) and onboard battery [degC]
	int16_t  batt_temp[2];  //External board battery temperatures [degC]
	uint16_t latchup[6];    //Number of latch-ups on each output 5V and +3.3 channel
                            //[5V1 5V2 5V3 3.3V1 3.3V2 3.3V3]
	uint8_t reset;          //Cause of last EPS reset
	uint16_t bootcount;     //Number of EPS reboots
	uint16_t sw_errors;     //Number of errors in the EPS software
	uint8_t  ppt_mode;      //0 = Hardware, 1 = MPPT, 2 = Fixed SW PPT
	uint8_t  channel_status;//Mask of output channel status, 1=on, 0=off
                            //MSB - [NC NC 3.3V3 3.3V2 3.3V1 5V3 5V2 5V1] - LSB
} eps_get_hk;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct EPS_GET_VERSION {
    uint8_t type[4];
    uint8_t ver[5];
    uint8_t date[12];
    uint8_t time[9];
}eps_get_version;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct EPS_SET_PV_VOLT {
    uint16_t voltage1;
    uint16_t voltage2;
    uint16_t voltage3;
}eps_set_pv_volt;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct EPS_SET_OUTPUT {
    uint8_t output_byte;
}eps_set_output;
#pragma pack(pop)


/* Private define ------------------------------------------------------------*/
/* Buffer size of priTransferDataBuffer*/
#define TRANSFERBUFFERSIZE 50

/* Private variables ---------------------------------------------------------*/
/* FreeRTOS */
xTaskHandle I2CThreadHandle_2;


extern xQueueHandle xQueue_EPS;


/* UART handler declaration */
extern UART_HandleTypeDef UartHandle;

/*I2C communication declaration*/
priI2C_WriteActionTypeDef I2C_transfer_slave_2;

/* register in module*/
static eps_get_hk hk_test;
static eps_get_version version_test;

static eps_set_pv_volt pv_volt_test;
static eps_set_output output_test;


/* Private function prototypes -----------------------------------------------*/
static void I2C_2_Slave_Mode(void *argument);
static void Initial_Register(void);
static void Update_Register(void *argument);
static void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength);



/************variable in environment*************************/
static uint16_t envEPS_Battery_Voltage;


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

    uint8_t priTransferDataBuffer[50];
    
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
				case 0x08:
					I2C_transfer_slave_2.priErrorcode = 0;
					//reply data
				
					I2C_transfer_slave_2.priSizeofReply = 2+43;
				
					priTransferDataBuffer[0] = I2C_transfer_slave_2.priCommand;
					priTransferDataBuffer[1] = I2C_transfer_slave_2.priErrorcode;
					memcpy( (void *)(priTransferDataBuffer+2), (void *)&hk_test, 43);
				break;
			
				case 0x09:
					output_test.output_byte = *(I2C_transfer_slave_2.priRequestPtr);
			
					//print result
					//HAL_UART_Transmit(&UartHandle, &output_test, 1 , 1000);
			
					/* Flush Tx/Rx buffers */
					Flush_Buffer(priTransferDataBuffer,TRANSFERBUFFERSIZE);
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
    memset(&hk_test,0,sizeof(eps_get_hk));
    memset(&version_test,0,sizeof(eps_get_version));
    
    memset(&pv_volt_test,0,sizeof(eps_set_pv_volt));
    memset(&output_test,0,sizeof(eps_set_output));
	

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
    //uint8_t buff[6] ={0,0,0,0,0,0};
    uint8_t *output_test = "popout\n";
		
	portTickType xLastWakeTime;
	
	portBASE_TYPE xStatus;
    
    xData test2;
    uint16_t* temp_int = NULL;
			
	//prvNewPrintString("Hey",3);
	xLastWakeTime = xTaskGetTickCount();
    prvNewPrintString("Update Reg\n",12);
	
    for(;;)
	{
		//prvNewPrintString("Hey",3);    
        /* for Queue Test */
        if(xQueue_EPS!=0)
        {
            //for(;;)
                if(uxQueueMessagesWaiting(xQueue_EPS) != 0)
                {
                    //prvNewPrintString("Hey",3);
                    //prvNewPrintString(output_test,7);
    
                    xStatus = xQueueReceive(xQueue_EPS, &test2 ,0);
                    if(xStatus == pdPASS)
                    {
                        //prvNewPrintString("Success ",8);
                    
                        switch(test2.refRegister)
                        {
                            case ref_envEPS_Battery_Voltage:
                    
                            temp_int = (type_envEPS_Battery_Voltage *)test2.ptrRegister;
                            /*get register value from Queue*/
                            envEPS_Battery_Voltage = *temp_int;
                            break;
                                      
                            default:
                            prvNewPrintString("Something Wrong",15); 
                        }
                
                    /* Print to Screen*/
                    //sprintf (buff, "%d", *temp_int);
                    //prvNewPrintString(buff,6);
                
                    free(temp_int);
                            
                    //Register(Port) 8
                    hk_test.bv = envEPS_Battery_Voltage;
                    
                    //Register(Port) 14
                    }
                    else
                        prvNewPrintString("Fail",4);
                    
                    //vTaskDelayUntil( &xLastWakeTime, 1000 );
                }
                else
                {
                    prvNewPrintString("Empty ",6);
                    break;
                }

        }

			
		//vTaskDelayUntil( &xLastWakeTime, (1000/portTICK_RATE_MS) );
        //prvNewPrintString(output_test,7);
            
        vTaskDelayUntil( &xLastWakeTime, 1000 );
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
