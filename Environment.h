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

#pragma pack(push, 1)
typedef struct
{
    uint16_t refRegister;
    //uint8_t ucSource;
    void* ptrRegister;
}xData;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    uint16_t envADCS_Estimated_Angular_X;
    uint16_t envADCS_Estimated_Angular_Y;
    uint16_t envADCS_Estimated_Angular_Z;

}xData_ADCS_Package;
#pragma pack(pop)




//extern uint16_t deleteQueue_uint16(QUEUE_uint16 *q);
extern void submain_Environment(void);

#define Queue_Number  10

/*File Name*/

/*File Item Size*/
#define size_fileADCS_Estimated_Angular_X 2
#define size_fileADCS_Estimated_Angular_Y 2
#define size_fileADCS_Estimated_Angular_Z 2
// /*EPS Queue*/
// #define ref_envEPS_Battery_Voltage  (0x0100)



// /*ADCS Queue*/
// #define ref_envADCS_Estimated_Angular_X  (0x0200)
// #define ref_envADCS_Estimated_Angular_Y  (0x0201)
// #define ref_envADCS_Estimated_Angular_Z  (0x0202)


// /* */
// #define type_envEPS_Battery_Voltage uint16_t
// #define type_envADCS_Estimated_Angular uint16_t


/*Export */
//extern xQueueHandle xQueue_EPS;
extern xQueueHandle xQueue_ADCS;

#endif /* __ENVIRONMENT_H */
