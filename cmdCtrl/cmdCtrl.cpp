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

#include <iostream>
#include <cstring>

uint32_t cmdCtrl::manager( void )
{
    uint32_t isCmdDoneCnt = 0;

    stm32_lock_acquire( &cmdLock );

    auto i = cnt;
    
    while( i > 0 )
    {
        auto idx = prioBuffer[ --i ];

        CmdState cmdState = cmdBuffer[ idx ].execute();

        if( cmdState == CmdState::Done )
        {
            ++isCmdDoneCnt;

            if(    ( cmdBuffer[ idx ].getPeriod() == 0 ) && 
                 ( ( cmdBuffer[ idx ].getOptions() & CmdOption::RepeatOnReply ) == false ) &&  
                 ( ( cmdBuffer[ idx ].getOptions() & CmdOption::RepeatForever ) == false )   )
            {
                removeCmdByIdx( idx );
            }
        }
        else if( cmdState == CmdState::Timeout )
        {
            if(    ( cmdBuffer[ idx ].getPeriod() == 0 ) && 
                   ( cmdBuffer[ idx ].isRetryDone() == true ) && 
                 ( ( cmdBuffer[ idx ].getOptions() & CmdOption::RepeatOnTimeout ) == false ) &&  
                 ( ( cmdBuffer[ idx ].getOptions() & CmdOption::RepeatForever )   == false )   )
            {
                removeCmdByIdx( idx );
            }
        }
    }

    stm32_lock_release( &cmdLock );

    return isCmdDoneCnt;
}


/**
  * @brief  Load a command to be executed.
  * @param  pCmdObj - Command object, if needed. Can be set to nullptr.
  *         cmdType - Type of command.
  * @retval True if command added successfully to tx buffer.
  */

bool cmdCtrl::loadCmd(  CmdObj*   pCmdObj, 
                        CmdType   cmdType, 
                        CmdType   cmdReplyType, 
                        PrioLevel cmdPrioLevel,
                        uint32_t  cmdToken,
                        uint32_t  cmdRetryNr, 
                        uint32_t  cmdTimeoutMs, 
                        uint32_t  cmdPeriodMs, 
                        uint32_t  cmdDelayMs,
                        CmdOption cmdOptions  )
{
    bool isCmdAdded = false;

    stm32_lock_acquire( &cmdLock );

    /* Is there still place in the buffer? */
    if( ( cmdType != CmdType::noCmd ) && ( cnt < cmdBuffer.max_size() ) ) 
    {                
        if( cmdToken == 0 )
        {
            cmdToken = getNewToken();
        }
                
        /* Get next free index in the command array. */
        auto idx = idxBuffer[ cnt ];

        cmdBuffer[ idx ].set( cmdType, pCmdObj, cmdPrioLevel, cmdReplyType, cmdToken, cmdRetryNr, cmdTimeoutMs, cmdPeriodMs, cmdDelayMs, cmdOptions );

        prioBuffer[ cnt++ ] = idx;

        /* Re-sort prioBuffer, low priority cmds first. */
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

        isCmdAdded = true;           
    } 

    stm32_lock_release( &cmdLock );

    return isCmdAdded;
}

/**
  * @brief  Load a reply to be executed.
  * @param  Command to be transmitted.
  * @retval True if command added successfully to tx buffer.
  */

bool cmdCtrl::loadReply(    CmdObj*   pCmdObj, 
                            CmdType   cmdType, 
                            CmdType   cmdReplyType, 
                            PrioLevel cmdPrioLevel,
                            uint32_t  cmdToken,
                            uint32_t  cmdRetryNr, 
                            uint32_t  cmdTimeoutMs, 
                            uint32_t  cmdPeriodMs, 
                            uint32_t  cmdDelayMs )
    {

    bool isReplyAdded = false;

    if( this->loadCmd( pCmdObj, cmdType, cmdReplyType, cmdPrioLevel, cmdToken, cmdRetryNr, cmdTimeoutMs, cmdPeriodMs, cmdDelayMs ) )
    {
        for( Cmd &cmd: cmdBuffer ) 
        { 
            if( ( cmd.token == cmdToken ) && ( cmd.replyType ==  cmdType ) )
            {
                cmd.replied();

                break;
            }
        }

        isReplyAdded = true;           
    } 

    return isReplyAdded;
}

/** @brief  Remove the command with specified token. If command and reply are both in the buffer (with same token), both are removed.
  * @param  Token of the command to remove.
  * @retval True if the command has been (found) and removed.
  */

bool cmdCtrl::removeCmdByToken( uint32_t token )
{
    bool isCmdRemoved = false;

    uint32_t i = cnt;

    while ( i > 0 )
    {
        auto idx = idxBuffer[ --i ];

        if( cmdBuffer[ idx ].token == token )
        {
            cmdBuffer[ idx ].reset();
            
            isCmdRemoved = freeIndex( idx );
        }
    }

    return isCmdRemoved;
}

/** @brief  Remove the command with specified index.
  * @param  Index of the command to remove.
  * @retval True if the command has been (found) and removed.
  */
bool cmdCtrl::removeCmdByIdx( uint32_t idx )
{
    bool isCmdRemoved = false;

    if( freeIndex( idx ) )
    {
        cmdBuffer[ idx ].reset();

        isCmdRemoved = true;
    }

    return isCmdRemoved;
}