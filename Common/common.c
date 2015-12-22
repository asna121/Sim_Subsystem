#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"

#include "common.h"
#include "peripheral.h"



/* UART Semaphore*/
xSemaphoreHandle uart_lock = NULL;

/* Private function prototypes -----------------------------------------------*/
void prvNewPrintString (const uint8_t *pcString, const uint8_t size);


void Init_common(void)
{
  /*Mutex */
  uart_lock = xSemaphoreCreateMutex();
}



void prvNewPrintString (const uint8_t *pcString, const uint8_t size)
{
    xSemaphoreTake(uart_lock, 10 * configTICK_RATE_HZ);
    HAL_UART_Transmit(&UartHandle, (uint8_t *)pcString, size, 1000);
    xSemaphoreGive(uart_lock);
}

void Error_Handler(void)
{
  /* Turn LED4 on */
 // BSP_LED_On(LED4);
  while(1)
		prvNewPrintString(" #@Error@# ",12);
}
