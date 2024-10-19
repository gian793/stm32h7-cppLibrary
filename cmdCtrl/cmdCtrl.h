/**
 * \file            cmdCtrl.h
 * \brief           Commands controller. Sends and receives commands asynchronously.
 * \author          giancarlo.marcolin@gmail.com
 */

#ifndef CMD_CTRL_H_
#define CMD_CTRL_H_

/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/

class cmdCtrl
{
    public:

        cmdCtrl() { reset(); }

        uint32_t manager( void );

        bool load(                      
                    CmdObj*   pCmdObj, 
                    CmdType   cmdType, 
                    CmdType   cmdReplyType = CmdType::noCmd, 
                    PrioLevel cmdPrioLevel = PrioLevel::low, 
                    uint32_t  cmdToken    = 0,

                    uint32_t  cmdRetryNr   = cmdDefaultRetryNr, 
                    uint32_t  cmdTimeoutMs = cmdDefaultTimeoutMs, 
                    uint32_t  cmdPeriodMs  = cmdDefaultPeriodMs, 
                    uint32_t  cmdDelayMs   = cmdDefaultDelayMs,  
                    
                    bool isReply = false );

        //bool removeCmd( uint32_t token );

        uint32_t getCmdCnt( void ) { return cnt; } 

        //uint32_t getFreeIdxCnt( void ) { return ( cmdBufferSize - cnt ); }

    private:

        LockingData_t cmdLock;

        std::array<Cmd,      cmdBufferSize> cmdBuffer;  /* In-coming commands or replies to out-going commands. */
        std::array<uint32_t, cmdBufferSize> prioBuffer; /* Increasing priority order, index=0 has lowest priority. */
        std::array<uint32_t, cmdBufferSize> idxBuffer;  /* Next command free index. */

        uint32_t cnt{0};                                /* Number of commands to execute. */

        uint32_t nextIdx{0};                            /* Index of the next command to load. */

        uint32_t nextToken{0};

        uint32_t dummy{0};

        void reset( void ) 
        { 
            for( uint32_t i = 0; i < cmdBufferSize; ++i )
            {
                cmdBuffer[ i ].reset();

                idxBuffer[ i ] = i;
            }  
        }

        Cmd getCmd( uint32_t cmdIdx )  { return cmdBuffer[ cmdIdx ]; }

        void freeIndex( uint32_t idx ) 
        { 
            /* Remove idx from priority buffer. */
            for( auto i = idx; i < cnt - 1; ++i )   
            {
                prioBuffer[ i ] = prioBuffer[ i + 1 ];
            }

            /* Add idx to available index buffer. */
            idxBuffer[ --cnt ] = idx;
        }; 
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
