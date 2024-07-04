#include "CppUTest/TestHarness.h"

#include <iostream>
#include <cstring>

extern "C"
{
	/*
	 * Add your c-only include files here
	 */
}

#include <random>
#include <array>
#include "cmdConfig.h"
#include "cmd.h"
#include "cmdCtrl.h"

TEST_GROUP( cmdCtrl )
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

TEST( cmdCtrl, cmdPriority)
{
    Cmd cmd1;   /* Default low priority. */
    Cmd cmd2;

    Cmd cmd3{   CmdType::noCmd, 
                CmdType::noCmd, 
                PrioLevel::high, 
                cmdDefaultRetryNr, 
                cmdDefaultTimeoutMs, 
                cmdDefaultPeriodMs, 
                cmdDefaultDelayMs, 
                nullptr, nullptr, nullptr   };

	CHECK_FALSE( Cmd::priorityGreater( cmd1, cmd2 ) );
    CHECK_TRUE( Cmd::priorityEqual( cmd1, cmd2 ) );
    CHECK_FALSE( Cmd::prioritySmaller( cmd1, cmd2 ) );

    CHECK_TRUE( Cmd::priorityGreater( cmd3, cmd2 ) );
    CHECK_TRUE( Cmd::prioritySmaller( cmd1, cmd3 ) );
}