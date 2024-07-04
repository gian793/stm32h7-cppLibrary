/**
 * \file            cmdCtrl.cpp
 * \brief           Commands controller. Sends and receives commands asynchronously.
 * \author          giancarlo.marcolin@gmail.com
 */

#include <algorithm>
#include <array>

#include "cmdConfig.h"
#include "cmd.h"
#include "cmdCtrl.h"


/* bool cmdCtrl::CmpPriorityGreater( const Cmd &a, const Cmd &b )
{
    return ( a.priority > b.priority );
}  */

void cmdCtrl::Manager( void )
{


}

void cmdCtrl::Tx( Cmd &xCmd )
{
    auto elemNr = txBuffer.size();

    xCmd.setToken( ++nextToken );

    if ( elemNr < txBuffer.max_size() ) 
    {        
        
        //uint32_t cmdToken = ++token;

        txBuffer[ elemNr ] = xCmd; // Add the new element at the back

        std::sort( begin(txBuffer), end(txBuffer), Cmd::priorityGreater );
    }
}
