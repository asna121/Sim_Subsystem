/**
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_ThreadCreation/Inc/main.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    14-August-2015 
  * @brief   This file contains all the functions prototypes for the main.c 
  *          file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
//#include "stm32f429i_discovery.h"
	 
/* FatFs includes component */
//#include "ff_gen_drv.h"
//#include "usbh_diskio.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/ 
/* User can use this section to tailor USARTx/UARTx instance used and associated 
   resources */
/* Definition for USARTx clock resources */
#define USARTx                           USART3
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE() 

#define USARTx_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_8
#define USARTx_TX_GPIO_PORT              GPIOD  
#define USARTx_TX_AF                     GPIO_AF7_USART3
#define USARTx_RX_PIN                    GPIO_PIN_9
#define USARTx_RX_GPIO_PORT              GPIOD 
#define USARTx_RX_AF                     GPIO_AF7_USART3

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART3_IRQn
#define USARTx_IRQHandler                USART3_IRQHandler

/* Size of Transmission buffer */
#define TXBUFFERSIZE                     (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                     TXBUFFERSIZE

/* User can use this section to tailor I2Cx/I2Cx instance used and associated
   resources */
/* Definition for I2Cx clock resources */
#define I2Cx_1                             I2C1
#define I2Cx_1_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_1_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_1_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2Cx_1_FORCE_RESET()               __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_1_RELEASE_RESET()             __HAL_RCC_I2C1_RELEASE_RESET()

 #define I2Cx_2                             I2C2
 #define I2Cx_2_CLK_ENABLE()                __HAL_RCC_I2C2_CLK_ENABLE()
 #define I2Cx_2_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()
 #define I2Cx_2_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()

 #define I2Cx_2_FORCE_RESET()               __HAL_RCC_I2C2_FORCE_RESET()
 #define I2Cx_2_RELEASE_RESET()             __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_1_SCL_PIN                    GPIO_PIN_6
#define I2Cx_1_SCL_GPIO_PORT              GPIOB
#define I2Cx_1_SCL_AF                     GPIO_AF4_I2C1
#define I2Cx_1_SDA_PIN                    GPIO_PIN_7
#define I2Cx_1_SDA_GPIO_PORT              GPIOB
#define I2Cx_1_SDA_AF                     GPIO_AF4_I2C1

 #define I2Cx_2_SCL_PIN                    GPIO_PIN_1
 #define I2Cx_2_SCL_GPIO_PORT              GPIOF
 #define I2Cx_2_SCL_AF                     GPIO_AF4_I2C2
 #define I2Cx_2_SDA_PIN                    GPIO_PIN_0
 #define I2Cx_2_SDA_GPIO_PORT              GPIOF
 #define I2Cx_2_SDA_AF                     GPIO_AF4_I2C2

/* Definition for I2Cx's NVIC */
 #define I2Cx_1_EV_IRQn                    I2C1_EV_IRQn
 #define I2Cx_1_EV_IRQHandler              I2C1_EV_IRQHandler
 #define I2Cx_1_ER_IRQn                    I2C1_ER_IRQn
 #define I2Cx_1_ER_IRQHandler              I2C1_ER_IRQHandler

 #define I2Cx_2_EV_IRQn                    I2C2_EV_IRQn
 #define I2Cx_2_EV_IRQHandler              I2C2_EV_IRQHandler
 #define I2Cx_2_ER_IRQn                    I2C2_ER_IRQn
 #define I2Cx_2_ER_IRQHandler              I2C2_ER_IRQHandler
 
/* Definition for I2Cx's Interrupt Priority */
 #define I2Cx_1_EV_Priority										11
 #define I2Cx_1_ER_Priority										10

 #define I2Cx_2_EV_Priority										11
 #define I2Cx_2_ER_Priority										10

/* Definition for I2Cx's ADDRESS */
#define I2C_1_ADDRESS        0x3E

#define I2C_2_ADDRESS        0x04

/* private variable define*/

 #define priI2Cx_1_NewCommand priI2C_NewCommand_1
 
 #define priI2Cx_2_NewCommand priI2C_NewCommand_2



/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
	
/* Exported functions ------------------------------------------------------- */


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

