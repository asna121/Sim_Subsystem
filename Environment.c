/**
    ***********************************************************************
    * @file     module/Enviroment.c
    * @author   TIKI
    * @version  V0.1
    * @date     25-November-2015
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
#include "semphr.h"
#include "task.h"
#include "queue.h"

#include "common.h"

#include "Environment.h"

#define repeat


/* Private variables ---------------------------------------------------------*/
FATFS USBDISKFatFs;           /* File system object for USB disk logical drive */
uint8_t USBDISKPath[4];          /* USB Host logical drive path */

/*File in the disk*/
//FIL MyFile_EPS;                   /* File object */

FIL MyFile_ADCS_X;                   /* File object */
FIL MyFile_ADCS_Y;                   /* File object */
//FIL MyFile_ADCS_Z;                   /* File object */

USBH_HandleTypeDef hUSB_Host;	/* USB Host handle */

typedef enum {
  APPLICATION_IDLE = 0,  
  APPLICATION_READY,
  APPLICATION_RUNNING,	//not used yet
}MSC_ApplicationTypeDef;

MSC_ApplicationTypeDef Appli_state = APPLICATION_IDLE;

//xQueueHandle AppliEvent;

/*Queue with Corresponding File*/
//xQueueHandle xQueue_EPS;
xQueueHandle xQueue_ADCS;
    
/* UART handler declaration */
extern xSemaphoreHandle uart_lock;


/* Private function prototypes -----------------------------------------------*/
static void Check_Data_Queue(void *argument);
static void StartThread(void *argument);
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);
static void MSC_Application(void);


/* import function*/


void submain_Environment(void)
{
  
	/* USB application task */
	xTaskCreate(StartThread,"USER_Thread", 8 * configMINIMAL_STACK_SIZE, NULL, 0,NULL);

}

/**
  * @brief  Start task
  * @param  pvParameters not used
  * @retval None
  */

static void StartThread(void *argument)
{
    
    
    //prvNewPrintString(" #@RE@# ",12);
    //vTaskDelay(1);
    //prvNewPrintString(" #@OK@# ",12);
    if(FATFS_LinkDriver(&USBH_Driver, (char *)USBDISKPath) == 0)
	{

        /* Init MSC Application */
	
		/* Start Host Library */
		USBH_Init(&hUSB_Host, USBH_UserProcess, 0);
        
        
	
		/* Add Supported Class */
		USBH_RegisterClass(&hUSB_Host, USBH_MSC_CLASS);
	
		/* Start Host Process */
		USBH_Start(&hUSB_Host);
	
		if(f_mount(&USBDISKFatFs, "", 0) != FR_OK)
		{  
			/* FatFs Initialization Error */
			Error_Handler();
		}
		else
			 prvNewPrintString(" #@OK@# ",12);
		
	}
	
    for( ;; )
    {
		/* USB Host Background task */
        USBH_Process(&hUSB_Host);
		
        switch(Appli_state)
        {
        case APPLICATION_READY:
            MSC_Application();
            //Appli_state = APPLICATION_IDLE;
        break;
        
        case APPLICATION_IDLE:
        break;
        
        case APPLICATION_RUNNING:
            //prvNewPrintString(" # ",3);
        default:
        break;
        }
    }
	
}

/**
  * @brief  User Process
  * @param  phost: Host Handle
  * @param  id: Host Library user message ID
  * @retval None
  */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{  
  switch(id)
  { 
  case HOST_USER_SELECT_CONFIGURATION:
    break;
    
  case HOST_USER_DISCONNECTION:
    Appli_state = APPLICATION_IDLE;
    
    f_mount(NULL, (TCHAR const*)"", 0);      
    break;
    
  case HOST_USER_CLASS_ACTIVE:
    Appli_state = APPLICATION_READY;
    break;
    
  default:
    break;
  }
}

