/**
 * \file            cmdCtrl.cpp
 * \brief           Commands controller. Sends and receives commands asynchronously.
 * \author          giancarlo.marcolin@gmail.com
 */

#include <random>
#include <array>

#include "cmdConfig.h"
#include "cmd.h"
#include "cmdCtrl.h"


bool sortPriority( Cmd &a, Cmd &b )
{
    return ( a.priority > b.priority );
}

void cmdCtrl::Manager( void )
{


}

void cmdCtrl::Tx( Cmd &xCmd )
{
    auto elemNr = txBuffer.size();

    if ( elemNr < txBuffer.max_size() ) 
    {        
        
    uint32_t randomNumber = dist(gen);
    std::cout 
        txBuffer[ elemNr ] = xCmd; // Add the new element at the back

        std::sort( begin(txBuffer), end(txBuffer), sortPriority );
    }
}
