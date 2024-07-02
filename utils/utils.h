/**
 * \file            ringBuffer.h
 * \brief           Ring buffer.
 * \author          g.marcolin
 */

#ifndef UTILS_H_
#define UTILS_H_

/*---------------------------------------------------------------------------*/

/* Functions definition in header file to avoid inline. */

/*---------------------------------------------------------------------------*/

 template<typename T>
 concept SupportsGreaterThan = requires (T x) { x > x; };

 template<typename T>
 bool isGreaterThan( const T& var1, const T& var2 )
 {
     return ( var1 > var2 );
 }

 /*---------------------------------------------------------------------------*/

 template<typename T>
 concept SupportsLessThan = requires (T x) { x < x; };

 template<typename T>
 requires std::copyable<T> && SupportsLessThan<T>
 bool getMax( const T& var1, const T& var2 )
 {
     return var2 < var1 ? var1 : var2;
 }

 /*---------------------------------------------------------------------------*/

 template<typename T>
 uint32_t sum( const T& collection )
 {
     uint32_t sum = 0;

     for( const auto& elem : collection )
     {
         sum += elem;
     }

     return sum;
 }

#endif // UTILS_H_
