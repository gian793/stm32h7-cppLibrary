/**
 * \file            cmdCtrl.cpp
 * \brief           Controller for sending and receiving commands asynchronously.
 * \author          giancarlo.marcolin@gmail.com
 */

#include <algorithm>
#include <array>
#include <cstdint>

#include "stm32_lock.h"
#include "cmdConfig.h"
#include "cmd.h"
#include "cmdCtrl.h"


/**
  * @brief  Commands handler. Must be invoked periodically.
  * @param  None.
  * @retval True if any command is handled.
  */

bool cmdCtrl::manager( void )
{
    stm32_lock_acquire( &cmdLock );

    auto i = cnt;

    while( --i >= 0 )
    {
        auto idx = prioBuffer[ i ];

        if( cmdBuffer[ idx ].execute() == CmdState::Done )
        {
            cmdBuffer[ idx ].reset();

            freeIndex( i ); 
        }
    }

    stm32_lock_release( &cmdLock );

    return true;
}


/**
  * @brief  Load a command to be executed.
  * @param  Command to be transmitted.
  * @retval True if command added successfully to tx buffer.
  */

bool cmdCtrl::load( CmdType   cmdType, 
                    CmdObj*   pCmdObj, 
                    uint32_t  cmdToken,
                    CmdType   cmdReplyType,
                    PrioLevel cmdPrioLevel, 
                    uint32_t  cmdRetryNr, 
                    uint32_t  cmdTimeoutMs, 
                    uint32_t  cmdPeriodMs, 
                    uint32_t  cmdDelayMs,
                    bool      isReply )
{
    bool isCmdAdded = false;

    stm32_lock_acquire( &cmdLock );

    /* Is there still place in the buffer? */
    if( cnt < cmdBuffer.max_size() ) 
    {                
        /* Get next free index in the command array. */
        auto idx = idxBuffer[ cnt ];

        uint32_t token = cmdToken == 0 ? ++nextToken : cmdToken;

        cmdBuffer[ idx ].set( cmdType, pCmdObj, token, cmdReplyType, cmdPrioLevel, cmdRetryNr, cmdTimeoutMs, cmdPeriodMs, cmdDelayMs );

        prioBuffer[ cnt++ ] = idx;

        /* Low priority cmds first. */
        for( auto i = cnt - 1; i > 0; --i )
        {
            if( cmdBuffer[ prioBuffer[ i ] ].priority > cmdBuffer[ prioBuffer[ i - 1 ] ].priority )
            {
                break;    /* Exit. */
            }
            else
            {
                auto tmpIdx = prioBuffer[ i ];

                prioBuffer[ i ] = prioBuffer[ i - 1 ];

                prioBuffer[ i - 1 ] = tmpIdx;
            }
        }

        if( isReply )
        {
            for( Cmd &cmd: cmdBuffer ) 
            { 
                if( ( cmd.token == cmdToken ) && ( cmd.replyType ==  cmdType ) )
                {
                    cmd.replied();

                    break;
                }
            }
        }

        isCmdAdded = true;           
    } 

    stm32_lock_release( &cmdLock );

    return isCmdAdded;
}