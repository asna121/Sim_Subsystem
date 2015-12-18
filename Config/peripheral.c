#include "peripheral.h"
#include "common.h"
#include "main.h"


/* UART handler declaration */
UART_HandleTypeDef UartHandle;

/* I2C handler declaration */
I2C_HandleTypeDef I2CxHandle_2;


/* Receive New CMD flag */
uint8_t priI2Cx_2_NewCommand =0;

void Init_Peripheral(void)
{
/*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART1 configured as follow:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance          = USARTx;
  
  UartHandle.Init.BaudRate     = 9600;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	
	if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    Error_Handler();
  }

 /*##-1- Configure the I2C peripheral #######################################*/
  I2CxHandle_2.Instance             = I2Cx_2;
  I2CxHandle_2.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  I2CxHandle_2.Init.ClockSpeed      = 400000;
  I2CxHandle_2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2CxHandle_2.Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
  I2CxHandle_2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2CxHandle_2.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
  I2CxHandle_2.Init.OwnAddress1     = I2C_2_ADDRESS;
  I2CxHandle_2.Init.OwnAddress2     = 0;
	
	if(HAL_I2C_Init(&I2CxHandle_2) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
}


/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Turn LED4 on: Transfer error in reception/transmission process */
//  BSP_LED_On(LED4); 
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
/**
  * @brief  I2C error callbacks.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{

}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	
	if(hi2c == &I2CxHandle_2)
		priI2Cx_2_NewCommand =1;
    
//	if(hi2c == &I2CxHandle_1)
//		priI2Cx_1_NewCommand =1;
    
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{

}
