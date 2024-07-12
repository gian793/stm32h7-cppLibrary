/**
 * \file            mxLog.h
 * \brief           Usart log class.
 * \author          g.marcolin
 */

#ifndef MX_LOG_H_
#define MX_LOG_H_

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#include "stm32h7xx.h"
#include "stm32h7xx_hal_uart.h"
#include "stm32_lock.h"
#include "fLog.h"
#include "ringBuffer.h"

/*---------------------------------------------------------------------------*/

/* Functions definition in header file to avoid inline. */

/*---------------------------------------------------------------------------*/

class mxLog : public ringBuffer<char> {

    public:

        explicit mxLog( char* pBuf, const size_t bufSize, UART_HandleTypeDef& usartHandle ) : ringBuffer{ pBuf, bufSize }, pUsart{ &usartHandle } {}

        // Disable copy/move
        mxLog( const mxLog& )            = delete;
        mxLog( mxLog&& )                 = delete;
        mxLog& operator=( const mxLog& ) = delete;
        mxLog& operator=( mxLog&& )      = delete;

		size_t Info( const fLog formatLog, ... );
        
//        size_t Debug  ( const char* format, ... );
//        size_t Warn ( const char* format, ... );
//        size_t Error( const char* format, ... );

        size_t storeMsg( const fData_t* pfData, va_list args );

        void taskManager( void );

    private:
        UART_HandleTypeDef* pUsart;

        size_t putPar( const char format, const char* par );

        size_t getParSize( const char format, const char* par );

        bool getHeader( void );

        void printOut( void );
        void printStr( const char* pStr, const char endChar );
        void printPar( const char format );
};

#else

typedef struct mxLog mxLog;

size_t mxLogInfo( mxLog* pLog, const char* format, ... );

#endif

/*---------------------------------------------------------------------------*/

#endif /* MX_LOG_H_ */
