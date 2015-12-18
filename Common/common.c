#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"

#include "common.h"



/* UART Semaphore*/
xSemaphoreHandle uart_lock = NULL;

extern UART_HandleTypeDef UartHandle;

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
