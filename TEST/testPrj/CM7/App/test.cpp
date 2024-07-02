////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------------------------------------
// FILE: test.cpp
//
// PROJECT:
//    TestPrjH755mx_CM7
//
// DESCRIPTION:
//    ...
//
// AUTHOR:
//    giancarlo.marcolin@gmail.com
//----------------------------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <array>
#include <complex>
#include <ctime>
#include <concepts>
#include <mutex>
#include <memory>
#include <cstddef>
#include <cstring>

#include "cmsis_os.h"
#include "fLog.h"
#include "mxLog.h"
#include "logM7.h"
#include "test.hpp"

/*---------------------------------------------------------------------------*/

extern "C" [[noreturn]] void vCppTestTask( void* pArgument );

/*---------------------------------------------------------------------------*/

//extern mxLog M7LOG;

/*---------------------------------------------------------------------------*/

static bool hbTrigger = false;

float flIdx = 0;

/*---------------------------------------------------------------------------*/

void vCppTestTask( void* pArgument )
{
    while(1)
    {
        if( hbTrigger )
        {
            //hbTrigger = false;

            ++flIdx;

            M7LOG.Info( "\n\r", nullptr );
            M7LOG.Info( "Heartbeat ");
            M7LOG.Info( "idx: %f", flIdx );

            osDelay( pdMS_TO_TICKS( 530 ) );
        }

        osDelay( pdMS_TO_TICKS( 50 ) );
    }
}

/*---------------------------------------------------------------------------*/
