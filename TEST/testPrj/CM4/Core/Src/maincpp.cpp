////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------------------------------------
// FILE: maincpp.cpp
//
// PROJECT:
//    TestPrjH755mx_CM4
//
// DESCRIPTION:
//    ...
//
// AUTHOR:
//    g.marcolin@inventronicsglobal.com
//----------------------------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <complex>
#include <ctime>
#include <concepts>
#include <mutex>
#include <memory>
#include <cstddef>
#include <cstring>

#include "utils.h"
#include "fLog.h"
#include "mxLog.h"
#include "main.h"
#include "maincpp.h"

/*---------------------------------------------------------------------------*/

extern "C" [[noreturn]] void mainCpp( void );

/*---------------------------------------------------------------------------*/

extern UART_HandleTypeDef huart7;

/*---------------------------------------------------------------------------*/

static constexpr uint32_t ledToggle_PERIOD_MS = 1000;

//static constexpr uint32_t txBuf_SIZE = 80;

static constexpr time_t todayUnix_TIME  = 1697950558;

/*---------------------------------------------------------------------------*/

static constexpr size_t logBuffer_SIZE = 128;
static char logBuffer[ logBuffer_SIZE ] = {0};

//static constexpr uint32_t intBuf_SIZE = 32;
//static uint32_t intBuf[ intBuf_SIZE ] = {0};

/*---------------------------------------------------------------------------*/

//static void timeTest( void )
//{
//    char txBuf[ txBuf_SIZE ] = {0};
//    static time_t time = todayUnix_TIME;
//
//    ++time;
//
//    struct tm* pTimeInfo = localtime (&time);
//
//    strftime( txBuf, sizeof( txBuf ), "\n\rTime -> %a %Y-%m-%d %H:%M:%S", pTimeInfo );
//
//    HAL_UART_Transmit( &huart7 , ( const uint8_t* )txBuf, txBuf_SIZE, 100 );
//}

/*---------------------------------------------------------------------------*/

//static uint32_t array[] = { 0, 1, 2, 3, 4, 5 };
//static uint32_t arraySum = 0;

//static const uint32_t a1 = 10;
//static const uint32_t a2 = 5;
//
//int i1{2500};
//int i2{2000};
//
//std::complex<double> cd1(10.0, 2.0);
//[[deprecated("do not use")]] std::complex<double> cd2(3.7, 4);

//static void arrayTest( uint32_t type )
//{
//    [[maybe_unused]] uint32_t i = 0;
//
//    arraySum = sum( array );
//
//    if(  ( arraySum <= 15 )
//        && isGreaterThan( i1, i2 )
//     /*   && isGreaterThan( cd1, cd2 )  */  ) /* -> Error: complex numbers do not have '>' operator. */
//    {
//        switch( type )
//        {
//            //[[fallthrough]]
//            default:
//                type = 2;
//
//            case 0:
//                break;
//
//            case 1:
//                break;
//        }
//
//        array[0] = getMax( a1, a2 );
//    }
//    else [[likely]]
//    {
//        type = 10;
//    }
//}

/*---------------------------------------------------------------------------*/


[[noreturn]] void mainCpp()
{
    static uint32_t prevTick = 0;

    /* mxLog constructor. */
    mxLog M4LOG{ logBuffer, logBuffer_SIZE, huart7 };

    const char srcText[] = "\n\rThis is my first TEXT printed out!";

    M4LOG.put( srcText, strlen(srcText) );

    while(1)
    {
        uint32_t tick = HAL_GetTick();

        if( isGreaterThan( ( tick - prevTick ), ledToggle_PERIOD_MS ) )
        {
            prevTick = tick;

            HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin );

            //timeTest();

            //arrayTest( tick );

            /*----------------------------------*/
            static time_t time = todayUnix_TIME;

            ++time;

            struct tm* pTimeInfo = localtime (&time);

            char timeBuf[64] = {0};

            strftime( timeBuf, sizeof( timeBuf ), "\n\rTime -> %a %Y-%m-%d %H:%M:%S", pTimeInfo );

//            M4LOG.put( timeBuf, strlen(timeBuf) );

//            M4LOG.dbgPrint( timeBuf );
            /*----------------------------------*/
        }

//        M4LOG.taskManager();

//        if( !M4LOG.isEmpty() )
//        {
//            char c = M4LOG.get();
//            HAL_UART_Transmit( &huart7 , (const uint8_t*)&c, 1, 10 );
//        }
    }
}

/*---------------------------------------------------------------------------*/
