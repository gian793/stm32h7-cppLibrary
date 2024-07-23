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
        switch( state )
        {
            case CmdState::Idle:
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