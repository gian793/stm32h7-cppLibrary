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
        CmdObj testObj;
        
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

    CHECK_EQUAL( 0, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, cmdCnt )
{
     cmdCtrl testCtrl;
     Cmd cmd; 

     testCtrl.load( CmdType::cmd1, &testObj );
     testCtrl.load( CmdType::cmd1, &testObj );
     testCtrl.load( CmdType::cmd1, &testObj );

     CHECK_EQUAL( 3, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, cmdNotAdded )
{
     cmdCtrl testCtrl;
     Cmd cmd; 

     testCtrl.load( CmdType::cmd1, &testObj );
     testCtrl.load( CmdType::noCmd, &testObj );
     testCtrl.load( CmdType::cmd2, &testObj );

     CHECK_EQUAL( 2, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, txSort )
{
    Cmd c1{ CmdType::cmd1, 
            CmdType::noCmd, 
            PrioLevel::high, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs };

    Cmd c2{ CmdType::cmd2, 
            CmdType::noCmd, 
            PrioLevel::low, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs };

    Cmd c3{ CmdType::cmd3, 
            CmdType::noCmd, 
            PrioLevel::l2, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs };

    Cmd c4{ CmdType::cmd4, 
            CmdType::noCmd, 
            PrioLevel::l2, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs };
    
    Cmd c5{ CmdType::cmd5, 
            CmdType::noCmd, 
            PrioLevel::low, 
            cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs };

    cmdCtrl testCtrl;

//     CHECK_TRUE( testCtrl.load( c1 ) );
//     CHECK_TRUE( testCtrl.load( c2 ) );
//     CHECK_TRUE( testCtrl.load( c3 ) );
//     CHECK_TRUE( testCtrl.load( c4 ) );
//     CHECK_TRUE( testCtrl.load( c5 ) );

//     CHECK_TRUE( testCtrl.getTxCmd( 0 ).type == CmdType::cmd5 );
//     CHECK_TRUE( testCtrl.getTxCmd( 1 ).type == CmdType::cmd2 );
//     CHECK_TRUE( testCtrl.getTxCmd( 2 ).type == CmdType::cmd3 );
//     CHECK_TRUE( testCtrl.getTxCmd( 3 ).type == CmdType::cmd4 );
//     CHECK_TRUE( testCtrl.getTxCmd( 4 ).type == CmdType::noCmd );
//     CHECK_TRUE( testCtrl.getTxCmd( 5 ).type == CmdType::cmd1 );

//     CHECK_EQUAL( 1, testCtrl.getTxCmd( 4 ).token );
//     CHECK_EQUAL( 2, testCtrl.getTxCmd( 5 ).token );
//     CHECK_EQUAL( 3, testCtrl.getTxCmd( 1 ).token );
//     CHECK_EQUAL( 4, testCtrl.getTxCmd( 2 ).token );
//     CHECK_EQUAL( 5, testCtrl.getTxCmd( 3 ).token );
//     CHECK_EQUAL( 6, testCtrl.getTxCmd( 0 ).token );

//     std::cout << "Test t2" << std::endl;
}

// TEST( cmdCtrl, emptyManager )
// {
//     cmdCtrl testCtrl;

//     CHECK_TRUE( testCtrl.manager() );
    
//     std::cout << "Test t3" << std::endl;
// }