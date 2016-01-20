/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"


/* FatFs includes component */
#include "ff_gen_drv.h"
#include "usbh_diskio.h"

extern void submain_Environment(void);

#pragma pack(push, 1)
typedef struct
{
    uint16_t refPackage;
    //uint8_t ucSource;
    void* ptrPackage;
}xData;
#pragma pack(pop)

/*
There are differnet period of packge in the same subsystem.
*/

/* ADCS Package 1 */
#pragma pack(push, 1)
typedef struct
{
    uint16_t envADCS_Estimated_Angular_X;
    uint16_t envADCS_Estimated_Angular_Y;
    uint16_t envADCS_Estimated_Angular_Z;

}xData_ADCS_Package_1;
#pragma pack(pop)

/* ADCS Package 2 */
// #pragma pack(push, 1)
// typedef struct
// {
    // uint16_t envADCS_Estimated_Angular_X;
    // uint16_t envADCS_Estimated_Angular_Y;
    // uint16_t envADCS_Estimated_Angular_Z;

// }xData_ADCS_Package;
// #pragma pack(pop)

#define Queue_Number  30

/*ADCS Queue*/
#define ref_envADCS_Package_1  (0x0201)
#define ref_envADCS_Package_2  (0x0202)

/*EPS Queue*/
#define ref_envEPS_Pacakge_1 (0x0101)

/*
Define the input file in the USB disk.
*/

/*File Name*/

/*File Item Size*/
#define size_fileADCS_Estimated_Angular_X 2
#define size_fileADCS_Estimated_Angular_Y 2
#define size_fileADCS_Estimated_Angular_Z 2








// /* */
// #define type_envEPS_Battery_Voltage uint16_t
// #define type_envADCS_Estimated_Angular uint16_t


/*Export */
//extern xQueueHandle xQueue_EPS;
extern xQueueHandle xQueue_ADCS;

#endif /* __ENVIRONMENT_H */
