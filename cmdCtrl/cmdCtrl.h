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

        cmdCtrl() { resetTxBuf(); }

        bool manager( void );

        bool load(  
                    CmdType   cmdType, 
                    CmdObj*   pCmdObj, 
                    uint32_t  cmdToken     = 0,

                    CmdType   cmdReplyType = CmdType::noCmd, 
                    
                    PrioLevel cmdPrioLevel = PrioLevel::low, 
                    uint32_t  cmdRetryNr   = cmdDefaultRetryNr, 
                    uint32_t  cmdTimeoutMs = cmdDefaultTimeoutMs, 
                    uint32_t  cmdPeriodMs  = cmdDefaultPeriodMs, 
                    uint32_t  cmdDelayMs   = cmdDefaultDelayMs  );

        //void RemoveCmd( uint32_t token );

        Cmd getTxCmd( uint32_t cmdIdx )  { return cmdBuffer[ cmdIdx ]; }

        uint32_t getTxCnt( void ) { return txCnt; } 

    private:

        LockingData_t cmdLock;

        std::array<Cmd, cmdBufferSize> cmdBuffer;    /* Out-going commands. */
        std::array<uint32_t, cmdBufferSize> prioBuffer;    /* In-coming commands or replies to out-going commands. */

        uint32_t txCnt{0};                        /* Number of cmds to send. */

        uint32_t nextToken{0};

        void resetTxBuf( void ) { for( Cmd &c: cmdBuffer ) { c.reset(); } }
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
