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

class myObj: public cmdObj {
    public:
        void send( void ) override { res++; };

        uint32_t getRes( void ) { return res; }
        void reset( void ) { res = 0; }

    private:
        uint32_t res = 0;
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

    std::cout << "Test c1" << std::endl;
}

TEST( cmd, executeIdle )
{
    Cmd cmd;

    CHECK_TRUE( ( CmdState::Idle == cmd.execute() ) );
    CHECK_TRUE( ( CmdState::Idle == cmd.execute() ) );
    CHECK_TRUE( ( CmdState::Idle == cmd.execute() ) );

    std::cout << "Test c2" << std::endl;
}

TEST( cmd, executeType1 )
{
    Cmd cmd{ CmdType::cmd1 };

    CHECK_TRUE( ( CmdState::Sent == cmd.execute() ) );

    std::cout << "Test c3" << std::endl;
}

TEST( cmd, executeDelay )
{
    constexpr uint32_t TEST_DelayMs = 23;

    Cmd cmd{    CmdType::cmd1, CmdType::noCmd, 
                PrioLevel::high, 
                cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, TEST_DelayMs   };

    cmd.init( 0 );

    auto time1 = HAL_GetTick();

    while( CmdState::Sent != cmd.execute() ) {};

    auto deltaTimeMs = HAL_GetTick() - time1;

    CHECK_TRUE( deltaTimeMs == TEST_DelayMs );

    std::cout << "Test c4" << std::endl;
}

TEST( cmd, donCb1 )
{
    myObj localObj;

    localObj.reset();

    Cmd cmd{    &localObj, 
                CmdType::cmd1, CmdType::noCmd, 
                PrioLevel::high, 
                cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs   };


    cmd.init( 0 );

    CHECK_EQUAL( 0, localObj.getRes() );

    CHECK_TRUE( CmdState::Sent == cmd.execute() );

    CHECK_EQUAL( 1, localObj.getRes() );

    std::cout << "Test c5" << std::endl;
}

TEST( cmd, donCb2 )
{
    Cmd cmd{    CmdType::cmd1, CmdType::noCmd, 
                PrioLevel::high, 
                cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs   };

    myObj localObj;

    localObj.reset();

    cmd.init( 0, &localObj );

    CHECK_EQUAL( 0, localObj.getRes() );

    CHECK_TRUE( CmdState::Sent == cmd.execute() );

    CHECK_EQUAL( 1, localObj.getRes() );

    std::cout << "Test c6" << std::endl;
}

TEST( cmd, waitForReplyState )
{
    Cmd cmd{    CmdType::cmd1, CmdType::cmd1, 
                PrioLevel::high, 
                cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs   };

    cmd.init( 0 );

    CHECK_TRUE( CmdState::Sent == cmd.execute() );

    CHECK_TRUE( CmdState::WaitForReply == cmd.execute() );

    std::cout << "Test c7" << std::endl;
}