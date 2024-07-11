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
    testCtrl.Rx( cmd );
    testCtrl.Rx( cmd );

    CHECK_EQUAL( 3, testCtrl.getTxCnt() );
    CHECK_EQUAL( 4, testCtrl.getRxCnt() );
}

TEST( cmdCtrl, setGetToken )
{
    Cmd cmd; 
    cmd.setToken( 32 );

    CHECK_EQUAL( 32, cmd.getToken() );
}

TEST( cmdCtrl, txSort )
{

    Cmd t0{ CmdType::noCmd, 
            CmdType::noCmd, 
            PrioLevel::l3, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, 
            nullptr, nullptr, nullptr   };

    Cmd t1{ CmdType::cmd1, 
            CmdType::noCmd, 
            PrioLevel::high, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, 
            nullptr, nullptr, nullptr   };

    Cmd t2{ CmdType::cmd2, 
            CmdType::noCmd, 
            PrioLevel::low, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, 
            nullptr, nullptr, nullptr   };

    Cmd t3{ CmdType::cmd3, 
            CmdType::noCmd, 
            PrioLevel::l2, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, 
            nullptr, nullptr, nullptr   };

    Cmd t4{ CmdType::cmd4, 
            CmdType::noCmd, 
            PrioLevel::l2, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, 
            nullptr, nullptr, nullptr   };
    
    Cmd t5{ CmdType::cmd5, 
            CmdType::noCmd, 
            PrioLevel::low, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, 
            nullptr, nullptr, nullptr   };

    cmdCtrl testCtrl;

    CHECK_TRUE( testCtrl.Tx( t0 ) );
    CHECK_TRUE( testCtrl.Tx( t1 ) );
    CHECK_TRUE( testCtrl.Tx( t2 ) );
    CHECK_TRUE( testCtrl.Tx( t3 ) );
    CHECK_TRUE( testCtrl.Tx( t4 ) );
    CHECK_TRUE( testCtrl.Tx( t5 ) );

    CHECK_TRUE( testCtrl.getTxCmd( 0 ).type == CmdType::cmd5 );
    CHECK_TRUE( testCtrl.getTxCmd( 1 ).type == CmdType::cmd2 );
    CHECK_TRUE( testCtrl.getTxCmd( 2 ).type == CmdType::cmd3 );
    CHECK_TRUE( testCtrl.getTxCmd( 3 ).type == CmdType::cmd4 );
    CHECK_TRUE( testCtrl.getTxCmd( 4 ).type == CmdType::noCmd );
    CHECK_TRUE( testCtrl.getTxCmd( 5 ).type == CmdType::cmd1 );

    CHECK_EQUAL( 1, testCtrl.getTxCmd( 4 ).getToken() );
    CHECK_EQUAL( 2, testCtrl.getTxCmd( 5 ).getToken() );
    CHECK_EQUAL( 3, testCtrl.getTxCmd( 1 ).getToken() );
    CHECK_EQUAL( 4, testCtrl.getTxCmd( 2 ).getToken() );
    CHECK_EQUAL( 5, testCtrl.getTxCmd( 3 ).getToken() );
    CHECK_EQUAL( 6, testCtrl.getTxCmd( 0 ).getToken() );
}

TEST( cmdCtrl, rxSort )
{

    Cmd r0{ CmdType::noCmd, 
            CmdType::noCmd, 
            PrioLevel::l3, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, 
            nullptr, nullptr, nullptr   };

    Cmd r1{ CmdType::cmd1, 
            CmdType::noCmd, 
            PrioLevel::high, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, 
            nullptr, nullptr, nullptr   };

    Cmd r2{ CmdType::cmd2, 
            CmdType::noCmd, 
            PrioLevel::low, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, 
            nullptr, nullptr, nullptr   };

    cmdCtrl testCtrl;

    r0.setToken( 1 );
    r1.setToken( 2 );
    r2.setToken( 3 );
    
    CHECK_TRUE( testCtrl.Rx( r0 ) );
    CHECK_TRUE( testCtrl.Rx( r1 ) );
    CHECK_TRUE( testCtrl.Rx( r2 ) );

    CHECK_TRUE( testCtrl.getRxCmd( 0 ).type == CmdType::cmd2 );
    CHECK_TRUE( testCtrl.getRxCmd( 1 ).type == CmdType::noCmd );
    CHECK_TRUE( testCtrl.getRxCmd( 2 ).type == CmdType::cmd1 );

    CHECK_EQUAL( 1, testCtrl.getRxCmd( 1 ).getToken() );
    CHECK_EQUAL( 2, testCtrl.getRxCmd( 2 ).getToken() );
    CHECK_EQUAL( 3, testCtrl.getRxCmd( 0 ).getToken() );
}

TEST( cmdCtrl, emptyManager )
{
    cmdCtrl testCtrl;

    CHECK_FALSE( testCtrl.Manager() );
}