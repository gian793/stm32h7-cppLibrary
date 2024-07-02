#include "CppUTest/TestHarness.h"

#include <iostream>
#include <cstring>

extern "C"
{
	/*
	 * Add your c-only include files here
	 */
}

#include "stm32_lock.h"
#include "ringBuffer.h"

TEST_GROUP( ringbuf )
{
	static constexpr size_t buf_SIZE = 4;

    void setup()
    {	
		MemoryLeakWarningPlugin::saveAndDisableNewDeleteOverloads();
    }

    void teardown()
    {
		MemoryLeakWarningPlugin::restoreNewDeleteOverloads();
    }
};

TEST( ringbuf, char_init)
{
	/*--------- Data ----------*/

	char buf[ buf_SIZE ] = {0};

	ringBuffer<char> rBuffer{ buf, buf_SIZE };
	
	/*------- Sequence --------*/
	
	CHECK_TRUE( rBuffer.isEmpty() );
	CHECK_EQUAL( 0, rBuffer.size() );
	CHECK_EQUAL( 0, rBuffer.get() );
	CHECK_EQUAL( buf_SIZE, rBuffer.capacity() );
}

TEST( ringbuf, int_init)
{
	/*--------- Data ----------*/

	uint32_t buf[ buf_SIZE ] = {0};

	ringBuffer<uint32_t> rBuffer{ buf, buf_SIZE };
	
	/*------- Squence --------*/

	CHECK_TRUE( rBuffer.isEmpty() );	/* <uint32_t> instance is valid */
}


TEST( ringbuf, put_item1)
{
	/*--------- Data ----------*/

	uint32_t buf[ buf_SIZE ] = {0};

	ringBuffer<uint32_t> rBuffer{ buf, buf_SIZE };

	const char cc = 'a';

	rBuffer.put( cc ); 	/* Put first item. */
	
	/*------- Sequence --------*/

	CHECK_FALSE( rBuffer.isEmpty() );
	CHECK_EQUAL( 1, rBuffer.size() );
	CHECK_EQUAL( cc, rBuffer.get() );
	CHECK_EQUAL( 0, rBuffer.size() );	/* After get the size is zero.*/
}

TEST( ringbuf, put_array)
{
	/*--------- Data ----------*/

	char buf[ buf_SIZE ] = {0};

	ringBuffer<char> rBuffer{ buf, buf_SIZE };

	const char ccbuf[] = {'a', 'b', 'c', 'd'};

	rBuffer.put( ccbuf,  sizeof(ccbuf)/sizeof(ccbuf[0]) ); 	
	
	/*------- Sequence --------*/

	CHECK_FALSE( rBuffer.isEmpty() );
	CHECK_TRUE( rBuffer.isFull() );

	CHECK_EQUAL( buf_SIZE, rBuffer.size() );

	CHECK_EQUAL( ccbuf[0], rBuffer.get() );	/* Get 1st item. */

	CHECK_FALSE( rBuffer.isFull() );

	CHECK_EQUAL( ccbuf[1], rBuffer.get() );	/* Get 2nd item. */
	CHECK_EQUAL( ccbuf[2], rBuffer.get() );	/* Get 3rd item. */

	CHECK_FALSE( rBuffer.isEmpty() );
	CHECK_EQUAL( ccbuf[3], rBuffer.get() );	/* Get 4th item. */
}


TEST( ringbuf, bufNotFull)
{
	/*--------- Data ----------*/

	uint32_t buf[ buf_SIZE ] = {0};

	ringBuffer<uint32_t> rBuffer{ buf, buf_SIZE };

	const uint32_t u32Buf[] = {1, 2, 3};

	rBuffer.put( u32Buf, sizeof(u32Buf)/sizeof(u32Buf[0])  ); 	
	
	/*------- Sequence --------*/

	CHECK_FALSE( rBuffer.isFull() );
}

TEST( ringbuf, bufRollOver)
{
	/*--------- Data ----------*/

	uint32_t buf[ buf_SIZE ] = {0};

	ringBuffer<uint32_t> rBuffer{ buf, buf_SIZE };

	const uint32_t u32Buf[ buf_SIZE + 1 ] = {1, 2, 3, 4, 5};

	const size_t u32BufferSize = sizeof(u32Buf)/sizeof(u32Buf[0]);

	rBuffer.put( u32Buf, u32BufferSize ); 	
	
	/*------- Sequence --------*/

	CHECK_TRUE( rBuffer.isFull() );
	CHECK_EQUAL( u32Buf[ 1 ], rBuffer.get() );
	CHECK_EQUAL( u32Buf[ 2 ], rBuffer.get() );
	CHECK_EQUAL( u32Buf[ 3 ], rBuffer.get() );
	CHECK_EQUAL( u32Buf[ 4 ], rBuffer.get() );

	CHECK_TRUE( rBuffer.isEmpty() );
}

TEST( ringbuf, reset)
{
	/*--------- Data ----------*/

	uint32_t buf[ buf_SIZE ] = {0};

	ringBuffer<uint32_t> rBuffer{ buf, buf_SIZE };

	const uint32_t u32Buf[] = {1, 2, 3};

	rBuffer.put( u32Buf, sizeof(u32Buf)/sizeof(u32Buf[0])  ); 	

	rBuffer.reset();
	
	/*------- Sequence --------*/

	CHECK_TRUE( rBuffer.isEmpty() );
	CHECK_EQUAL( 0, rBuffer.size() );
	CHECK_EQUAL( 0, rBuffer.get() );
}