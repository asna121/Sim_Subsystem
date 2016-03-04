#include "stm32f4xx_hal.h"

extern uint8_t priI2C_NewCommand_2;

void submain_IFB(I2C_HandleTypeDef *argument);

/*for INMS Interface board*/

/**
On board status and INMS status
**/
#pragma pack(push, 1)
typedef struct IFB_CURRENT_TEMPERATURE{
	uint16_t IFB_5V_Current;		    //5V current [mA]
	uint16_t IFB_3_3V_Current;		    //3_3V current [mA]
	uint16_t IFB_INMS_Temp;		    //INMS_Temperature [degree Celsius]
	uint16_t IFB_IFB_Temp;			//IFB_Temperature [degree Celsius]
} ifb_current_temperature;
#pragma pack(pop)


/* private variable define*/
 
#define  priI2C_NewCommand_2 priI2Cx_2_NewCommand

#undef I2Cx_2_ADDRESS
#define I2Cx_2_ADDRESS (0x7D<<1)
