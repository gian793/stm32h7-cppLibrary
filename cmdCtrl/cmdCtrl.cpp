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

bool cmdCtrl::Manager( void )
{
    stm32_lock_acquire( &cmdLock );

    /* Commands Tx. */
    auto idx = txCnt;

    while( idx > 0 )
    {
        --idx;
        txBuffer[ idx ].execute(); 
    }

    /* Commands Rx. */
    idx = rxCnt;
    
    while( idx > 0 )
    {
        --idx;
        //isCmdHandled = rxCmdManager( rxBuffer[ idx ] ); 
    }

    stm32_lock_release( &cmdLock );

    return true;
}



/**
  * @brief  Transmit a command.
  * @param  Command to be transmitted.
  * @retval True if command added successfully to tx buffer.
  */

bool cmdCtrl::Tx( Cmd &xCmd )
{
    bool isCmdAdded = false;

    stm32_lock_acquire( &cmdLock );

    if( txCnt < txBuffer.max_size() ) 
    {
        xCmd.init( ++nextToken );

        txBuffer[ txCnt++ ] = xCmd;

        /* High priority cmds first. */
        std::sort( txBuffer.begin(), txBuffer.begin() + txCnt, Cmd::prioritySmallerEqual );

        isCmdAdded = true;
    }      

    stm32_lock_release( &cmdLock );

    return isCmdAdded;
}



/**
  * @brief  Receive a command.
  * @param  Command received.
  * @retval True if command added successfully to rx buffer.
  */

bool cmdCtrl::Rx( Cmd &xCmd )
{
    bool isCmdAdded = false;

    stm32_lock_acquire( &cmdLock );

    if( rxCnt < rxBuffer.max_size() ) 
    {
        /* Token already set by the sender. */
        rxBuffer[ rxCnt++ ] = xCmd; 

        /* High priority cmds first. */
        std::sort( rxBuffer.begin(), rxBuffer.begin() + rxCnt, Cmd::prioritySmallerEqual );

        isCmdAdded = true;
    }

    stm32_lock_release( &cmdLock );        

    return isCmdAdded;
}
