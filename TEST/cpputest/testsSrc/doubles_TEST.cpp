#include "CppUTest/TestHarness.h"

#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>


extern "C"
{
	/*
	 * Add your c-only include files here
	 */
    #include "stm32h7xx_hal.h"
}

using namespace std::chrono_literals;

TEST_GROUP( doubles )
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


TEST( doubles, getTick )
{
    auto time1 = HAL_GetTick( );
    
    std::this_thread::sleep_for(123ms);

    auto deltaTimeMs = HAL_GetTick( ) - time1;

    CHECK_TRUE( deltaTimeMs == 123 );

}