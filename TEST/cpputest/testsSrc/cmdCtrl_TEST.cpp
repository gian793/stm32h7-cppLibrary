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
        myObj() : sendCnt{0}, replyCnt{0}, timeoutCnt{0} {}

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

/*
class pingObj: public CmdObj {
    public:
        myObj() : sendCnt{0}, replyCnt{0}, timeoutCnt{0} {}

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
*/

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

// #if 0 
TEST( cmdCtrl, Init )
{
    cmdCtrl testCtrl;

    CHECK_EQUAL( 0, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, cmdCnt )
{
    cmdCtrl testCtrl;

    testCtrl.loadCmd( &testObj, CmdType::cmd1  );
    testCtrl.loadCmd( &testObj, CmdType::cmd1  );
    testCtrl.loadCmd( &testObj, CmdType::cmd1  );

    CHECK_EQUAL( 3, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, cmdNotAdded )
{
    cmdCtrl testCtrl;

    testCtrl.loadCmd( &testObj, CmdType::cmd1 );
    testCtrl.loadCmd( &testObj, CmdType::noCmd );
    testCtrl.loadCmd( &testObj, CmdType::cmd2 );

    CHECK_EQUAL( 2, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, txSort )
{
    cmdCtrl testCtrl;

    CHECK_TRUE( testCtrl.loadCmd( &obj1, CmdType::cmd1, CmdType::noCmd, PrioLevel::high ) );
    CHECK_TRUE( testCtrl.loadCmd( &obj2, CmdType::cmd1, CmdType::noCmd, PrioLevel::low ) );
    CHECK_TRUE( testCtrl.loadCmd( &obj3, CmdType::cmd1, CmdType::noCmd, PrioLevel::l2 ) );
    CHECK_TRUE( testCtrl.loadCmd( &obj4, CmdType::cmd1, CmdType::noCmd, PrioLevel::l2 ) );
    CHECK_TRUE( testCtrl.loadCmd( &obj5, CmdType::cmd1, CmdType::noCmd, PrioLevel::l3 ) );

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

    testCtrl.loadCmd(  &testObj, CmdType::cmd1 );
    testCtrl.loadCmd(  &testObj, CmdType::cmd1 );
    testCtrl.loadCmd(  &testObj, CmdType::cmd2 );

   CHECK_TRUE( testCtrl.manager() == 3 );
}

TEST( cmdCtrl, removeCmdByToken )
{
    cmdCtrl testCtrl;

    testCtrl.loadCmd(  &testObj, CmdType::cmd1, CmdType::noCmd, PrioLevel::low, 22222 );
    testCtrl.loadCmd(  &testObj, CmdType::cmd1, CmdType::noCmd, PrioLevel::low, 33333 );
    testCtrl.loadCmd(  &testObj, CmdType::cmd1, CmdType::noCmd, PrioLevel::low, 44444 );
    testCtrl.loadCmd(  &testObj, CmdType::cmd1, CmdType::noCmd, PrioLevel::low, 55555 );
    
    CHECK_EQUAL( 4, testCtrl.getCmdCnt() );

    CHECK_TRUE( testCtrl.removeCmdByToken( 33333 ) );
    CHECK_EQUAL( 3, testCtrl.getCmdCnt() );
    CHECK_TRUE( testCtrl.removeCmdByToken( 55555 ) );
    CHECK_EQUAL( 2, testCtrl.getCmdCnt() );

    /* false remove. */
    CHECK_FALSE( testCtrl.removeCmdByToken( 33333 ) );
    CHECK_EQUAL( 2, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, periodicManager )
{
    constexpr uint32_t TEST_PeriodMs = 10;
    cmdCtrl testCtrl;

    testCtrl.loadCmd( &testObj, CmdType::cmd1 );
    testCtrl.loadCmd( &testObj, CmdType::cmd1, CmdType::noCmd, PrioLevel::high, 0, cmdDefaultRetryNr, cmdDefaultTimeoutMs, TEST_PeriodMs, cmdDefaultDelayMs );
    testCtrl.loadCmd( &testObj, CmdType::cmd2 );

    CHECK_EQUAL( 3, testCtrl.manager() );
    CHECK_EQUAL( 3, testObj.getSendCnt() );
    CHECK_EQUAL( 1, testCtrl.getCmdCnt() ); /* Only periodic comand is still in the row. */

    /* Wait for new period to start. */
    while( testCtrl.manager() != 1 ) {};

    /* Measure period. */
    auto time1 = HAL_GetTick();
    while( testCtrl.manager() != 1 ) {};
    auto deltaTimeMs = HAL_GetTick() - time1;

    printf("\n\r DELTA: %u", deltaTimeMs );

    CHECK_TRUE( deltaTimeMs >= TEST_PeriodMs ); // && deltaTimeMs <= TEST_PeriodMs + 5 );

    /* Count periodic calls. */
    testObj.reset();
    CHECK_EQUAL( 0, testObj.getSendCnt() );
    time1 = HAL_GetTick();
    while( ( HAL_GetTick() - time1 ) < 3*TEST_PeriodMs + 5 ){ testCtrl.manager(); }
    CHECK_EQUAL( 3, testObj.getSendCnt() );
}

TEST( cmdCtrl, replyWrongCmd )
{
    cmdCtrl testCtrl;  
    constexpr uint32_t testTOKEN = 12345;

    myObj cmd1Obj;
    myObj replyObj;  

    cmd1Obj.reset();
    replyObj.reset();

    testCtrl.loadCmd( &cmd1Obj, CmdType::cmd1, CmdType::cmd2,  PrioLevel::low,  testTOKEN );

    /* Bring cmd1 in wait for reply state. */
    testCtrl.manager();
    CHECK_EQUAL( 1, cmd1Obj.getSendCnt() );

    /* Load wrong cmd reply (Cmd3 instead of Cmd2). */
    testCtrl.loadReply( &replyObj, CmdType::cmd3, CmdType::noCmd, PrioLevel::high, testTOKEN );
    CHECK_EQUAL( 2, testCtrl.getCmdCnt() );

    /* Close cmd1. */
    testCtrl.manager();

    CHECK_EQUAL( 1, cmd1Obj.getSendCnt() );
    CHECK_EQUAL( 0, cmd1Obj.getReplyCnt() );
    CHECK_EQUAL( 0, cmd1Obj.getTimeoutCnt() );

    CHECK_EQUAL( 1, replyObj.getSendCnt() );
    CHECK_EQUAL( 0, replyObj.getReplyCnt() );
    CHECK_EQUAL( 0, replyObj.getTimeoutCnt() );

    CHECK_EQUAL( 1, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, replyWrongToken )
{
    cmdCtrl testCtrl;  
    constexpr uint32_t testTOKEN = 12345;

    myObj cmd1Obj;
    myObj replyObj;  

    cmd1Obj.reset();
    replyObj.reset();

    testCtrl.loadCmd( &cmd1Obj, CmdType::cmd1, CmdType::cmd2,  PrioLevel::low,  testTOKEN );

    /* Bring cmd1 in wait for reply state. */
    testCtrl.manager();
    CHECK_EQUAL( 1, cmd1Obj.getSendCnt() );

    /* Load wrong cmd reply (worng token). */
    testCtrl.loadReply( &replyObj, CmdType::cmd2, CmdType::noCmd, PrioLevel::high, testTOKEN+1 );
    CHECK_EQUAL( 2, testCtrl.getCmdCnt() );

    /* Close cmd1. */
    testCtrl.manager();

    CHECK_EQUAL( 1, cmd1Obj.getSendCnt() );
    CHECK_EQUAL( 0, cmd1Obj.getReplyCnt() );
    CHECK_EQUAL( 0, cmd1Obj.getTimeoutCnt() );

    CHECK_EQUAL( 1, replyObj.getSendCnt() );
    CHECK_EQUAL( 0, replyObj.getReplyCnt() );
    CHECK_EQUAL( 0, replyObj.getTimeoutCnt() );

    CHECK_EQUAL( 1, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, replyCmd )
{
    cmdCtrl testCtrl;  

    myObj cmd1Obj;
    myObj replyObj;  

    cmd1Obj.reset();
    replyObj.reset();

    testCtrl.loadCmd( &cmd1Obj, CmdType::cmd1, CmdType::cmd2,  PrioLevel::low,  12345 );

    /* Bring cmd1 in wait for reply state. */
    testCtrl.manager();
    CHECK_EQUAL( 1, cmd1Obj.getSendCnt() );

    /* Load reply. */
    testCtrl.loadReply( &replyObj, CmdType::cmd2, CmdType::noCmd, PrioLevel::high, 12345 );
    CHECK_EQUAL( 2, testCtrl.getCmdCnt() );

    /* Close cmd1. */
    testCtrl.manager();

    CHECK_EQUAL( 1, cmd1Obj.getSendCnt() );
    CHECK_EQUAL( 1, cmd1Obj.getReplyCnt() );
    CHECK_EQUAL( 0, cmd1Obj.getTimeoutCnt() );

    CHECK_EQUAL( 1, replyObj.getSendCnt() );
    CHECK_EQUAL( 0, replyObj.getReplyCnt() );
    CHECK_EQUAL( 0, replyObj.getTimeoutCnt() );

    CHECK_EQUAL( 0, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, doNotRepeatOnTimeout )
{
    cmdCtrl testCtrl;  

    myObj cmd1Obj;
    myObj replyObj;  

    cmd1Obj.reset();
    replyObj.reset();

    constexpr uint32_t TEST_TimeoutMs = 10;

    testCtrl.loadCmd( &testObj, CmdType::cmd1, CmdType::noCmd, PrioLevel::high, 0, cmdDefaultRetryNr, TEST_TimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs );

    /* Wait timeout period. */
    auto time1 = HAL_GetTick();
    while( ( HAL_GetTick() - time1 ) <= ( TEST_TimeoutMs + 1 ) ) 
    {
        testCtrl.manager();
    }

    CHECK_EQUAL( 0, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, repeatOnTimeout )
{
    constexpr uint32_t TEST_TimeoutMs = 10;
    cmdCtrl testCtrl;  
    myObj cmd1Obj;

    cmd1Obj.reset();

    CHECK_EQUAL( 0, testCtrl.getCmdCnt() );

    testCtrl.loadCmd( &cmd1Obj, CmdType::cmd1, CmdType::cmd2, PrioLevel::high, 0, cmdDefaultRetryNr, TEST_TimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, CmdOption::RepeatOnTimeout );

    CHECK_EQUAL( 1, testCtrl.getCmdCnt() );
    CHECK_EQUAL( 0, cmd1Obj.getTimeoutCnt() );

    /* Wait timeout period. */
    auto time1 = HAL_GetTick();
    while( ( HAL_GetTick() - time1 ) <= ( TEST_TimeoutMs + 5 ) ) 
    {
        testCtrl.manager();
    }

    CHECK_EQUAL( 1, cmd1Obj.getTimeoutCnt() );
    CHECK_EQUAL( 1, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, repeatOnReply )
{
    constexpr uint32_t TEST_TimeoutMs = 10;
    constexpr uint32_t TEST_TOKEN = 462456;
    cmdCtrl testCtrl;  
    myObj cmd1Obj;

    cmd1Obj.reset();

    CHECK_EQUAL( 0, testCtrl.getCmdCnt() );

    testCtrl.loadCmd( &cmd1Obj, CmdType::cmd1, CmdType::cmd2, PrioLevel::high, TEST_TOKEN, cmdDefaultRetryNr, TEST_TimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, CmdOption::RepeatOnReply );

    testCtrl.manager();
    
    CHECK_EQUAL( 1, testCtrl.getCmdCnt() );
    CHECK_EQUAL( 0, cmd1Obj.getReplyCnt() );

    /* Load reply. */
    testCtrl.loadReply( nullptr, CmdType::cmd2, CmdType::noCmd, PrioLevel::high, TEST_TOKEN, cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs );

    CHECK_EQUAL( 2, testCtrl.getCmdCnt() );

    testCtrl.manager();

    CHECK_EQUAL( 1, cmd1Obj.getReplyCnt() );
    CHECK_EQUAL( 1, testCtrl.getCmdCnt() );
}

TEST( cmdCtrl, repeatForever )
{
    constexpr uint32_t LOOP_CNT_TEST = 10;
    cmdCtrl testCtrl;  
    myObj cmd1Obj;

    cmd1Obj.reset();

    CHECK_EQUAL( 0, cmd1Obj.getSendCnt() );

    testCtrl.loadCmd( &cmd1Obj, CmdType::cmd1, CmdType::noCmd, PrioLevel::high, 0, cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, CmdOption::RepeatForever );

    for( uint32_t i = 0; i < LOOP_CNT_TEST  ; ++i )
    {
        testCtrl.manager(); /* Send execution. */
        testCtrl.manager(); /* Done execution. */
    }
    
    CHECK_EQUAL( LOOP_CNT_TEST, cmd1Obj.getSendCnt() );
}
// #endif

TEST( cmdCtrl, retryCnt )
{
    constexpr uint32_t retryNR_TEST = 3;
    constexpr uint32_t timeoutMs_TEST = 1;
    constexpr uint32_t LOOP_CNT_TEST = 10*retryNR_TEST; /* Do it many times. */
    cmdCtrl testCtrl;  
    myObj cmd1Obj;

    cmd1Obj.reset();

    CHECK_EQUAL( 0, cmd1Obj.getSendCnt() );

    testCtrl.loadCmd( &cmd1Obj, CmdType::cmd1, CmdType::cmd2, PrioLevel::high, 0, cmdDefaultRetryNr, timeoutMs_TEST, cmdDefaultPeriodMs, cmdDefaultDelayMs );

    CHECK_EQUAL( 1, testCtrl.getCmdCnt() );

    for( uint32_t i = 0; i < LOOP_CNT_TEST  ; ++i )
    {
        testCtrl.manager(); /* Idle -> Sent. */

        auto time = HAL_GetTick();
        while( ( HAL_GetTick() - time ) < timeoutMs_TEST ) {}

        testCtrl.manager(); /* Wait for reply -> Timeout. */
        testCtrl.manager(); /* Done -> Idle. */
    }
    
    CHECK_EQUAL( retryNR_TEST + 1, cmd1Obj.getSendCnt() );
}

TEST( cmdCtrl, pingPong )
{
    constexpr uint32_t TEST_TOKEN = 462456;
    uint32_t testLoopCnt = 10;
    cmdCtrl ctrlPingPong;  
    myObj cmdPing;
    uint32_t replyCnt = 0;

    cmdPing.reset();

    CHECK_EQUAL( 0, ctrlPingPong.getCmdCnt() );
    
    ctrlPingPong.loadCmd( &cmdPing, CmdType::cmd1, CmdType::cmd2, PrioLevel::high, TEST_TOKEN, cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs, CmdOption::RepeatOnReply );

    while( testLoopCnt-- > 0 )
    {      
        ctrlPingPong.manager();     /* Idle -> Sent -> WaitForReply. */ 

        /* Load the reply. */   
        ctrlPingPong.loadReply( nullptr, CmdType::cmd2, CmdType::noCmd, PrioLevel::high, TEST_TOKEN, cmdDefaultRetryNr, cmdDefaultTimeoutMs, cmdDefaultPeriodMs, cmdDefaultDelayMs );
 
        ++replyCnt;
           
        ctrlPingPong.manager();     /* WaitForReply -> Done. */ 
        ctrlPingPong.manager();     /* Done         -> Idle. */

        CHECK_EQUAL( 1, ctrlPingPong.getCmdCnt() );
        CHECK_EQUAL( replyCnt, cmdPing.getReplyCnt() );
    }
}
