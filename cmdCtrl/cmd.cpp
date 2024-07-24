/**
 * \file            cmd.cpp
 * \brief           Commands class.
 * \author          giancarlo.marcolin@gmail.com
 */

// #include <algorithm>
// #include <array>
// #include <cstdint>

// #include "stm32_lock.h"
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
        uint32_t timerMs = 0; //getTimerValue()

        switch( state )
        {
            case CmdState::Idle:
                if( ( timerMs - delayTimerMs ) >= delayMs )
                {
                    delayTimerMs = ( timerMs - delayTimerMs ) - delayMs;
                    state = CmdState::Sent;
                }
                break; 
            
            case CmdState::Sent:
                break; 
            
            case CmdState::WaitForReply:
                break; 
            
            case CmdState::Timeout:
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
    return 0;
}