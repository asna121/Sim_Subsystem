/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

/* Includes -----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"


/* FatFs includes component */
#include "ff_gen_drv.h"
#include "usbh_diskio.h"

/* Export -------------------------------------------------------------------*/

/* Queue Handler of each Subsystem */

//extern xQueueHandle xQueue_EPS;
//extern xQueueHandle xQueue_ADCS;
extern xQueueHandle xQueue_IFB;

/* submain of Environment*/
extern void submain_Environment(void);


/* Structure ----------------------------------------------------------------*/

/* xData transmit  format*/
#pragma pack(push, 1)
typedef struct
{
    uint16_t refPackage;
    //uint8_t ucSource;
    void* ptrPackage;
}xData;
#pragma pack(pop)

/*
There are differnet subsytem with the same update period. so this system need
a reference to know received package type.

Define Subsystem's Package send to Subsystem's Queue
*/

/* ADCS Package 1 */
// #pragma pack(push, 1)
// typedef struct
// {
    // uint16_t envADCS_Estimated_Angular_X;
    // uint16_t envADCS_Estimated_Angular_Y;
    // uint16_t envADCS_Estimated_Angular_Z;

// }xData_ADCS_Package_1;
// #pragma pack(pop)

/* ADCS Package 2 */
// #pragma pack(push, 1)
// typedef struct
// {
    // uint16_t envADCS_Estimated_Angular_X;
    // uint16_t envADCS_Estimated_Angular_Y;
    // uint16_t envADCS_Estimated_Angular_Z;

// }xData_ADCS_Package;
// #pragma pack(pop)

/* IFB Package 1 */
#pragma pack(push, 1)
typedef struct
{
    uint16_t envIFB_5V_Current;
    uint16_t envIFB_3_3V_Current;
    uint16_t envIFB_INMS_Temp;
    uint16_t envIFB_IFB_Temp;
    
}xData_IFB_Package_1;
#pragma pack(pop)

/* Define -------------------------------------------------------------------*/

/*General Queue Number*/
#define Queue_Number  30

/*
Define the reference number of package. These package data are transfered from 
environment to corresponding subsystem.
*/

/*IFB Queue*/
#define ref_envIFB_Package_1 (0x0301)

/*ADCS Queue*/
#define ref_envADCS_Package_1  (0x0201)
#define ref_envADCS_Package_2  (0x0202)

/*EPS Queue*/
#define ref_envEPS_Package_1 (0x0101)

/*
Define the input file in the USB disk.
*/

/* Total Number of File for all subsystem*/
#define number_of_file 4

/**File Item Size**/

/* IFB File Item Size*/
#define size_fileIFB_5V_Current 2
#define size_fileIFB_3_3V_Current 2
#define size_fileIFB_INMS_Temp 2
#define size_fileIFB_IFB_Temp 2

/* ADCS File Item Size*/
//#define size_fileADCS_Estimated_Angular_X 2
//#define size_fileADCS_Estimated_Angular_Y 2
//#define size_fileADCS_Estimated_Angular_Z 2


#endif /* __ENVIRONMENT_H */
