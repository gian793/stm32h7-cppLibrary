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
#include "cmdCtrl.h"

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

uint32_t prioIdx{0};
std::array<char, cmdBufferSize> prioritySeq;

class myObj1: public CmdObj {public: void send( void ) override { prioritySeq[ prioIdx++] = '1'; }; };
class myObj2: public CmdObj {public: void send( void ) override { prioritySeq[ prioIdx++] = '2'; }; };
class myObj3: public CmdObj {public: void send( void ) override { prioritySeq[ prioIdx++] = '3'; }; };
class myObj4: public CmdObj {public: void send( void ) override { prioritySeq[ prioIdx++] = '4'; }; };
class myObj5: public CmdObj {public: void send( void ) override { prioritySeq[ prioIdx++] = '5'; }; };

TEST_GROUP( cmdCtrl )
{
    myObj testObj;

    myObj1 obj1;
    myObj2 obj2;
    myObj3 obj3;
    myObj4 obj4;
    myObj5 obj5;

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

    testCtrl.load( CmdType::cmd1, &testObj );
    testCtrl.load( CmdType::cmd1, &testObj );
    testCtrl.load( CmdType::cmd1, &testObj );

    CHECK_EQUAL( 3, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, cmdNotAdded )
{
    cmdCtrl testCtrl;

    testCtrl.load( CmdType::cmd1, &testObj );
    testCtrl.load( CmdType::noCmd, &testObj );
    testCtrl.load( CmdType::cmd2, &testObj );

    CHECK_EQUAL( 2, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, txSort )
{
    cmdCtrl testCtrl;

    CHECK_TRUE( testCtrl.load( CmdType::cmd1, &obj1, PrioLevel::high ) );
    CHECK_TRUE( testCtrl.load( CmdType::cmd1, &obj2, PrioLevel::low ) );
    CHECK_TRUE( testCtrl.load( CmdType::cmd1, &obj3, PrioLevel::l2 ) );
    CHECK_TRUE( testCtrl.load( CmdType::cmd1, &obj4, PrioLevel::l2 ) );
    CHECK_TRUE( testCtrl.load( CmdType::cmd1, &obj5, PrioLevel::l3 ) );

    testCtrl.manager();

    CHECK_TRUE( prioritySeq[0] == '1' );
    CHECK_TRUE( prioritySeq[1] == '5' );
    CHECK_TRUE( prioritySeq[2] == '3' );
    CHECK_TRUE( prioritySeq[3] == '4' );
    CHECK_TRUE( prioritySeq[4] == '2' );
}

TEST( cmdCtrl, emptyManager )
{
    cmdCtrl testCtrl;

   CHECK_TRUE( testCtrl.manager() == 0 );
}

TEST( cmdCtrl, notEmptyManager )
{
    cmdCtrl testCtrl;

    testCtrl.load( CmdType::cmd1, &testObj );
    testCtrl.load( CmdType::cmd1, &testObj );
    testCtrl.load( CmdType::cmd2, &testObj );

   CHECK_TRUE( testCtrl.manager() == 3 );
}

TEST( cmdCtrl, periodicManager )
{
    
    
    first check that periodic commands remoaininthe ctrl array



    constexpr uint32_t TEST_PeriodMs = 10;

    cmdCtrl testCtrl;

    testCtrl.load( CmdType::cmd1, &testObj );
    testCtrl.load( CmdType::cmd1, &testObj, PrioLevel::high, CmdType::noCmd, cmdDefaultRetryNr, cmdDefaultTimeoutMs, TEST_PeriodMs, cmdDefaultDelayMs );
    testCtrl.load( CmdType::cmd2, &testObj );

    CHECK_TRUE( testCtrl.manager() == 3 );

    auto time1 = HAL_GetTick();
    while( testCtrl.manager() == 0 ) {};
    auto deltaTimeMs = HAL_GetTick() - time1;

    CHECK_TRUE( deltaTimeMs >= 10 );
}