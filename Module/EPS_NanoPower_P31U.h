#include "stm32f4xx_hal.h"

extern uint8_t priI2C_NewCommand_2;

void submain_EPS(I2C_HandleTypeDef *argument);

/*for EPS NanoPower_P31U*/

/**
The housekeeping format
**/
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

/* private variable define*/
 
#define  priI2C_NewCommand_2 priI2Cx_2_NewCommand
