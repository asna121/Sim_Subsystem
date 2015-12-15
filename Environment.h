/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* FatFs includes component */
#include "ff_gen_drv.h"
#include "usbh_diskio.h"

typedef struct
{
    uint16_t refRegister;
    //uint8_t ucSource;
    void* ptrRegister;
}xData;

//extern uint16_t deleteQueue_uint16(QUEUE_uint16 *q);
extern void submain_Environment(void);

#define Queue_EPS_Size  10

/* */
#define ref_envEPS_Battery_Voltage  (0x0100)


/* */
#define type_envEPS_Battery_Voltage uint16_t


#endif /* __ENVIRONMENT_H */
