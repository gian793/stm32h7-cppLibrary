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

    CmdState state = CmdState::Idle;

    do{ state = cmd.execute(); } 
    while( state  == CmdState::WaitForReply );

    CHECK_TRUE( state == CmdState::Timeout );
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
                PrioLevel::high, cmdDefaultRetryNr, 
                cmdDefaultTimeoutMs, TEST_PeriodMs, TEST_DelayMs, 
                CmdOption::None   };

    cmd.init( 0 );

    /* Initial delay check. */
    auto time = HAL_GetTick();
    while( CmdState::Done != cmd.execute() ) {};
    auto deltaTimeMs = HAL_GetTick() - time;
    CHECK_TRUE( ( deltaTimeMs >= TEST_DelayMs ) && ( deltaTimeMs < ( TEST_DelayMs + 5 ) ) );

    auto cycleNr = 1;

    while( cycleNr-- > 0)
    {
        /* Period check. */
        time = HAL_GetTick();
        while( CmdState::Done != cmd.execute() ) {};
        deltaTimeMs = HAL_GetTick() - time;

        CHECK_TRUE( ( deltaTimeMs >= TEST_PeriodMs ) && ( deltaTimeMs < ( TEST_PeriodMs + 5 ) ) );
    }
}

TEST( cmd, cmdOption )
{
    Cmd cmd0{    CmdType::cmd1, CmdType::noCmd, PrioLevel::high, cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs   };
    Cmd cmd1{    CmdType::cmd1, CmdType::noCmd, PrioLevel::high, cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, CmdOption::RepeatOnReply };
    Cmd cmd2{    CmdType::cmd1, CmdType::noCmd, PrioLevel::high, cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, CmdOption::RepeatOnTimeout };
    Cmd cmd3{    CmdType::cmd1, CmdType::noCmd, PrioLevel::high, cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, CmdOption::RepeatForever };

    CHECK_TRUE( CmdOption::None            == cmd0.getOptions() );
    CHECK_TRUE( CmdOption::RepeatOnReply   == cmd1.getOptions() );
    CHECK_TRUE( CmdOption::RepeatOnTimeout == cmd2.getOptions() );
    CHECK_TRUE( CmdOption::RepeatForever   == cmd3.getOptions() );
}