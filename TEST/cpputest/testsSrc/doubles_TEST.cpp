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

    auto deltaT = 123ms;
    
    std::this_thread::sleep_for(deltaT);

    auto dt = std::chrono::duration<int, std::milli>(HAL_GetTick( ) - time1);

    if( dt < deltaT )
    {
       std::cout << "Error double TEST -> deltaTimeMs = " << dt << std::endl;
    }

    CHECK_TRUE( ( dt >= deltaT ) && ( dt < ( deltaT + std::chrono::duration<int, std::milli>(10) ) ) );
}