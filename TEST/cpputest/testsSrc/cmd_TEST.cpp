#include "CppUTest/TestHarness.h"

#include <iostream>
#include <cstring>

extern "C"
{
	/*
	 * Add your c-only include files here
	 */
    #include "stm32h7xx_hal.h"
}

#include <random>
#include <array>

#include "stm32_lock.h"
#include "cmdConfig.h"
#include "cmd.h"

class myObj: public CmdObj {
    public:
        void send( void ) override { sendCnt++; };
        void reply( void ) override { replyCnt++; };
        void timeout( void ) override { timeoutCnt++; };

        uint32_t getSendCnt( void ) { return sendCnt; }
        uint32_t getReplyCnt( void ) { return replyCnt; }
        uint32_t getTimeoutCnt( void ) { return timeoutCnt; }

        void reset( void ) { sendCnt = 0; replyCnt = 0; timeoutCnt = 0; }

    private:
        uint32_t sendCnt = 0;
        uint32_t replyCnt = 0;
        uint32_t timeoutCnt = 0;
};

TEST_GROUP( cmd )
{
    void setup()
    {	
		MemoryLeakWarningPlugin::saveAndDisableNewDeleteOverloads();
    }

    void teardown()
    {
		MemoryLeakWarningPlugin::restoreNewDeleteOverloads();
    }
};

TEST( cmd, cmdPriority )
{
    Cmd cmd1;   /* Default low priority. */
    Cmd cmd2;

    Cmd cmd3{   CmdType::noCmd, CmdType::noCmd, 
				PrioLevel::high, 
				cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs   };

	CHECK_TRUE( Cmd::priorityGreaterEqual( cmd1, cmd2 ) );
    CHECK_TRUE( Cmd::priorityEqual( cmd1, cmd2 ) );
    CHECK_TRUE( Cmd::prioritySmallerEqual( cmd1, cmd2 ) );

    CHECK_TRUE( Cmd::priorityGreaterEqual( cmd3, cmd2 ) );
    CHECK_TRUE( Cmd::prioritySmallerEqual( cmd1, cmd3 ) );
}

TEST( cmd, executeIdle )
{
    Cmd cmd;

    CHECK_TRUE( ( CmdState::Idle == cmd.execute() ) );
    CHECK_TRUE( ( CmdState::Idle == cmd.execute() ) );
    CHECK_TRUE( ( CmdState::Idle == cmd.execute() ) );
}

TEST( cmd, executeNoReply )
{
    Cmd cmd{ CmdType::cmd1 };

    CHECK_TRUE( ( CmdState::Done == cmd.execute() ) );
}

TEST( cmd, executeWithReply )
{
    Cmd cmd{ CmdType::cmd1, CmdType::cmd2 };

    CHECK_TRUE( ( CmdState::Sent == cmd.execute() ) );
}

TEST( cmd, executeWaitForReply )
{
    Cmd cmd{ CmdType::cmd1, CmdType::cmd2 };

    CHECK_TRUE( ( CmdState::Sent == cmd.execute() ) );

    CHECK_TRUE( ( CmdState::WaitForReply == cmd.execute() ) );
}

TEST( cmd, executeDelay )
{
    constexpr uint32_t TEST_DelayMs = 175;

    Cmd cmd{    CmdType::cmd1, CmdType::noCmd, 
                PrioLevel::high, 
                cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, TEST_DelayMs   };

    cmd.init( 0 );

    auto time1 = HAL_GetTick();

    while( CmdState::Done != cmd.execute() ) {};

    auto deltaTimeMs = HAL_GetTick() - time1;

    CHECK_TRUE( ( deltaTimeMs >= TEST_DelayMs ) && ( deltaTimeMs < ( TEST_DelayMs + 5 ) ) );
}

TEST( cmd, timeout )
{
    constexpr uint32_t TEST_DelayMs = 5;

    Cmd cmd{    CmdType::cmd1, CmdType::cmd2, 
                PrioLevel::high, 
                cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, TEST_DelayMs   };

    CHECK_TRUE( CmdState::Sent == cmd.execute() );

    while( cmd.execute() != CmdState::Timeout ) {}

    CHECK_TRUE( CmdState::Done == cmd.execute() );
}

