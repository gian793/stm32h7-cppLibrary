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
        uint32_t timerMs = HAL_GetTick();

        switch( state )
        {
            case CmdState::Idle:
                if( ( timerMs - delayTimerMs ) >= delayMs )
                {
                    delayTimerMs = ( timerMs - delayTimerMs ) - delayMs;

                    if( pObj != nullptr )
                    {
                        pObj->send();
                    }

                    state = CmdState::Sent;
                }
                break; 
            
            case CmdState::Sent:
                //(this->*send_Cb)();
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
    return HAL_GetTick();
}