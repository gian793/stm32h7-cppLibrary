/**
 * \file            cmd.cpp
 * \brief           Commands class.
 * \author          giancarlo.marcolin@gmail.com
 */

// #include <algorithm>
// #include <array>
#include <cstdint>

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
                    delayTimerMs   = timerMs - delayTimerMs - delayMs;
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

            case CmdState::Done:
            case CmdState::Timeout:
                if( periodMs > 0 )
                {
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