TEST( cmd, sendCb_noReply )
{
    myObj localObj;

    localObj.reset();

    Cmd cmd{    &localObj, 
                CmdType::cmd1, CmdType::noCmd, 
                PrioLevel::high, 
                cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs   };

    CHECK_EQUAL( 0, localObj.getSendCnt() );

    CHECK_TRUE( CmdState::Done == cmd.execute() );

    CHECK_EQUAL( 1, localObj.getSendCnt() );
}

TEST( cmd, sendCb_withReply )
{
    myObj localObj;

    localObj.reset();

    Cmd cmd{    &localObj, 
                CmdType::cmd1, CmdType::cmd2, 
                PrioLevel::high, 
                cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs   };

    cmd.execute();

    CHECK_EQUAL( 1, localObj.getSendCnt() );
}

TEST( cmd, replyCb )
{
    myObj localObj;

    localObj.reset();

    Cmd cmd{    &localObj, 
                CmdType::cmd1, CmdType::cmd2, 
                PrioLevel::high, 
                cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs   };

    cmd.execute();
    cmd.replied();
    cmd.execute();

    CHECK_EQUAL( 1, localObj.getReplyCnt() );
}

TEST( cmd, timeoutCb )
{
    myObj localObj;

    localObj.reset();

    Cmd cmd{    &localObj, 
                CmdType::cmd1, CmdType::cmd2, 
                PrioLevel::high, 
                cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs   };

    cmd.execute();
    while( CmdState::WaitForReply == cmd.execute() ) {}

    CHECK_EQUAL( 0, localObj.getReplyCnt() );
    CHECK_EQUAL( 0, localObj.getReplyCnt() );
    CHECK_EQUAL( 1, localObj.getTimeoutCnt() );
}

TEST( cmd, executePeriod )
{
    constexpr uint32_t TEST_DelayMs  = 25;
    constexpr uint32_t TEST_PeriodMs = 50;

    Cmd cmd{    CmdType::cmd1, CmdType::noCmd, 
                PrioLevel::high, 
                cmdDefaultRetryNr, cmdDefaultTimeoutMs, TEST_PeriodMs, TEST_DelayMs   };

    cmd.init( 0 );

    /* Initial delay check. */
    auto time = HAL_GetTick();
    while( CmdState::Done != cmd.execute() ) {};
    auto deltaTimeMs = HAL_GetTick() - time;
    CHECK_TRUE( ( deltaTimeMs >= TEST_DelayMs ) && ( deltaTimeMs < ( TEST_DelayMs + 10 ) ) );

    auto cycleNr = 5;

    while( cycleNr-- > 0)
    {
        /* Period check. */
        time = HAL_GetTick();
        while( CmdState::Done != cmd.execute() ) {};
        deltaTimeMs = HAL_GetTick() - time;

        CHECK_TRUE( ( deltaTimeMs >= TEST_PeriodMs ) && ( deltaTimeMs < ( TEST_PeriodMs + 5 ) ) );
    }
}


// TEST( cmd, donCb2 )
// {
//     Cmd cmd{    CmdType::cmd1, CmdType::noCmd, 
//                 PrioLevel::high, 
//                 cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs   };

//     myObj localObj;

//     localObj.reset();

//     cmd.init( 0, &localObj );

//     CHECK_EQUAL( 0, localObj.getRes() );

//     CHECK_TRUE( CmdState::Sent == cmd.execute() );

//     CHECK_EQUAL( 1, localObj.getSendCnt() );

//     std::cout << "Test c6" << std::endl;
// }

// TEST( cmd, waitForReplyState )
// {
//     Cmd cmd{    CmdType::cmd1, CmdType::cmd1, 
//                 PrioLevel::high, 
//                 cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs   };

//     cmd.init( 0 );

//     CHECK_TRUE( CmdState::Sent == cmd.execute() );

//     CHECK_TRUE( CmdState::WaitForReply == cmd.execute() );

//     std::cout << "Test c7" << std::endl;
// }