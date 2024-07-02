/**
 * \file            extChCmd.cpp
 * \brief           External module control channel.
 * \author          giancarlo.marcolin@gmail.com
 */

#include "cmdConfig.h"
#include "cmd.h"
#include "extChCmd.hpp"
#include "cmsis_os.h"

/*---------------------------------------------------------------------------*/

extern "C" [[noreturn]] void vExtChCmdTask( void* pArgument );

/*---------------------------------------------------------------------------*/

void vExtChCmdTask( void* pArgument )
{
    Cmd testCmd1{};

    Cmd testCmd2{ CmdType::cmd1, CmdType::cmd2 };

    while(1)
    {
        osDelay( pdMS_TO_TICKS( 10 ) );
    }
}
/*---------------------------------------------------------------------------*/