static void MSC_Application(void)
{
    //FRESULT res;                                          /* FatFs function common result code */
    prvNewPrintString(" #@START@# ",12);
	
	/* Register the file system object to the FatFs module */
    if(f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
    {
        /* FatFs Initialization Error */
        Error_Handler();
    }
    else
    {
        
        // /* Open the text file object with read access */
        // if(f_open(&MyFile_EPS, "EPS_16s", FA_READ) != FR_OK)
        // {
            // /* 'STM32.TXT' file Open for read Error */
            // Error_Handler();
        // }
        // else
        // {
            // Appli_state = APPLICATION_RUNNING;
            
            // /*Create Queue for each subsystem*/
            // xQueue_EPS = xQueueCreate( Queue_Size , sizeof(xData) ); //for EPS
    
            // // if(xQueue_EPS!=NULL)
            // // prvNewPrintString(" 5555555555 ",12);
    
            // /* Check All Data Queue in Enivronment*/
            // xTaskCreate(Check_Data_Queue,"Data Queue Check", configMINIMAL_STACK_SIZE, NULL, 1,NULL); //Caution
        // }
        
        /* Open the text file object with read access */
        if(f_open(&MyFile_ADCS_X, "ADCS_x", FA_READ) != FR_OK)
        {
            /* 'STM32.TXT' file Open for read Error */
            Error_Handler();
        }
        else
        {
            Appli_state = APPLICATION_RUNNING;

            if(xQueue_ADCS==NULL)
            {
                /*Create Queue for each subsystem*/
                xQueue_ADCS = xQueueCreate( Queue_Number , sizeof(xData) ); //for ADCS
            }
        }

        // /* Open the text file object with read access */
        // if(f_open(&MyFile_ADCS_Y, "ADCS_y", FA_READ) != FR_OK)
        // {
            // /* 'STM32.TXT' file Open for read Error */
            // Error_Handler();
        // }
        // else
        // {
            // Appli_state = APPLICATION_RUNNING;

            // if(xQueue_ADCS!=NULL)
            // {
                // /*Create Queue for each subsystem*/
                // xQueue_ADCS = xQueueCreate( Queue_Size , sizeof(xData) ); //for ADCS
            // }

        // }
 
        if(Appli_state == APPLICATION_RUNNING)
        {
            /* Check All Data Queue in Enivronment*/
            xTaskCreate(Check_Data_Queue,"Data Queue Check", configMINIMAL_STACK_SIZE, NULL, 1,NULL); //Caution
        }
    
    }
    
    /* Unlink the USB disk I/O driver */
    //FATFS_UnLinkDriver((char *)USBDISKPath);
}

//週期為1秒
static void Check_Data_Queue(void *argument)
{
    portTickType xLastWakeTime;

    portBASE_TYPE xStatus;
    
    
    uint32_t bytesread;                     /* File read counts */
    
    uint8_t i = 0;
    
    uint8_t rtext[100];                                   /* File read buffer */   
    
    xData test1;
    xData_ADCS_Package* temp_package = NULL;
    
    /*for test*/
    //uint16_t temp[17] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
    //uint16_t* temp_int = NULL;
    
    //uint16_t* Pri_int = NULL;
    
    /*for print screen*/
	//uint8_t buff[6] ={0,0,0,0,0,0};

    
	xLastWakeTime = xTaskGetTickCount();
    
    for(;;)
    {
        if(Appli_state == APPLICATION_RUNNING)
        {
            
            if(uxQueueMessagesWaiting(xQueue_ADCS) <= (Queue_Number-5))
            {
                 /* Read data from the text file */
                f_read(&MyFile_ADCS_X, rtext, sizeof(xData_ADCS_Package)*5, (void *)&bytesread);   
                
                for(i=0;i<(bytesread/sizeof(xData_ADCS_Package));i++)
                {
                    //創造空間 大小為次系統的Package
                    temp_package = (xData_ADCS_Package *)malloc(sizeof( xData_ADCS_Package ));
            
                    //給值
                    /* for the test */
                    (*temp_package).envADCS_Estimated_Angular_X = ((rtext[i*2] & 0xff) << 8) | (rtext[i*2+1] & 0xff);
            
                    //複製到struct中的成員
                    test1.refRegister = 0x0000;
                    test1.ptrRegister = (void *)temp_package;
        
                    //buffer, sizeof(long)
                    xStatus = xQueueSendToBack(xQueue_ADCS, &test1 ,0);

                    /* Print to Screen*/

                }
                #ifdef repeat
                if((bytesread/sizeof(xData_ADCS_Package))!=5)
                {
                    prvNewPrintString("\n",1);
                    
                    f_rewind(&MyFile_ADCS_X);
                }
                #endif
                
            }
                
        }
        else
        { 
            /* Close the open text file */
            f_close(&MyFile_ADCS_X);
        }
        vTaskDelayUntil( &xLastWakeTime, 250 );        
    }
    
}
