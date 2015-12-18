#include "stm32f4xx_hal.h"

extern uint8_t priI2C_NewCommand_2;

void submain_EPS(I2C_HandleTypeDef *argument);

/* private variable define*/
 
#define  priI2C_NewCommand_2 priI2Cx_2_NewCommand
