#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "stm32h7xx_hal_uart.h"

char uartPrintOutBuf[ UART_PRINT_OUT_BUF_SIZE ] = {0};
uint32_t outBufIndex = 0;

void Hal_Uart_Reset( void )
{
    memset( uartPrintOutBuf, 0, UART_PRINT_OUT_BUF_SIZE );

    outBufIndex = 0;
}

const char* mock_GetBuffer( void )
{
    return (const char*)uartPrintOutBuf;
}

/**
  * @brief Fake for HAL_UART_Transmit
  */

HAL_StatusTypeDef HAL_UART_Transmit( UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout )
{
    HAL_StatusTypeDef res = HAL_OK;

    uint8_t* pDst = (uint8_t*)&uartPrintOutBuf[ outBufIndex ];

    if( ( outBufIndex + Size ) <= UART_PRINT_OUT_BUF_SIZE )
    {
        outBufIndex += Size;

        while( Size-- > 0 )
        {
            *pDst = *pData;

            ++pDst;
            ++pData;
        }
    }
    else
    {
        res = HAL_ERROR;
    }

    return res;
}
