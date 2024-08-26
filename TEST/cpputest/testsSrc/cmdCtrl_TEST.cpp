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

#include "stm32_lock.h"
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

TEST( cmdCtrl, Init )
{
    cmdCtrl testCtrl;

    CHECK_EQUAL( 0, testCtrl.getTxCnt() );
}

TEST( cmdCtrl, cmdCnt )
{
    cmdCtrl testCtrl;
    Cmd cmd; 

    testCtrl.Tx( cmd );
    testCtrl.Tx( cmd );
    testCtrl.Tx( cmd );

    CHECK_EQUAL( 3, testCtrl.getTxCnt() );
}

TEST( cmdCtrl, txSort )
{

    Cmd t0{ CmdType::noCmd, 
            CmdType::noCmd, 
            PrioLevel::l3, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs };

    Cmd t1{ CmdType::cmd1, 
            CmdType::noCmd, 
            PrioLevel::high, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs };

    Cmd t2{ CmdType::cmd2, 
            CmdType::noCmd, 
            PrioLevel::low, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs };

    Cmd t3{ CmdType::cmd3, 
            CmdType::noCmd, 
            PrioLevel::l2, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs };

    Cmd t4{ CmdType::cmd4, 
            CmdType::noCmd, 
            PrioLevel::l2, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs };
    
    Cmd t5{ CmdType::cmd5, 
            CmdType::noCmd, 
            PrioLevel::low, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs };

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

    CHECK_EQUAL( 1, testCtrl.getTxCmd( 4 ).token );
    CHECK_EQUAL( 2, testCtrl.getTxCmd( 5 ).token );
    CHECK_EQUAL( 3, testCtrl.getTxCmd( 1 ).token );
    CHECK_EQUAL( 4, testCtrl.getTxCmd( 2 ).token );
    CHECK_EQUAL( 5, testCtrl.getTxCmd( 3 ).token );
    CHECK_EQUAL( 6, testCtrl.getTxCmd( 0 ).token );
}


TEST( cmdCtrl, emptyManager )
{
    cmdCtrl testCtrl;

    CHECK_TRUE( testCtrl.Manager() );
}