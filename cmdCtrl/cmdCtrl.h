/**
 * \file            cmdCtrl.h
 * \brief           Commands controller. Sends and receives commands asynchronously.
 * \author          giancarlo.marcolin@gmail.com
 */

#ifndef CMD_CTRL_H_
#define CMD_CTRL_H_

/*---------------------------------------------------------------------------*/

#include <iostream>
#include <cstring>

/*---------------------------------------------------------------------------*/

class cmdCtrl
{
    public:

        cmdCtrl() { reset(); }

        uint32_t manager( void );

        bool loadCmd(   CmdObj*   pCmdObj, 
                        CmdType   cmdType, 
                        CmdType   cmdReplyType = CmdType::noCmd, 
                        PrioLevel cmdPrioLevel = PrioLevel::low, 
                        uint32_t  cmdToken     = 0,

                        uint32_t  cmdRetryNr   = cmdDefaultRetryNr, 
                        uint32_t  cmdTimeoutMs = cmdDefaultTimeoutMs, 
                        uint32_t  cmdPeriodMs  = cmdDefaultPeriodMs, 
                        uint32_t  cmdDelayMs   = cmdDefaultDelayMs,
                        CmdOption cmdOptions   = CmdOption::None  );

        bool loadReply( CmdObj*   pCmdObj, 
                        CmdType   cmdType, 
                        CmdType   cmdReplyType = CmdType::noCmd, 
                        PrioLevel cmdPrioLevel = PrioLevel::low, 
                        uint32_t  cmdToken     = 0,

                        uint32_t  cmdRetryNr   = cmdDefaultRetryNr, 
                        uint32_t  cmdTimeoutMs = cmdDefaultTimeoutMs, 
                        uint32_t  cmdPeriodMs  = cmdDefaultPeriodMs, 
                        uint32_t  cmdDelayMs   = cmdDefaultDelayMs );

        bool removeCmdByToken( uint32_t token );

        bool removeCmdByIdx( uint32_t idx );

        uint32_t getCmdCnt( void ) { return cnt; } 

        //uint32_t getFreeIdxCnt( void ) { return ( cmdBufferSize - cnt ); }

    private:

        LockingData_t cmdLock;

        std::array<Cmd,      cmdBufferSize> cmdBuffer;  /* In-coming commands or replies to out-going commands. */
        std::array<uint32_t, cmdBufferSize> prioBuffer = {0}; /* Increasing priority order, index=0 has lowest priority. */
        std::array<uint32_t, cmdBufferSize> idxBuffer = {0};  /* Next command free index. Indexes < cnt are already in use. Indexes >= cnt are free. */

        uint32_t cnt{0};                                /* Number of commands to execute. */

        uint32_t nextIdx{0};                            /* Index of the next command to load. */

        // Test comment ++

        void reset( void ) 
        { 
            for( uint32_t i = 0; i < cmdBufferSize; ++i )
            {
                cmdBuffer[ i ].reset();

                idxBuffer[ i ] = i;
            }  
        }

        Cmd getCmd( uint32_t cmdIdx )  { return cmdBuffer[ cmdIdx ]; }


        bool freeIndex( uint32_t idx ) 
        { 
            bool isIndexFound = false;
            bool isPriorityFound = false;

            for( uint32_t i = 0; i < cnt; ++i )   
            {
                /* Remove idx from idxBuffer. */
                if( isIndexFound == false )
                {
                    if( idxBuffer[ i ] == idx )
                    {
                        isIndexFound = true;

                        if( i != cnt - 1 )
                        {
                            idxBuffer[ i ] = idxBuffer[ i + 1 ];
                        }
                    }
                }
                else
                {
                    idxBuffer[ i ] = idxBuffer[ i + 1 ];
                }

                /* Remove idx from prioBuffer. */
                if( isPriorityFound == false )
                {
                    if( prioBuffer[ i ] == idx )
                    {
                        isPriorityFound = true;
                        
                        if( i != cnt - 1 )
                        {
                            prioBuffer[ i ] = prioBuffer[ i + 1 ];
                        }
                    }
                }
                else
                {
                    prioBuffer[ i ] = prioBuffer[ i + 1 ];
                }
            }

            if( isIndexFound )
            {
                --cnt;
            }

            return isIndexFound;
        }

        uint32_t getNewToken( void )
        {
            uint32_t rndToken = rand();

            for( uint32_t i = 0; i < cnt; ++i )   
            {
                /* If token already in use, repeat all. */
                if( cmdBuffer[ idxBuffer[ i ] ].token == rndToken )
                {
                    return getNewToken();
                }
            }

            return rndToken;
        }
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

    //// std::cout << "CmdType is: " << countCmd( cmd ) << std::endl; 

}
 */
