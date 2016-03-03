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

/**File in the disk**/

/* EPS File object*/
FIL MyFile_EPS;

/* ADCS File object*/
FIL MyFile_ADCS_X;
FIL MyFile_ADCS_Y;
FIL MyFile_ADCS_Z;

/* IFB File object*/
FIL MyFile_IFB_5V_C;
FIL MyFile_IFB_3_3V_C;
FIL MyFile_IFB_INMS_T;
FIL MyFile_IFB_IFB_T;


USBH_HandleTypeDef hUSB_Host;	/* USB Host handle */

typedef enum {
  APPLICATION_IDLE = 0,  
  APPLICATION_READY,
  APPLICATION_RUNNING,	//not used yet
}MSC_ApplicationTypeDef;

MSC_ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*Queue handler with Corresponding Subsystem*/
xQueueHandle xQueue_EPS;
xQueueHandle xQueue_ADCS;
xQueueHandle xQueue_IFB;

/* Private function prototypes ----------------------------------------------*/
static void Update_EPS_Data_in_Period(void *argument);
static void Update_ADCS_Data_in_Period(void *argument);
static void Update_IFB_Data_in_Period(void *argument);
static void StartThread(void *argument);
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);
static void MSC_Application(void);

/* Export function ----------------------------------------------------------*/
void EPS_Data_Input(void);
void ADCS_Data_Input(void);
void IFB_Data_Input(void);



/* import function-----------------------------------------------------------*/

/* import variable-----------------------------------------------------------*/
/* UART handler declaration */

extern xSemaphoreHandle uart_lock;

/**
  * @brief  invoke environment(USB Disk) 
  * @param  None
  * @retval None
  */

void submain_Environment(void)
{
  
	/* USB application task */
	xTaskCreate(StartThread,"USER_Thread", 16 * configMINIMAL_STACK_SIZE, NULL, 0,NULL);

}

/**
  * @brief  Start task
  * @param  pvParameters not used
  * @retval None
  */

static void StartThread(void *argument)
{
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
	
	//Enable EPS Environment Data Input Task
	//EPS_Data_Input();
	
	//Enable ADCS Environment Data Input Task
	//ADCS_Data_Input();
	
	//Enable IFB Environment Data Input Task
	IFB_Data_Input();
	
}

static void readfile_in_disk(FIL* myfile, uint8_t* temp_rtext, uint8_t size_of_item,  uint32_t* temp_byteread)
{
    /* Read data from the text file */
    f_read(myfile, temp_rtext, size_of_item*5, (void *)temp_byteread);
    
    if(((*temp_byteread)/size_of_item)!=5)
    {
     //prvNewPrintString("\n",1);
    #ifdef repeat
    f_rewind(myfile);
    #endif
    }

}


