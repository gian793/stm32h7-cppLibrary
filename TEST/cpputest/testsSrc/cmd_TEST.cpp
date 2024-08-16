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

void testDoneCb( void );

bool globalFlag = false;
void testDoneCb( void ) { globalFlag = true; }

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

TEST( cmd, cmdInit )
{

}

TEST( cmd, cmdPriority )
{
    Cmd cmd1;   /* Default low priority. */
    Cmd cmd2;

    Cmd cmd3{   CmdType::noCmd, CmdType::noCmd, 
						PrioLevel::high, 
						cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, 
						nullptr, nullptr, nullptr   };

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
}

TEST( cmd, executeType1 )
{
    Cmd cmd{ CmdType::cmd1 };

    CHECK_TRUE( ( CmdState::Sent == cmd.execute() ) );
}

TEST( cmd, setGetToken )
{
    Cmd cmd; 
    cmd.setToken( 32 );

    CHECK_EQUAL( 32, cmd.getToken() );
}

TEST( cmd, executeDelay )
{
    constexpr uint32_t TEST_DelayMs = 23;

    Cmd cmd{    CmdType::cmd1, CmdType::noCmd, 
                PrioLevel::high, 
                cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, TEST_DelayMs, 
                nullptr, nullptr, nullptr   };

    cmd.init( 0 );

    auto time1 = HAL_GetTick();

    while( CmdState::Sent != cmd.execute() ) {};

    auto deltaTimeMs = HAL_GetTick() - time1;

    CHECK_TRUE( deltaTimeMs >= TEST_DelayMs &&  deltaTimeMs < ( TEST_DelayMs + 1 ));
}

TEST( cmd, donCb )
{
    Cmd cmd{    CmdType::cmd1, CmdType::noCmd, 
                PrioLevel::high, 
                cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, 
                nullptr, nullptr, nullptr };

    //cmd.setCallbacks( nullptr, nullptr, testDoneCb );

    //while( CmdState::Sent != cmd.execute() ) {};

    //CHECK_TRUE( globalFlag );
}