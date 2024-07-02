////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------------------------------------
// FILE: cSimpleTest.c
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

#ifdef __cplusplus
#undef __cplusplus
#endif

#include <stdbool.h>

#include "cmsis_os.h"

#include "mxLog.h"
#include "logM7.h"

#include "cSimpleTest.h"

/*---------------------------------------------------------------------------*/

extern void vCsimpleTask( void* pArgument );

/*---------------------------------------------------------------------------*/

//static bool hbTrigger = false;
//float flIdx = 0;

static struct mxLog* pM7LOG;

static bool ctestTrigger = false;

uint32_t cIdx = 0;

/*---------------------------------------------------------------------------*/

void vCsimpleTask( void* pArgument )
{
    pM7LOG = &M7LOG;

    mxLogInfo( pM7LOG, "C TEST start\n\r" );

    while(1)
    {
        if( ctestTrigger )
        {
            //ctestTrigger = false;

            ++cIdx;

            //mxLogInfo( pM7LOG, "\n\r");
            mxLogInfo( pM7LOG, "\n\rCtest counter: %u", cIdx );

            osDelay( pdMS_TO_TICKS( 370 ) );
        }

        osDelay( pdMS_TO_TICKS( 10 ) );
    }
}

/*---------------------------------------------------------------------------*/
