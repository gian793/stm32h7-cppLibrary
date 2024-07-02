/**
 * \file            cmdCtrl.h
 * \brief           Commands controller. Sends and receives commands asynchronously.
 * \author          giancarlo.marcolin@gmail.com
 */

#ifndef CMD_CTRL_H_
#define CMD_CTRL_H_

/*---------------------------------------------------------------------------*/

class cmdCtrl
{
public:

    cmdCtrl(): txCnt(0), rxCnt(0) {
        std::random_device rd;  
        std::mt19937 gen(rd());     /* Standard mersenne_twister_engine seeded with rd(). */
        std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());
    }

    void Manager( void );

    void Tx( Cmd &xCmd );

private:
    std::array<Cmd, cmdBufferSize> txBuffer;    /* Out-going commands. */

    std::array<Cmd, cmdBufferSize> rxBuffer;    /* In-coming commands or replies to out-going commands. */

    uint32_t txCnt;

    uint32_t rxCnt;
};

/*---------------------------------------------------------------------------*/

#endif /* CSIMPLE_TEST_H_ */


/*---------------------------------------------------------------------------*/
/*
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <array>

#include <algorithm>
#include <iterator>
 

typedef struct
{
    char c;
    int m;
}
type1_t;

bool sort_m( type1_t &a, type1_t&b )
{
    return (a.m > b.m);
}

int main()
{
    std::array<type1_t, 3> arr{{ {'w',2}, {'y',49}, {'s',6} }};

    std::sort(begin(arr), end(arr), sort_m);

    for( auto &i: arr )std::cout << i.c << std::endl;  
}
*/
/*---------------------------------------------------------------------------*/


/*

enum class cmdType{ cmd_xx1 = 0,
                    cmd_xx2,
                    cmd_xx3,
                    cmd_Max,

                    // helpers.
                    begin = 0,
                    end = cmd_Max };

int countCmd( cmdType cmd )
{
    if( cmd >= cmdType::end )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

consteval int cmdTypeCout( void )
{
    return static_cast<int>(cmdType::end);
}

void cmdTypeLoop( void )
{
    for( cmdType cmd = cmdType::begin; cmd < cmdType::end; cmd = static_cast<cmdType>(static_cast<size_t>(cmd)+1) )
    {

    }
}

int main()
{
    int i = 11;

    cmdType cmd = cmdType::cmd_Max;

    //std::cout << "CmdType is: " << countCmd( cmd ) << std::endl;

}
 */
