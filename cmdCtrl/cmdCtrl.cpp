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

    /* Commands Tx. */
    auto idx = txCnt;

    while( idx > 0 )
    {
        --idx;
        cmdBuffer[ idx ].execute(); 
    }

    stm32_lock_release( &cmdLock );

    return true;
}


/**
  * @brief  Transmit a command.
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
                    uint32_t  cmdDelayMs )
{
    bool isCmdAdded = false;

    stm32_lock_acquire( &cmdLock );

    auto idx = txCnt;

    while( idx > 0 )
    {
        --idx;
    
        /* Is this a reply? */
        if( cmdBuffer[ idx ].type  == cmdType && 
            cmdBuffer[ idx ].token == cmdToken)
        {
            cmdBuffer[ idx ].replied();
        }
    }

    if( txCnt < cmdBuffer.max_size() ) 
    {
        uint32_t token = cmdToken == 0 ?  ++nextToken : cmdToken;

        cmdBuffer[ txCnt++ ].set( cmdType, pCmdObj, token, cmdReplyType, cmdPrioLevel, cmdRetryNr, cmdTimeoutMs, cmdPeriodMs, cmdDelayMs );

        /* Low priority cmds first. */
        std::sort( cmdBuffer.begin(), cmdBuffer.begin() + txCnt, Cmd::prioritySmallerEqual );

        isCmdAdded = true;
    }      

    stm32_lock_release( &cmdLock );

    return isCmdAdded;
}