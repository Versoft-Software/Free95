/*
 * Free 95
 *
 * NAME:
 *		stdint.h
 *
 * DESCRIPTION:
 *		Types definition.
 *
 * Author: Rainy101112
 *
 */

#ifndef INCLUDE_STDINT_H_
#define INCLUDE_STDINT_H_

#ifdef __UINT8_TYPE__
typedef __UINT8_TYPE__ uint8_t;
#endif

#ifdef __UINT16_TYPE__
typedef __UINT16_TYPE__ uint16_t;
#endif

#ifdef __UINT32_TYPE__
typedef __UINT32_TYPE__ uint32_t;
#endif

#ifdef __UINT64_TYPE__
typedef __UINT64_TYPE__ uint64_t;
#endif

#ifdef __INT8_TYPE__
typedef __INT8_TYPE__ int8_t;
#endif

#ifdef __INT16_TYPE__
typedef __INT16_TYPE__ int16_t;
#endif

#ifdef __INT32_TYPE__
typedef __INT32_TYPE__ int32_t;
#endif

#ifdef __INT64_TYPE__
typedef __INT64_TYPE__ int64_t;
#endif

typedef __UINT_LEAST8_TYPE__ uint_least8_t;
typedef __UINT_LEAST16_TYPE__ uint_least16_t;
typedef __UINT_LEAST32_TYPE__ uint_least32_t;
typedef __UINT_LEAST64_TYPE__ uint_least64_t;
typedef __UINT_FAST8_TYPE__ uint_fast8_t;
typedef __UINT_FAST16_TYPE__ uint_fast16_t;
typedef __UINT_FAST32_TYPE__ uint_fast32_t;
typedef __UINT_FAST64_TYPE__ uint_fast64_t;
typedef __INT_LEAST8_TYPE__ int_least8_t;
typedef __INT_LEAST16_TYPE__ int_least16_t;
typedef __INT_LEAST32_TYPE__ int_least32_t;
typedef __INT_LEAST64_TYPE__ int_least64_t;
typedef __INT_FAST8_TYPE__ int_fast8_t;
typedef __INT_FAST16_TYPE__ int_fast16_t;
typedef __INT_FAST32_TYPE__ int_fast32_t;
typedef __INT_FAST64_TYPE__ int_fast64_t;

#ifdef __UINTPTR_TYPE__
typedef __UINTPTR_TYPE__ uintptr_t;
#endif

#ifdef __INTPTR_TYPE__
typedef __INTPTR_TYPE__ intptr_t;
#endif

typedef __UINTMAX_TYPE__ uintmax_t;
typedef __INTMAX_TYPE__ intmax_t;

typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;
typedef uint64_t uint64;

#endif // INCLUDE_STDINT_H_