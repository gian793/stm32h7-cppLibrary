/**
 * \file            mxLog.cpp
 * \brief           Usart log class.
 * \author          g.marcolin
 */

/*---------------------------------------------------------------------------*/

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <memory>
#include <iostream>
#include <string.h>
#include <cstdarg>

#include "fLog.h"
#include "mxLog.h"

/*---------------------------------------------------------------------------*/

/* c++14 constexpr function can do this in a simple way. */
#include <iostream>

/*---------------------------------------------------------------------------*/

#define FILENAME    ( strrchr( __FILE__, '/' ) ? strrchr( __FILE__, '/' ) + 1 : __FILE__ )

// #define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

/*---------------------------------------------------------------------------*/

constexpr char logHdr[]{"%%"};
constexpr size_t logHdr_SIZE = strlen( logHdr );

/*---------------------------------------------------------------------------*/

size_t mxLog::storeMsg( const fData_t* pfData, va_list args )
{
    size_t sz = 0;

    stm32_lock_acquire( &lock );

    /* Put header in ring buffer. */
    put( logHdr, logHdr_SIZE );

    /* Put fData in ring buffer. */
    put( reinterpret_cast<const char*>(pfData), sizeof( fData_t ) );

    if( pfData->pos != -1 )
    {
        const char *pcPar;

        switch( pfData->format )
        {
            case 's':{
                pcPar = va_arg(args, const char*);
                sz = strlen( pcPar ) + 1;
                put( pcPar, sz );}
                break;

            case 'u':
            case 'x':
            case 'X':{
                sz = sizeof( uint32_t );
                uint32_t tmp = va_arg(args, uint32_t);
                pcPar = reinterpret_cast<const char*>(&tmp);
                put( pcPar, sz );
                put( '\0' );}
                break;

            case 'd':{
                sz = sizeof( int32_t );
                int32_t tmp = va_arg(args, int32_t);
                pcPar = reinterpret_cast<const char*>(&tmp);
                put( pcPar, sz );
                put( '\0' );}
                break;

            case 'g':{
                sz = sizeof( double );
                double tmp = va_arg(args, double);
                pcPar = reinterpret_cast<const char*>(&tmp);
                put( pcPar, sz );
                put( '\0' );}
                break;

            default:
                break;
        }
    }

    stm32_lock_release( &lock );

    sz += ( logHdr_SIZE + sizeof( fData_t ) );

    return sz;
}
/**
  * @brief  Print out an Info message with parameter.
  * @param  Pointer to format string.
  *         Void pointer to parameter.
  * @retval Number of byte to be printed out.
  */

size_t mxLog::Info( const fLog formatLog, ... )
{
    std::va_list arg;

    va_start(arg, formatLog);

    size_t sz = storeMsg( &formatLog.fData, arg );

    va_end(arg);

    return sz;
}

/**
 * C wrapper
 */
extern "C" size_t mxLogInfo( mxLog* pLog, const char* format, ... )
{
    std::va_list arg;

    va_start(arg, format);

    const fData_t fData = { .pStr   = format,
                            .pos    = fPositionGet( format ),
                            .format = fFormatGet( format )  };

    size_t sz = pLog->storeMsg( &fData, arg );

    va_end(arg);

    return sz;
}


/**
  * @brief  Print out an Info message.
  * @param  Pointer to string.
  *         end character
  * @retval None.
  */

void mxLog::printStr( const char* pStr, const char endChar )
{
    if( pStr != nullptr )
    {
        while( *pStr != endChar )
        {
            if( *pStr == '%' )
            {
                HAL_UART_Transmit( pUsart, (const uint8_t*)"{} ", 3, 10 );
                ++pStr;
            }
            else
            {
                HAL_UART_Transmit( pUsart, (const uint8_t*)pStr, 1, 10 );
            }

            ++pStr;
        }
    }
}

/**
  * @brief  Print out the  parameter.
  * @param  Parameter format.
  * @retval None.
  */

#ifdef CPPUTEST
    #define UINT32_FORMAT               "%u"
    #define UINT32_HEX_FORMAT           "%x"
    #define UINT32_HEX_CAP_FORMAT       "%X"
    #define INT32_FORMAT                "%d"
    #define DOUBLE_FORMAT               "%g"
#else
    #define UINT32_FORMAT               "%lu"
    #define UINT32_HEX_FORMAT           "%lx"
    #define UINT32_HEX_CAP_FORMAT       "%lX"
    #define INT32_FORMAT                "%ld"
    #define DOUBLE_FORMAT               "%g"
#endif

