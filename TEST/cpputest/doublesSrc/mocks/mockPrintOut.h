#ifndef MOCK_PRINT_OUT_H
#define  MOCK_PRINT_OUT_H

#include "stm32h7xx_hal_uart.h"

#ifdef __cplusplus

#include <string.h>

class mockPrintOut
{
    public:
		mockPrintOut( char* pBuf, const size_t size ) : buf{ pBuf }, maxSize{ size }
        {}

        void reset( void )
        {
            Hal_Uart_Reset();
        }

        bool check( const char* pStr )
        {
            return ( strncmp( pStr, buf, maxSize ) == 0 );
        }

        char* getBuf( void )
        {
            return buf;
        }

    private:
		char* buf;
		
        const size_t maxSize;
};

#endif // __cplusplus

#endif // MOCK_PRINT_OUT_H