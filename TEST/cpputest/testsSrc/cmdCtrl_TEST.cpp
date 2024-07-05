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

TEST( cmdCtrl, cmdPriority )
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

TEST( cmdCtrl, Init )
{
    cmdCtrl testCtrl;

    CHECK_EQUAL( 0, testCtrl.getTxCnt() );
    CHECK_EQUAL( 0, testCtrl.getRxCnt() );
}

TEST( cmdCtrl, cmdCnt )
{
    cmdCtrl testCtrl;
    Cmd cmd; 

    testCtrl.Tx( cmd );
    testCtrl.Tx( cmd );
    testCtrl.Tx( cmd );

    testCtrl.Rx( cmd );
    testCtrl.Rx( cmd );

    CHECK_EQUAL( 3, testCtrl.getTxCnt() );
    CHECK_EQUAL( 2, testCtrl.getRxCnt() );
}