void mxLog::printPar( const char format )
{
    constexpr size_t bufSIZE = 16;
    char outBuf[ bufSIZE ]{0};

    if( format == 's' )
    {
        char c = 0;

        while( ( c = get() ) != '\0' )
        {
            HAL_UART_Transmit( pUsart, reinterpret_cast<const uint8_t*>(&c), 1, 10 );
        }
    }
    else
    {
        uint32_t i = 0;
        char parBuf[ bufSIZE ]{0};

        switch( format )
        {
            case 'u':{
                size_t sz = sizeof(uint32_t) + 1;
                while( i <  sz )
                {
                    parBuf[ i++ ] = get();
                }

                uint32_t* pluPar = (uint32_t*)&parBuf[0];
                snprintf( outBuf, bufSIZE, UINT32_FORMAT, *static_cast<const uint32_t*>(pluPar) );}
                break;

            case 'x':{
                size_t sz = sizeof(uint32_t) + 1;
                while( i <  sz )
                {
                    parBuf[ i++ ] = get();
                }

                uint32_t* pluPar = (uint32_t*)&parBuf[0];
                snprintf( outBuf, bufSIZE, UINT32_HEX_FORMAT, *static_cast<const uint32_t*>(pluPar) );}
                break;

            case 'X':{
                size_t sz = sizeof(uint32_t) + 1;
                while( i <  sz )
                {
                    parBuf[ i++ ] = get();
                }

                uint32_t* pluPar = (uint32_t*)&parBuf[0];
                snprintf( outBuf, bufSIZE, UINT32_HEX_CAP_FORMAT, *static_cast<const uint32_t*>(pluPar) );}
                break;

            case 'd':{
                size_t sz = sizeof(int32_t) + 1;
                while( i <  sz )
                {
                    parBuf[ i++ ] = get();
                }

                int32_t* pdPar = (int32_t*)&parBuf[0];
                snprintf( outBuf, bufSIZE, INT32_FORMAT, *static_cast<const int32_t*>(pdPar) );}
                break;

            case 'g':{
                size_t sz = sizeof(double) + 1;
                while( i <  sz )
                {
                    parBuf[ i++ ] = get();
                }
                double* pgPar = (double*)&parBuf[0];
                snprintf( outBuf, bufSIZE, DOUBLE_FORMAT, *static_cast<const double*>(pgPar) );}
                break;

            default:
                break;
        }

        if( ( i > 0 ) && ( i < bufSIZE ) )
        {
            i = 0;

            while( ( outBuf[ i ] != '\0' ) && ( i < bufSIZE) )
            {
                HAL_UART_Transmit( pUsart, reinterpret_cast<const uint8_t*>(&outBuf[ i ]), 1, 10 );

                ++i;
            }
        }
    }
}

/**
  * @brief  Get the next message header.
  * @param  None.
  * @retval True if header is found.
  */

bool mxLog::getHeader( void )
{
    uint32_t hdrIdx = 0;

    /* Look for header pattern until there are chars in the buffer. */
    while( !isEmpty() && ( hdrIdx != logHdr_SIZE ) )
    {
        if( get() == logHdr[ hdrIdx ] )
        {
            ++hdrIdx;
        }
        else
        {
            hdrIdx = 0;
        }
    }

    if( hdrIdx != logHdr_SIZE )
    {
        hdrIdx = 0;
    }

    return static_cast<bool>(hdrIdx);
}

/**
  * @brief  Print out an Info message.
  * @param  None.
  * @retval None.
  */

void mxLog::printOut( void )
{
    stm32_lock_acquire( &lock );

    if( getHeader() )
    {
        /* Re-compose fData. */
        fData_t fData{0};

        char* p = reinterpret_cast<char*>(&fData);

        size_t sz = sizeof( fData_t );

        while( sz-- )
        {
            *p = get();
            ++p;
        }

        /* When no parameter present -> print-out string. */
        if(    ( fData.pos == -1 ) \
            || ( fData.format == '0' ) \
            || ( isEmpty() == true )   )
        {
            printStr( fData.pStr, '\0' );
        }
        else
        {
            /* Print the first part of the format string. */
            printStr( fData.pStr, '%' );

            /* Print the parameter part. */
            printPar( fData.format );

            /* Print the second part of the format string. */
            printStr( fData.pStr + fData.pos + 2, '\0' );
        }
    }

    stm32_lock_release( &lock );
}


/**
  * @brief  Print out manager. Must be continuously called in main loop or placed in OS task.
  * @param  None.
  * @retval None.
  */

void mxLog::taskManager( void )
{
    while( !isEmpty() )
    {
        printOut();
    }
}

/*---------------------------------------------------------------------------*/
