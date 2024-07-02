#ifndef STM32H7xx_HAL_UART_H
#define STM32H7xx_HAL_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

#define UART_PRINT_OUT_BUF_SIZE         ( 1024 )

extern char uartPrintOutBuf[ ];

typedef enum 
{
  USART1,
  USART2,
  USART3,
  UART4,
  UART5,
  USART6,
  UART7,
  UART8,

  usart_Max
}usart_t;

typedef struct __UART_HandleTypeDef
{
    usart_t usart;

} UART_HandleTypeDef;

void Hal_Uart_Reset( void );
const char* mock_GetBuffer( void );

// HAL_StatusTypeDef 
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout);

#ifdef __cplusplus
}
#endif

#endif /* STM32H7xx_HAL_UART_H */