void EPS_Data_Input(void)
{

	/* Register the file system object to the FatFs module */
    if(f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
    {
        /* FatFs Initialization Error */
        Error_Handler();
    }
    else
    {

        /* Open the text file object with read access */
        if(f_open(&MyFile_EPS, "EPS_16s", FA_READ) != FR_OK)
        {
            /* 'STM32.TXT' file Open for read Error */
            Error_Handler();
        }
        else
            prvNewPrintString(" EPS_16s OK! ",13);
         

        Appli_state = APPLICATION_RUNNING;

        /* Once all the file are available, Create corresponding queue for these data package */
        if(xQueue_EPS==NULL)
        //if(xQueue_ADCS==NULL)
        //if(xQueue_IFB==NULL)
        {
            /*Create Queue for each subsystem*/
            
            xQueue_EPS = xQueueCreate( Queue_Number , sizeof(xData) ); //for EPS
            //xQueue_ADCS = xQueueCreate( Queue_Number , sizeof(xData) ); //for ADCS
            //xQueue_IFB = xQueueCreate( Queue_Number , sizeof(xData) ); //for IFB
        }

        
        /* If Queue is opened correctly, then "Update Data" task shall be created */
        if(Appli_state == APPLICATION_RUNNING)
        {
            /* Check All Data Queue in Enivronment*/
            xTaskCreate(Update_EPS_Data_in_Period,"Update EPS Data in Period", configMINIMAL_STACK_SIZE, NULL, 1,NULL); //Caution
        }
    
    }
    
    /* Unlink the USB disk I/O driver */
    //FATFS_UnLinkDriver((char *)USBDISKPath);
	
	
}

void ADCS_Data_Input(void)
{

	/* Register the file system object to the FatFs module */
    if(f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
    {
        /* FatFs Initialization Error */
        Error_Handler();
    }
    else
    {

        /* Open the text file object with read access */
        if(f_open(&MyFile_ADCS_X, "ADCS_wx", FA_READ) != FR_OK)
        {
            /* 'STM32.TXT' file Open for read Error */
            Error_Handler();
        }
        else
            prvNewPrintString(" X_OK ",6);
            
        if(f_open(&MyFile_ADCS_Y, "ADCS_wy", FA_READ) != FR_OK)
        {
            /* 'STM32.TXT' file Open for read Error */
            Error_Handler();
        }
        else
            prvNewPrintString(" Y_OK ",6);
        
        if(f_open(&MyFile_ADCS_Z, "ADCS_wz", FA_READ) != FR_OK)
        {
            /* 'STM32.TXT' file Open for read Error */
            Error_Handler();
        }
        else
            prvNewPrintString(" Z_OK ",6);
         

        Appli_state = APPLICATION_RUNNING;

        /* Once all the file are available, Create corresponding queue for these data package */
        if(xQueue_ADCS==NULL)
        {
            /*Create Queue for each subsystem*/
            xQueue_ADCS = xQueueCreate( Queue_Number , sizeof(xData) ); //for ADCS
        }

        
        /* If Queue is opened correctly, then "Update Data" task shall be created */
        if(Appli_state == APPLICATION_RUNNING)
        {
            /* Check All Data Queue in Enivronment*/
            xTaskCreate(Update_ADCS_Data_in_Period,"Update ADCS Data in Period", configMINIMAL_STACK_SIZE, NULL, 1,NULL); //Caution
        }
    
    }
    
    /* Unlink the USB disk I/O driver */
    //FATFS_UnLinkDriver((char *)USBDISKPath);
	
}


void IFB_Data_Input(void)
{

	/* Register the file system object to the FatFs module */
    if(f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
    {
        /* FatFs Initialization Error */
        Error_Handler();
    }
    else
    {

        /* Open the text file object with read access */
        if(f_open(&MyFile_IFB_5V_C, "IFB_5V", FA_READ) != FR_OK)
        {
            /* 'STM32.TXT' file Open for read Error */
            Error_Handler();
        }
        else
            prvNewPrintString(" IFB_5V_C OK!",13);
            
        if(f_open(&MyFile_IFB_3_3V_C, "IFB_3_3V", FA_READ) != FR_OK)
        {
            /* 'STM32.TXT' file Open for read Error */
            Error_Handler();
        }
        else
            prvNewPrintString(" IFB_3_3V OK! ",14);
        
        if(f_open(&MyFile_IFB_INMS_T, "IFB_INMS", FA_READ) != FR_OK)
        {
            /* 'STM32.TXT' file Open for read Error */
            Error_Handler();
        }
        else
            prvNewPrintString(" IFB_INMS_T OK! ",16);
        
        if(f_open(&MyFile_IFB_IFB_T, "IFB_IFB", FA_READ) != FR_OK)
        {
            /* 'STM32.TXT' file Open for read Error */
            Error_Handler();
        }
        else
            prvNewPrintString(" IFB_IFB_T OK! ",15);
         

        Appli_state = APPLICATION_RUNNING;

        /* Once all the file are available, Create corresponding queue for these data package */
        if(xQueue_IFB==NULL)
        {
            /*Create Queue for each subsystem*/
            xQueue_IFB = xQueueCreate( Queue_Number , sizeof(xData) ); //for IFB
        }

        
        /* If Queue is opened correctly, then "Update Data" task shall be created */
        if(Appli_state == APPLICATION_RUNNING)
        {
            /* Check All Data Queue in Enivronment*/
            xTaskCreate(Update_IFB_Data_in_Period,"Update IFB Data in Period", configMINIMAL_STACK_SIZE, NULL, 1,NULL); //Caution
        }
    
    }
    
    /* Unlink the USB disk I/O driver */
    //FATFS_UnLinkDriver((char *)USBDISKPath);
	
	
}

/**
  * @brief  Put read data into corresponding queue with corresponding period
  * @param  argument: no used
  * @retval None
  */
static void Update_EPS_Data_in_Period(void *argument)
{
    portTickType xLastWakeTime;

    portBASE_TYPE xStatus;
    
	/* File read counts */ 
    uint32_t bytesread_1;                     
    
    // Counter
    uint8_t i = 0;
    
	/* File read buffer */
    uint8_t rtext_1[50];                                      

    
    xData temp_xData;
    xData_EPS_Package_1* temp_package = NULL;
    
    /*for print screen*/
	uint8_t buff[6] ={0,0,0,0,0,0};

    
	xLastWakeTime = xTaskGetTickCount();
    
    for(;;)
    {
        /* data update period */
        vTaskDelayUntil( &xLastWakeTime, 1000 );
        
        if(Appli_state == APPLICATION_RUNNING)
        {
            
            /* if rtext is empty*/
            if(i==0)
            {
                /* Read data from the text file */
                readfile_in_disk(&MyFile_EPS, rtext_1, size_fileEPS_Battery_Voltage, &bytesread_1);        
            }
            
            /*創造空間 大小為次系統的Package*/
            temp_package = (xData_EPS_Package_1 *)pvPortMalloc(sizeof( xData_EPS_Package_1 ));
            //temp_package = (xData_IFB_Package_1 *)pvPortMalloc(sizeof( xData_IFB_Package_1 ));
            
            /*給值*/
            (*temp_package).envEPS_Battery_Voltage = ((rtext_1[i*2] & 0xff) << 8) | (rtext_1[i*2+1] & 0xff);
            
            /*複製到struct中的成員*/
            temp_xData.refPackage = ref_envEPS_Package_1;
            temp_xData.ptrPackage = (void *)temp_package;
        
            //buffer, sizeof(long)
            xStatus = xQueueSendToBack(xQueue_EPS, &temp_xData ,0);
            
            /**  this area can be used for another subsystem's package which has the same period **/
 
        }
        else
        { 
            /* Close the open text file */
            f_close(&MyFile_EPS);
        }
        
        /*read index increase*/
        i=(++i)%5;
        
    }
    
}

/**
  * @brief  Put read data into corresponding queue with corresponding period
  * @param  argument: no used
  * @retval None
  */
static void Update_ADCS_Data_in_Period(void *argument)
{
    portTickType xLastWakeTime;

    portBASE_TYPE xStatus;
    
    
    uint32_t bytesread_1;                     /* File read counts */
    uint32_t bytesread_2;                     /* File read counts */
    uint32_t bytesread_3;                     /* File read counts */
    
    // Counter
    uint8_t i = 0;
    
    uint8_t rtext_1[50];                                   /* File read buffer */   
    uint8_t rtext_2[50];                                   /* File read buffer */  
    uint8_t rtext_3[50];                                   /* File read buffer */     
    
    xData temp_xData;
    xData_ADCS_Package_1* temp_package = NULL;
    
    /*for print screen*/
	uint8_t buff[6] ={0,0,0,0,0,0};

    
	xLastWakeTime = xTaskGetTickCount();
    
    for(;;)
    {
        /* data update period */
        vTaskDelayUntil( &xLastWakeTime, 1000 );
        
        if(Appli_state == APPLICATION_RUNNING)
        {
            
            /* if rtext is empty*/
            if(i==0)
            {
                /* Read data from the text file */
                readfile_in_disk(&MyFile_ADCS_X, rtext_1, size_fileADCS_Estimated_Angular_X, &bytesread_1);
                readfile_in_disk(&MyFile_ADCS_Y, rtext_2, size_fileADCS_Estimated_Angular_Y, &bytesread_2);
                readfile_in_disk(&MyFile_ADCS_Z, rtext_3, size_fileADCS_Estimated_Angular_Z, &bytesread_3);           
            }
            
            /*創造空間 大小為次系統的Package*/
            temp_package = (xData_ADCS_Package_1 *)pvPortMalloc(sizeof( xData_ADCS_Package_1 ));
            
            /*給值*/
            (*temp_package).envADCS_Estimated_Angular_X = ((rtext_1[i*2] & 0xff) << 8) | (rtext_1[i*2+1] & 0xff);
            (*temp_package).envADCS_Estimated_Angular_Y = ((rtext_2[i*2] & 0xff) << 8) | (rtext_2[i*2+1] & 0xff);
            (*temp_package).envADCS_Estimated_Angular_Z = ((rtext_3[i*2] & 0xff) << 8) | (rtext_3[i*2+1] & 0xff);
            
            /*複製到struct中的成員*/
            temp_xData.refPackage = ref_envADCS_Package_1;
            temp_xData.ptrPackage = (void *)temp_package;
        
            //buffer, sizeof(long)
            xStatus = xQueueSendToBack(xQueue_ADCS, &temp_xData ,0);
                       
            /* Print to Screen*/
            //sprintf (buff, "%04X", (*temp_package).envADCS_Estimated_Angular_Z);
            //prvNewPrintString(buff,6);
 
        }
        else
        { 
            /* Close the open text file */
            f_close(&MyFile_ADCS_X);
			f_close(&MyFile_ADCS_Y);
			f_close(&MyFile_ADCS_Z);
        }
        
        /*read index increase*/
        i=(++i)%5;
        
    }
    
}

static void Update_IFB_Data_in_Period(void *argument)
{
    portTickType xLastWakeTime;

    portBASE_TYPE xStatus;
    
    
    uint32_t bytesread_1;                     /* File read counts */
    uint32_t bytesread_2;                     /* File read counts */
    uint32_t bytesread_3;                     /* File read counts */
    uint32_t bytesread_4;                     /* File read counts */     
    
    // Counter
    uint8_t i = 0;
    
    uint8_t rtext_1[50];                                   /* File read buffer */   
    uint8_t rtext_2[50];                                   /* File read buffer */  
    uint8_t rtext_3[50];                                   /* File read buffer */     
    uint8_t rtext_4[50];                                   /* File read buffer */ 
    
    xData temp_xData;
    xData_IFB_Package_1* temp_package = NULL;
    
    /*for print screen*/
	uint8_t buff[6] ={0,0,0,0,0,0};

    
	xLastWakeTime = xTaskGetTickCount();
    
    for(;;)
    {
        /* data update period */
        vTaskDelayUntil( &xLastWakeTime, 1000 );
        
        if(Appli_state == APPLICATION_RUNNING)
        {
            
            /* if rtext is empty*/
            if(i==0)
            {
                /* Read data from the text file */
                readfile_in_disk(&MyFile_IFB_5V_C, rtext_1, size_fileIFB_5V_Current, &bytesread_1);
                readfile_in_disk(&MyFile_IFB_3_3V_C, rtext_2, size_fileIFB_3_3V_Current, &bytesread_2);
                readfile_in_disk(&MyFile_IFB_INMS_T, rtext_3, size_fileIFB_INMS_Temp, &bytesread_3);
                readfile_in_disk(&MyFile_IFB_IFB_T, rtext_4, size_fileIFB_IFB_Temp, &bytesread_4);             
            }
            
            /*創造空間 大小為次系統的Package*/
            temp_package = (xData_IFB_Package_1 *)pvPortMalloc(sizeof( xData_IFB_Package_1 ));
            
            /*給值*/
            (*temp_package).envIFB_5V_Current = ((rtext_1[i*2] & 0xff) << 8) | (rtext_1[i*2+1] & 0xff);
            (*temp_package).envIFB_3_3V_Current = ((rtext_2[i*2] & 0xff) << 8) | (rtext_2[i*2+1] & 0xff);
            (*temp_package).envIFB_INMS_Temp = ((rtext_3[i*2] & 0xff) << 8) | (rtext_3[i*2+1] & 0xff);
            (*temp_package).envIFB_IFB_Temp = ((rtext_3[i*2] & 0xff) << 8) | (rtext_3[i*2+1] & 0xff);
            
            /*複製到struct中的成員*/
            temp_xData.refPackage = ref_envIFB_Package_1;
            temp_xData.ptrPackage = (void *)temp_package;
        
            //buffer, sizeof(long)
            xStatus = xQueueSendToBack(xQueue_IFB, &temp_xData ,0);
            
            /**  this area can be used for another subsystem's package which has the same period **/
        }
        else
        { 
            /* Close the open text file */
            f_close(&MyFile_IFB_5V_C);
            f_close(&MyFile_IFB_3_3V_C);
            f_close(&MyFile_IFB_INMS_T);
            f_close(&MyFile_IFB_IFB_T);
        }
        
        /*read index increase*/
        i=(++i)%5;
        
    }

}

