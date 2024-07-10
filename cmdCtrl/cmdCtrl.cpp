/**
 * \file            cmdCtrl.cpp
 * \brief           Controller for sending and receiving cmmands asynchronously.
 * \author          giancarlo.marcolin@gmail.com
 */

#include <algorithm>
#include <array>

#include "cmdConfig.h"
#include "cmd.h"
#include "cmdCtrl.h"



/**
  * @brief  Commands handler. Must be invoked periodically.
  * @param  None.
  * @retval None.
  */

void cmdCtrl::Manager( void )
{
    /* Commands Tx. */
    if( txCnt > 0 )
    {

    }

    /* Commands Rx. */
    if( rxCnt > 0 )
    {
        
    }
}



/**
  * @brief  Transmit a command.
  * @param  Command to be transmitted.
  * @retval True if command added successfully to tx buffer.
  */

bool cmdCtrl::Tx( Cmd &xCmd )
{
    bool isTxCmdAdded = false;

    /* TODO Add mutex for exclusive access to shared data. */

    if( txCnt < txBuffer.max_size() ) 
    {
        xCmd.setToken( ++nextToken );

        txBuffer[ txCnt++ ] = xCmd;

        /* High priority cmds first. */
        std::sort( txBuffer.begin(), txBuffer.begin() + txCnt, Cmd::prioritySmallerEqual );

        isTxCmdAdded = true;
    }      

    return isTxCmdAdded;
}



/**
  * @brief  Receive a command.
  * @param  Command received.
  * @retval True if command added successfully to rx buffer.
  */

bool cmdCtrl::Rx( Cmd &xCmd )
{
    bool isRxCmdAdded = false;

    /* TODO Add mutex for exclusive access to shared data. */

    if( rxCnt < rxBuffer.max_size() ) 
    {
        rxBuffer[ rxCnt++ ] = xCmd; 

        /* High priority cmds first. */
        std::sort( rxBuffer.begin(), rxBuffer.begin() + rxCnt, Cmd::prioritySmallerEqual );

        isRxCmdAdded = true;
    }        

    return isRxCmdAdded;
}
