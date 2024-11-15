/**
 * \file            cmd.cpp
 * \brief           Commands class.
 * \author          giancarlo.marcolin@gmail.com
 */

// #include <algorithm>
// #include <array>
#include <cstdint>
#include <iostream>
#include <cstring>

// #include "stm32_lock.h"
#include "stm32h7xx_hal.h"
#include "cmdConfig.h"
#include "cmd.h"

/**
  * @brief  Commands executor. Must be invoked periodically.
  * @param  None.
  * @retval Command state.
  */

CmdState Cmd::execute( void )
{
    if( type != CmdType::noCmd )
    {
        auto timerMs = HAL_GetTick();

        switch( state )
        {
            case CmdState::Idle:
                if( ( timerMs - delayTimerMs ) >= delayMs )
                {
                    if( periodMs > 0 )
                    {   /* Compensation. */
                        auto deltaMs = ( timerMs - delayTimerMs ) - delayMs;

                        delayMs = ( periodMs > deltaMs ) ? ( periodMs - deltaMs ) : 0 ;
                    }

                    delayTimerMs   = timerMs;
                    timeoutTimerMs = timerMs;

                    pObj->send();

                    state = ( replyType != CmdType::noCmd ) ? CmdState::Sent : CmdState::Done;
                }
                break; 
            
            case CmdState::Sent:
                state = CmdState::WaitForReply;

            case CmdState::WaitForReply:                
                if( isReplied )
                {
                    pObj->reply();

                    state = CmdState::Done;
                }
                else if( ( timerMs - timeoutTimerMs ) >= timeoutMs )
                {
                    pObj->timeout();

                    state = CmdState::Timeout;
                }
                break;

            case CmdState::Timeout:
                if( ( options & CmdOption::RepeatOnTimeout ) || 
                    ( retryCnt++ < retryNr) ) 
                {
                    state = CmdState::Idle;
                }

            case CmdState::Done:
                if( ( periodMs > 0 ) ||
                    ( isReplied && ( options & CmdOption::RepeatOnReply ) ) || 
                    ( options & CmdOption::RepeatForever ) )
                {
                    isReplied = false;

                    state = CmdState::Idle;
                }
                break;

            default:
                break;
        }
    }

    return state;
};


/**
  * @brief  Return the current timer in [ms]
  * @param  None.
  * @retval Timer [ms] value.
  */

uint32_t Cmd::getTimerMs( void ) const
{
    return HAL_GetTick();
}