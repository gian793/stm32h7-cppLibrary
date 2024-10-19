/**
 * \file            ringBuffer.h
 * \brief           Ring buffer.
 * \author          g.marcolin
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

/* #ifdef __cplusplus
extern "C" {
#endif */

#include <cassert>

template <class T>
class ringBuffer
{
    public:
		ringBuffer( T* pBuf, const size_t size ) : buf{ pBuf }, maxSize{ size }
        {
            assert( ( size % sizeof(T) ) == 0 );
            stm32_lock_init( &lock );
        }

        void reset( void );
        void put( const T item );           /* Add an item at the tail of the buffer. */
        void put( const T* itemBuf, const size_t itemCnt );     /* Put an array of new items in the buffer. */
        const T get( void );                /* Return the item at the head of the buffer. */
        bool isEmpty( void ) const;         /* If head and tail are equal, it is empty. */
        bool isFull( void ) const;      
        size_t capacity( void ) const;      
        size_t size( void ) const;          /* Current item count. */
        size_t space( void ) const;         /* Current free space count. */

    protected:
        LockingData_t lock;

    private:
		T* buf;
		
        const size_t maxSize;

        size_t head = 0;
        size_t tail = 0;

        bool isBufFull = false;
};

/* #ifdef __cplusplus
}
#endif */

#endif // RING_BUFFER_H_
