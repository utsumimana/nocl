/*
 * Copyright (c) 2025 Mana Utsumi
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#if !defined(_NOCL_STDATOMIC_H)
#define _NOCL_STDATOMIC_H

#if defined(__cplusplus)

extern "C" {

#endif

#if defined(NOCL_HAS_STDATOMIC_H) || \
    /* C11 */ (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__)) || \
    /* C++23 */ (defined(__cplusplus) && __cplusplus >= 202302L)

#include <stdatomic.h>

#define atomic  _Atomic

#elif /* Win32 */ defined(_WIN32) && \
    /* MSC 1.0 */ defined(_MSC_VER) && \
    (defined(_M_IX86) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64))

#include "inttypes.h"
#include "stdbool.h"

#if defined(NOCL_FEATURE_NO_INTTYPES) || defined(NOCL_FEATURE_NO_STDBOOL)

#define NOCL_FEATURE_NO_STDATOMIC

#else

#if _MSC_VER >= 1500

#define __nocl_internal_stdatomic_pragma  __pragma

#else

#define __nocl_internal_stdatomic_pragma(text)

#endif

#include <intrin.h>

#define _Atomic  volatile

#define ATOMIC_BOOL_LOCK_FREE     2
#define ATOMIC_CHAR_LOCK_FREE     2
#define ATOMIC_SHORT_LOCK_FREE    2
#define ATOMIC_INT_LOCK_FREE      2
#define ATOMIC_LONG_LOCK_FREE     2

#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64)

#define ATOMIC_LLONG_LOCK_FREE    2

#else

#define ATOMIC_LLONG_LOCK_FREE    0

#endif

#define ATOMIC_POINTER_LOCK_FREE  2

#define ATOMIC_FLAG_INIT  { 0 }

#if /* C23 */ !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L

#define ATOMIC_VAR_INIT(value)  (value)

#endif

typedef enum __memory_order {
    memory_order_relaxed = 0,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst
} memory_order;

#pragma warning(push)
#pragma warning(disable:4996)

#define __nocl_internal_stdatomic_signal_barrier  _ReadWriteBarrier

#pragma warning(pop)

#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64)

#define __nocl_internal_stdatomic_thread_barrier()  __dmb(0xB)
#define __nocl_internal_stdatomic_signal_or_thread_barrier  __nocl_internal_stdatomic_thread_barrier

#if defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64)

#define __nocl_internal_stdatomic_load_acquire_barrier()  __dmb(0x9)

#else

#define __nocl_internal_stdatomic_load_acquire_barrier  __nocl_internal_stdatomic_thread_barrier

#endif

#elif defined(_M_IX86) || defined(_M_X64)

#define __nocl_internal_stdatomic_signal_or_thread_barrier  __nocl_internal_stdatomic_signal_barrier

#endif

#pragma warning(push)
#pragma warning(disable:6001) /* Using uninitialized memory '_Guard' */
#pragma warning(disable:28113) /* Accessing a local variable _Guard via an Interlocked function: This is an unusual usage which could be reconsidered. */

__forceinline __inline void __cdecl __nocl_internal_stdatomic_thread_fence(const memory_order mo) {

    if (mo == memory_order_relaxed) return;

#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64)

    if (mo == memory_order_acquire || mo == memory_order_consume)
        __nocl_internal_stdatomic_load_acquire_barrier();
    else
        __nocl_internal_stdatomic_thread_barrier();

#elif defined(_M_IX86) || defined(_M_X64)

    __nocl_internal_stdatomic_signal_barrier();
    if (mo == memory_order_seq_cst) {
        /* Not initialized to avoid an unnecessary operation; the value does not matter */
        volatile long _Guard;

        /*
         * _mm_mfence could have been used, but it is slower on some CPUs.
         * The memory fence provided by interlocked operations has some exceptions, but this is fine:
         * std::atomic_thread_fence works with respect to other atomics only; it may not be a full fence for all ops.
         */
        (void) _InterlockedIncrement(&_Guard);
        __nocl_internal_stdatomic_signal_barrier();
    }

#endif

}

__forceinline __inline void __cdecl __nocl_internal_stdatomic_signal_fence(const memory_order mo) {
    if (mo != memory_order_relaxed) __nocl_internal_stdatomic_signal_barrier();
}

#pragma warning(pop)

typedef _Atomic bool atomic_bool;
typedef _Atomic char atomic_char;
typedef _Atomic signed char atomic_schar;
typedef _Atomic unsigned char atomic_uchar;
typedef _Atomic signed short atomic_short;
typedef _Atomic unsigned short atomic_ushort;
typedef _Atomic signed int atomic_int;
typedef _Atomic unsigned int atomic_uint;
typedef _Atomic signed long atomic_long;
typedef _Atomic unsigned long atomic_ulong;
typedef _Atomic signed long long atomic_llong;
typedef _Atomic unsigned long long atomic_ullong;
typedef _Atomic wchar_t atomic_wchar_t;
typedef _Atomic int_least8_t atomic_int_least8_t;
typedef _Atomic uint_least8_t atomic_uint_least8_t;
typedef _Atomic int_least16_t atomic_int_least16_t;
typedef _Atomic uint_least16_t atomic_uint_least16_t;
typedef _Atomic int_least32_t atomic_int_least32_t;
typedef _Atomic uint_least32_t atomic_uint_least32_t;
typedef _Atomic int_least64_t atomic_int_least64_t;
typedef _Atomic uint_least64_t atomic_uint_least64_t;
typedef _Atomic int_fast8_t atomic_int_fast8_t;
typedef _Atomic uint_fast8_t atomic_uint_fast8_t;
typedef _Atomic int_fast16_t atomic_int_fast16_t;
typedef _Atomic uint_fast16_t atomic_uint_fast16_t;
typedef _Atomic int_fast32_t atomic_int_fast32_t;
typedef _Atomic uint_fast32_t atomic_uint_fast32_t;
typedef _Atomic int_fast64_t atomic_int_fast64_t;
typedef _Atomic uint_fast64_t atomic_uint_fast64_t;
typedef _Atomic intptr_t atomic_intptr_t;
typedef _Atomic uintptr_t atomic_uintptr_t;
typedef _Atomic size_t atomic_size_t;
typedef _Atomic ptrdiff_t atomic_ptrdiff_t;
typedef _Atomic intmax_t atomic_intmax_t;
typedef _Atomic uintmax_t atomic_uintmax_t;

typedef struct atomic_flag {
    atomic_bool _Val;
} atomic_flag;

__inline __int8 __cdecl __nocl_internal_stdatomic_exchange_8(volatile __int8 *obj, __int8 desired) {
    return _InterlockedExchange8(obj, desired);
}

__inline __int16 __cdecl __nocl_internal_stdatomic_exchange_16(volatile __int16 *obj, __int16 desired) {
    return _InterlockedExchange16(obj, desired);
}

__inline __int32 __cdecl __nocl_internal_stdatomic_exchange_32(volatile __int32 *obj, __int32 desired) {
    return _InterlockedExchange((volatile long *) obj, desired);
}

__inline void __cdecl __nocl_internal_stdatomic_store_8(volatile __int8 *obj, __int8 desired) {
    _InterlockedExchange8(obj, desired);
}

__inline void __cdecl __nocl_internal_stdatomic_store_16(volatile __int16 *obj, __int16 desired) {
    _InterlockedExchange16(obj, desired);
}

__inline void __cdecl __nocl_internal_stdatomic_store_32(volatile __int32 *obj, __int32 desired) {
    _InterlockedExchange((volatile long *) obj, desired);
}

__inline bool __cdecl __nocl_internal_stdatomic_compare_exchange_8(volatile __int8 *obj, __int8* expected, __int8 desired) {
    __int8 cmp = *expected;
    *expected = _InterlockedCompareExchange8(obj, desired, cmp);
    return *expected == cmp;
}

__inline bool __cdecl __nocl_internal_stdatomic_compare_exchange_16(volatile __int16 *obj, __int16* expected, __int16 desired) {
    __int16 cmp = *expected;
    *expected = _InterlockedCompareExchange16(obj, desired, cmp);
    return *expected == cmp;
}

__inline bool __cdecl __nocl_internal_stdatomic_compare_exchange_32(volatile __int32 *obj, __int32* expected, __int32 desired) {
    __int32 cmp = *expected;
    *expected = _InterlockedCompareExchange((volatile long *) obj, desired, cmp);
    return *expected == cmp;
}

__inline __int8 __cdecl __nocl_internal_stdatomic_load_8(volatile __int8 *obj) {
    __int8 val;
    __nocl_internal_stdatomic_thread_fence(memory_order_acquire);
    val = *obj;
    __nocl_internal_stdatomic_thread_fence(memory_order_release);
    return val;
}

__inline __int16 __cdecl __nocl_internal_stdatomic_load_16(volatile __int16 *obj) {
    __int16 val;
    __nocl_internal_stdatomic_thread_fence(memory_order_acquire);
    val = *obj;
    __nocl_internal_stdatomic_thread_fence(memory_order_release);
    return val;
}

__inline __int32 __cdecl __nocl_internal_stdatomic_load_32(volatile __int32 *obj) {
    __int32 val;
    __nocl_internal_stdatomic_thread_fence(memory_order_acquire);
    val = *obj;
    __nocl_internal_stdatomic_thread_fence(memory_order_release);
    return val;
}

__inline __int8 __cdecl __nocl_internal_stdatomic_load_8_explicit(__int8 volatile *obj, const memory_order mo) {
    __int8 val;
    __nocl_internal_stdatomic_thread_fence(mo);
    val = *obj;
    __nocl_internal_stdatomic_thread_fence(memory_order_release);
    return val;
}

__inline __int16 __cdecl __nocl_internal_stdatomic_load_16_explicit(__int16 volatile *obj, const memory_order mo) {
    __int16 val;
    __nocl_internal_stdatomic_thread_fence(mo);
    val = *obj;
    __nocl_internal_stdatomic_thread_fence(memory_order_release);
    return val;
}

__inline __int32 __cdecl __nocl_internal_stdatomic_load_32_explicit(__int32 volatile *obj, const memory_order mo) {
    __int32 val;
    __nocl_internal_stdatomic_thread_fence(mo);
    val = *obj;
    __nocl_internal_stdatomic_thread_fence(memory_order_release);
    return val;
}

__inline __int8 __cdecl __nocl_internal_stdatomic_exchange_add_8(volatile __int8 *obj, __int8 arg) {
    return _InterlockedExchangeAdd8(obj, arg);
}

__inline __int16 __cdecl __nocl_internal_stdatomic_exchange_add_16(volatile __int16 *obj, __int16 arg) {
    return _InterlockedExchangeAdd16(obj, arg);
}

__inline __int32 __cdecl __nocl_internal_stdatomic_exchange_add_32(volatile __int32 *obj, __int32 arg) {
    return _InterlockedExchangeAdd((volatile long *) obj, arg);
}

__inline __int8 __cdecl __nocl_internal_stdatomic_and_8(volatile __int8 *obj, __int8 mask) {
    return _InterlockedAnd8(obj, mask);
}

__inline __int16 __cdecl __nocl_internal_stdatomic_and_16(volatile __int16 *obj, __int16 mask) {
    return _InterlockedAnd16(obj, mask);
}

__inline __int32 __cdecl __nocl_internal_stdatomic_and_32(volatile __int32 *obj, __int32 mask) {
    return _InterlockedAnd((volatile long *) obj, mask);
}

__inline __int8 __cdecl __nocl_internal_stdatomic_or_8(volatile __int8 *obj, __int8 mask) {
    return _InterlockedOr8(obj, mask);
}

__inline __int16 __cdecl __nocl_internal_stdatomic_or_16(volatile __int16 *obj, __int16 mask) {
    return _InterlockedOr16(obj, mask);
}

__inline __int32 __cdecl __nocl_internal_stdatomic_or_32(volatile __int32 *obj, __int32 mask) {
    return _InterlockedOr((volatile long *) obj, mask);
}

__inline __int8 __cdecl __nocl_internal_stdatomic_xor_8(volatile __int8 *obj, __int8 mask) {
    return _InterlockedXor8(obj, mask);
}

__inline __int16 __cdecl __nocl_internal_stdatomic_xor_16(volatile __int16 *obj, __int16 mask) {
    return _InterlockedXor16(obj, mask);
}

__inline __int32 __cdecl __nocl_internal_stdatomic_xor_32(volatile __int32 *obj, __int32 mask) {
    return _InterlockedXor((volatile long *) obj, mask);
}

#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64)

__inline __int64 __cdecl __nocl_internal_stdatomic_exchange_64(volatile __int64 *obj, __int64 desired) {
    return _InterlockedExchange64(obj, desired);
}

__inline void __cdecl __nocl_internal_stdatomic_store_64(volatile __int64 *obj, __int64 desired) {
    _InterlockedExchange64(obj, desired);
}

__inline bool __cdecl __nocl_internal_stdatomic_compare_exchange_64(volatile __int64 *obj, __int64* expected, __int64 desired) {
    __int64 cmp = *expected;
    *expected = _InterlockedCompareExchange64(obj, desired, cmp);
    return *expected == cmp;
}

__inline __int64 __cdecl __nocl_internal_stdatomic_load_64(volatile __int64 *obj) {
    __int64 val;
    __nocl_internal_stdatomic_thread_fence(memory_order_acquire);
    val = *obj;
    __nocl_internal_stdatomic_thread_fence(memory_order_release);
    return val;
}

__inline __int64 __cdecl __nocl_internal_stdatomic_load_64_explicit(__int64 volatile *obj, const memory_order mo) {
    __int64 val;
    __nocl_internal_stdatomic_thread_fence(mo);
    val = *obj;
    __nocl_internal_stdatomic_thread_fence(memory_order_release);
    return val;
}

__inline __int64 __cdecl __nocl_internal_stdatomic_exchange_add_64(volatile __int64 *obj, __int64 arg) {
    return _InterlockedExchangeAdd64(obj, arg);
}

__inline __int64 __cdecl __nocl_internal_stdatomic_and_64(volatile __int64 *obj, __int64 mask) {
    return _InterlockedAnd64(obj, mask);
}

__inline __int64 __cdecl __nocl_internal_stdatomic_or_64(volatile __int64 *obj, __int64 mask) {
    return _InterlockedOr64(obj, mask);
}

__inline __int64 __cdecl __nocl_internal_stdatomic_xor_64(volatile __int64 *obj, __int64 mask) {
    return _InterlockedXor64(obj, mask);
}

#define atomic_exchange(obj,desired) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_exchange_8((obj), desired) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_exchange_16((obj), desired) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_exchange_32((obj), desired) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_exchange_64((obj), desired) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_store(obj,desired) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_store_8((obj), desired) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_store_16((obj), desired) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_store_32((obj), desired) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_store_64((obj), desired) : (void) NULL)))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_compare_exchange_strong(obj,expected,desired) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_compare_exchange_8((obj), expected, desired) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_compare_exchange_16((obj), expected, desired) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_compare_exchange_32((obj), expected, desired) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_compare_exchange_64((obj), expected, desired) : false)))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_compare_exchange_weak  atomic_compare_exchange_strong

#define atomic_fetch_add(obj,arg) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_exchange_add_8((obj), arg) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_exchange_add_16((obj), arg) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_exchange_add_32((obj), arg) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_exchange_add_64((obj), arg) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_fetch_sub(obj,arg)  atomic_fetch_add(obj, -(arg))

#define atomic_fetch_and(obj,mask) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_and_8((obj), mask) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_and_16((obj), mask) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_and_32((obj), mask) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_and_64((obj), mask) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_fetch_or(obj,mask) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_or_8((obj), mask) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_or_16((obj), mask) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_or_32((obj), mask) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_or_64((obj), mask) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_fetch_xor(obj,mask) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_xor_8((obj), mask) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_xor_16((obj), mask) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_xor_32((obj), mask) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_xor_64((obj), mask) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#else

#define atomic_exchange(obj,desired) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_exchange_8((obj), desired) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_exchange_16((obj), desired) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_exchange_32((obj), desired) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_store(obj,desired) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_store_8((obj), desired) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_store_16((obj), desired) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_store_32((obj), desired) : (void) NULL)))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_compare_exchange_strong(obj,expected,desired) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_compare_exchange_8((obj), expected, desired) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_compare_exchange_16((obj), expected, desired) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_compare_exchange_32((obj), expected, desired : false))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_compare_exchange_weak  atomic_compare_exchange_strong

#define atomic_fetch_add(obj,arg) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_exchange_add_8((obj), arg) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_exchange_add_16((obj), arg) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_exchange_add_32((obj), arg) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_fetch_sub(obj,arg)  atomic_fetch_add(obj, -(arg))

#define atomic_fetch_and(obj,mask) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_and_8((obj), mask) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_and_16((obj), mask) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_and_32((obj), mask) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_fetch_or(obj,mask) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_or_8((obj), mask) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_or_16((obj), mask) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_or_32((obj), mask) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_fetch_xor(obj,mask) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_xor_8((obj), mask) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_xor_16((obj), mask) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_xor_32((obj), mask) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#endif

#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64)

__inline __int8 __cdecl __nocl_internal_stdatomic_exchange_8_explicit(volatile __int8 *obj, __int8 desired, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedExchange8_nf(obj, desired);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedExchange8_acq(obj, desired);
        case memory_order_release:
            return _InterlockedExchange8_rel(obj, desired);
        default:
            return _InterlockedExchange8(obj, desired);
    }
}

__inline __int16 __cdecl __nocl_internal_stdatomic_exchange_16_explicit(volatile __int16 *obj, __int16 desired, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedExchange16_nf(obj, desired);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedExchange16_acq(obj, desired);
        case memory_order_release:
            return _InterlockedExchange16_rel(obj, desired);
        default:
            return _InterlockedExchange16(obj, desired);
    }
}

__inline __int32 __cdecl __nocl_internal_stdatomic_exchange_32_explicit(volatile __int32 *obj, __int32 desired, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedExchange_nf((volatile long *) obj, desired);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedExchange_acq((volatile long *) obj, desired);
        case memory_order_release:
            return _InterlockedExchange_rel((volatile long *) obj, desired);
        default:
            return _InterlockedExchange((volatile long *) obj, desired);
    }
}

__inline __int64 __cdecl __nocl_internal_stdatomic_exchange_64_explicit(volatile __int64 *obj, __int64 desired, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedExchange64_nf(obj, desired);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedExchange64_acq(obj, desired);
        case memory_order_release:
            return _InterlockedExchange64_rel(obj, desired);
        default:
            return _InterlockedExchange64(obj, desired);
    }
}

__inline __int8 __cdecl __nocl_internal_stdatomic_compare_exchange_8_explicit(volatile __int8 *obj, __int8 *expected, __int8 desired, const memory_order smo, const memory_order fmo) {
    (void) fmo;
    switch (smo) {
        case memory_order_relaxed:
            return _InterlockedCompareExchange8_nf(obj, *expected, desired);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedCompareExchange8_acq(obj, *expected, desired);
        case memory_order_release:
            return _InterlockedCompareExchange8_rel(obj, *expected, desired);
        default:
            return _InterlockedCompareExchange8(obj, *expected, desired);
    }
}

__inline __int16 __cdecl __nocl_internal_stdatomic_compare_exchange_16_explicit(volatile __int16 *obj, __int16 *expected, __int16 desired, const memory_order smo, const memory_order fmo) {
    (void) fmo;
    switch (smo) {
        case memory_order_relaxed:
            return _InterlockedCompareExchange16_nf(obj, *expected, desired);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedCompareExchange16_acq(obj, *expected, desired);
        case memory_order_release:
            return _InterlockedCompareExchange16_rel(obj, *expected, desired);
        default:
            return _InterlockedCompareExchange16(obj, *expected, desired);
    }
}

__inline __int32 __cdecl __nocl_internal_stdatomic_compare_exchange_32_explicit(volatile __int32 *obj, __int32 *expected, __int32 desired, const memory_order smo, const memory_order fmo) {
    (void) fmo;
    switch (smo) {
        case memory_order_relaxed:
            return _InterlockedCompareExchange_nf((volatile long *) obj, *expected, desired);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedCompareExchange_acq((volatile long *) obj, *expected, desired);
        case memory_order_release:
            return _InterlockedCompareExchange_rel((volatile long *) obj, *expected, desired);
        default:
            return _InterlockedCompareExchange((volatile long *) obj, *expected, desired);
    }
}

__inline __int64 __cdecl __nocl_internal_stdatomic_compare_exchange_64_explicit(volatile __int64 *obj, __int64 *expected, __int64 desired, const memory_order smo, const memory_order fmo) {
    (void) fmo;
    switch (smo) {
        case memory_order_relaxed:
            return _InterlockedCompareExchange64_nf(obj, *expected, desired);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedCompareExchange64_acq(obj, *expected, desired);
        case memory_order_release:
            return _InterlockedCompareExchange64_rel(obj, *expected, desired);
        default:
            return _InterlockedCompareExchange64(obj, *expected, desired);
    }
}

__inline __int8 __cdecl __nocl_internal_stdatomic_exchange_add_8_explicit(__int8 volatile *obj, __int8 arg, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedExchangeAdd8_nf(obj, arg);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedExchangeAdd8_acq(obj, arg);
        case memory_order_release:
            return _InterlockedExchangeAdd8_rel(obj, arg);
        default:
            return _InterlockedExchangeAdd8(obj, arg);
    }
}

__inline __int16 __cdecl __nocl_internal_stdatomic_exchange_add_16_explicit(__int16 volatile *obj, __int16 arg, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedExchangeAdd16_nf(obj, arg);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedExchangeAdd16_acq(obj, arg);
        case memory_order_release:
            return _InterlockedExchangeAdd16_rel(obj, arg);
        default:
            return _InterlockedExchangeAdd16(obj, arg);
    }
}

__inline __int32 __cdecl __nocl_internal_stdatomic_exchange_add_32_explicit(__int32 volatile *obj, __int32 arg, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedExchangeAdd_nf((volatile long *) obj, arg);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedExchangeAdd_acq((volatile long *) obj, arg);
        case memory_order_release:
            return _InterlockedExchangeAdd_rel((volatile long *) obj, arg);
        default:
            return _InterlockedExchangeAdd((volatile long *) obj, arg);
    }
}

__inline __int64 __cdecl __nocl_internal_stdatomic_exchange_add_64_explicit(__int64 volatile *obj, __int64 arg, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedExchangeAdd64_nf(obj, arg);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedExchangeAdd64_acq(obj, arg);
        case memory_order_release:
            return _InterlockedExchangeAdd64_rel(obj, arg);
        default:
            return _InterlockedExchangeAdd64(obj, arg);
    }
}

__inline __int8 __cdecl __nocl_internal_stdatomic_and_8_explicit(__int8 volatile *obj, __int8 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedAnd8_nf(obj, mask);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedAnd8_acq(obj, mask);
        case memory_order_release:
            return _InterlockedAnd8_rel(obj, mask);
        default:
            return _InterlockedAnd8(obj, mask);
    }
}

__inline __int16 __cdecl __nocl_internal_stdatomic_and_16_explicit(__int16 volatile *obj, __int16 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedAnd16_nf(obj, mask);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedAnd16_acq(obj, mask);
        case memory_order_release:
            return _InterlockedAnd16_rel(obj, mask);
        default:
            return _InterlockedAnd16(obj, mask);
    }
}

__inline __int32 __cdecl __nocl_internal_stdatomic_and_32_explicit(__int32 volatile *obj, __int32 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedAnd_nf((volatile long *) obj, mask);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedAnd_acq((volatile long *) obj, mask);
        case memory_order_release:
            return _InterlockedAnd_rel((volatile long *) obj, mask);
        default:
            return _InterlockedAnd((volatile long *) obj, mask);
    }
}

__inline __int64 __cdecl __nocl_internal_stdatomic_and_64_explicit(__int64 volatile *obj, __int64 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedAnd64_nf(obj, mask);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedAnd64_acq(obj, mask);
        case memory_order_release:
            return _InterlockedAnd64_rel(obj, mask);
        default:
            return _InterlockedAnd64(obj, mask);
    }
}

__inline __int8 __cdecl __nocl_internal_stdatomic_or_8_explicit(__int8 volatile *obj, __int8 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedOr8_nf(obj, mask);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedOr8_acq(obj, mask);
        case memory_order_release:
            return _InterlockedOr8_rel(obj, mask);
        default:
            return _InterlockedOr8(obj, mask);
    }
}

__inline __int16 __cdecl __nocl_internal_stdatomic_or_16_explicit(__int16 volatile *obj, __int16 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedOr16_nf(obj, mask);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedOr16_acq(obj, mask);
        case memory_order_release:
            return _InterlockedOr16_rel(obj, mask);
        default:
            return _InterlockedOr16(obj, mask);
    }
}

__inline __int32 __cdecl __nocl_internal_stdatomic_or_32_explicit(__int32 volatile *obj, __int32 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedOr_nf((volatile long *) obj, mask);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedOr_acq((volatile long *) obj, mask);
        case memory_order_release:
            return _InterlockedOr_rel((volatile long *) obj, mask);
        default:
            return _InterlockedOr((volatile long *) obj, mask);
    }
}

__inline __int64 __cdecl __nocl_internal_stdatomic_or_64_explicit(__int64 volatile *obj, __int64 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedOr64_nf(obj, mask);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedOr64_acq(obj, mask);
        case memory_order_release:
            return _InterlockedOr64_rel(obj, mask);
        default:
            return _InterlockedOr64(obj, mask);
    }
}

__inline __int8 __cdecl __nocl_internal_stdatomic_xor_8_explicit(__int8 volatile *obj, __int8 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedXor8_nf(obj, mask);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedXor8_acq(obj, mask);
        case memory_order_release:
            return _InterlockedXor8_rel(obj, mask);
        default:
            return _InterlockedXor8(obj, mask);
    }
}

__inline __int16 __cdecl __nocl_internal_stdatomic_xor_16_explicit(__int16 volatile *obj, __int16 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedXor16_nf(obj, mask);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedXor16_acq(obj, mask);
        case memory_order_release:
            return _InterlockedXor16_rel(obj, mask);
        default:
            return _InterlockedXor16(obj, mask);
    }
}

__inline __int32 __cdecl __nocl_internal_stdatomic_xor_32_explicit(__int32 volatile *obj, __int32 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedXor_nf((volatile long *) obj, mask);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedXor_acq((volatile long *) obj, mask);
        case memory_order_release:
            return _InterlockedXor_rel((volatile long *) obj, mask);
        default:
            return _InterlockedXor((volatile long *) obj, mask);
    }
}

__inline __int64 __cdecl __nocl_internal_stdatomic_xor_64_explicit(__int64 volatile *obj, __int64 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            return _InterlockedXor64_nf(obj, mask);
        case memory_order_consume:
        case memory_order_acquire:
            return _InterlockedXor64_acq(obj, mask);
        case memory_order_release:
            return _InterlockedXor64_rel(obj, mask);
        default:
            return _InterlockedXor64(obj, mask);
    }
}

#elif defined(_M_IX86) || defined(_M_X64)

__inline __int8 __cdecl __nocl_internal_stdatomic_exchange_8_explicit(volatile __int8 *obj, __int8 desired, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int8 prev = *obj;
            *obj = desired;
            return prev;
        default:
            return _InterlockedExchange8(obj, desired);
    }
}

__inline __int16 __cdecl __nocl_internal_stdatomic_exchange_16_explicit(volatile __int16 *obj, __int16 desired, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int16 prev = *obj;
            *obj = desired;
            return prev;
        default:
            return _InterlockedExchange16(obj, desired);
    }
}

__inline __int32 __cdecl __nocl_internal_stdatomic_exchange_32_explicit(volatile __int32 *obj, __int32 desired, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int32 prev = *obj;
            *obj = desired;
            return prev;
        default:
            return _InterlockedExchange((volatile long *) obj, desired);
    }
}

__inline bool __cdecl __nocl_internal_stdatomic_compare_exchange_8_explicit(volatile __int8 *obj, __int8 *expected, __int8 desired, const memory_order smo, const memory_order fmo) {
    if (smo == memory_order_relaxed && fmo == memory_order_relaxed) {
        if (*obj == *expected) return true;
        else {
            *expected = *obj;
            return false;
        }
    }
    return _InterlockedCompareExchange8(obj, *expected, desired);
}

__inline __int16 __cdecl __nocl_internal_stdatomic_compare_exchange_16_explicit(volatile __int16 *obj, __int16 *expected, __int16 desired, const memory_order smo, const memory_order fmo) {
    if (smo == memory_order_relaxed && fmo == memory_order_relaxed) {
        if (*obj == *expected) return true;
        else {
            *expected = *obj;
            return false;
        }
    }
    return _InterlockedCompareExchange16(obj, *expected, desired);
}

__inline __int32 __cdecl __nocl_internal_stdatomic_compare_exchange_32_explicit(volatile __int32 *obj, __int32 *expected, __int32 desired, const memory_order smo, const memory_order fmo) {
    if (smo == memory_order_relaxed && fmo == memory_order_relaxed) {
        if (*obj == *expected) return true;
        else {
            *expected = *obj;
            return false;
        }
    }
    return _InterlockedCompareExchange((volatile long *) obj, *expected, desired);
}

__inline __int8 __cdecl __nocl_internal_stdatomic_exchange_add_8_explicit(volatile __int8 *obj, __int8 arg, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int8 prev = *obj;
            *obj += arg;
            return prev;
        default:
            return _InterlockedExchangeAdd8(obj, arg);
    }
}

__inline __int16 __cdecl __nocl_internal_stdatomic_exchange_add_16_explicit(volatile __int16 *obj, __int16 arg, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int16 prev = *obj;
            *obj += arg;
            return prev;
        default:
            return _InterlockedExchangeAdd16(obj, arg);
    }
}

__inline __int32 __cdecl __nocl_internal_stdatomic_exchange_add_32_explicit(volatile __int32 *obj, __int32 arg, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int32 prev = *obj;
            *obj += arg;
            return prev;
        default:
            return _InterlockedExchangeAdd((volatile long *) obj, arg);
    }
}

__inline __int8 __cdecl __nocl_internal_stdatomic_and_8_explicit(volatile __int8 *obj, __int8 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int8 prev = *obj;
            *obj &= mask;
            return prev;
        default:
            return _InterlockedAnd8(obj, mask);
    }
}

__inline __int16 __cdecl __nocl_internal_stdatomic_and_16_explicit(volatile __int16 *obj, __int16 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int16 prev = *obj;
            *obj &= mask;
            return prev;
        default:
            return _InterlockedAnd16(obj, mask);
    }
}

__inline __int32 __cdecl __nocl_internal_stdatomic_and_32_explicit(volatile __int32 *obj, __int32 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int32 prev = *obj;
            *obj &= mask;
            return prev;
        default:
            return _InterlockedAnd((volatile long *) obj, mask);
    }
}

__inline __int8 __cdecl __nocl_internal_stdatomic_or_8_explicit(volatile __int8 *obj, __int8 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int8 prev = *obj;
            *obj |= mask;
            return prev;
        default:
            return _InterlockedOr8(obj, mask);
    }
}

__inline __int16 __cdecl __nocl_internal_stdatomic_or_16_explicit(volatile __int16 *obj, __int16 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int16 prev = *obj;
            *obj |= mask;
            return prev;
        default:
            return _InterlockedOr16(obj, mask);
    }
}

__inline __int32 __cdecl __nocl_internal_stdatomic_or_32_explicit(volatile __int32 *obj, __int32 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int32 prev = *obj;
            *obj |= mask;
            return prev;
        default:
            return _InterlockedOr((volatile long *) obj, mask);
    }
}

__inline __int8 __cdecl __nocl_internal_stdatomic_xor_8_explicit(volatile __int8 *obj, __int8 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int8 prev = *obj;
            *obj ^= mask;
            return prev;
        default:
            return _InterlockedXor8(obj, mask);
    }
}

__inline __int16 __cdecl __nocl_internal_stdatomic_xor_16_explicit(volatile __int16 *obj, __int16 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int16 prev = *obj;
            *obj ^= mask;
            return prev;
        default:
            return _InterlockedXor16(obj, mask);
    }
}

__inline __int32 __cdecl __nocl_internal_stdatomic_xor_32_explicit(volatile __int32 *obj, __int32 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int32 prev = *obj;
            *obj ^= mask;
            return prev;
        default:
            return _InterlockedXor((volatile long *) obj, mask);
    }
}

#if defined(_M_X64)

__inline __int64 __cdecl __nocl_internal_stdatomic_exchange_64_explicit(volatile __int64 *obj, __int64 desired, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int64 prev = *obj;
            *obj = desired;
            return prev;
        default:
            return _InterlockedExchange64(obj, desired);
    }
}

__inline __int64 __cdecl __nocl_internal_stdatomic_compare_exchange_64_explicit(__int64 volatile *obj, __int64 *expected, __int64 desired, const memory_order smo, const memory_order fmo) {
    if (smo == memory_order_relaxed && fmo == memory_order_relaxed) {
        if (*obj == *expected) return true;
        else {
            *expected = *obj;
            return false;
        }
    }
    return _InterlockedCompareExchange64(obj, *expected, desired);
}

__inline __int64 __cdecl __nocl_internal_stdatomic_exchange_add_64_explicit(volatile __int64 *obj, __int64 arg, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int64 prev = *obj;
            *obj += arg;
            return prev;
        default:
            return _InterlockedExchangeAdd64(obj, arg);
    }
}

__inline __int64 __cdecl __nocl_internal_stdatomic_and_64_explicit(volatile __int64 *obj, __int64 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int64 prev = *obj;
            *obj &= mask;
            return prev;
        default:
            return _InterlockedAnd64(obj, mask);
    }
}

__inline __int64 __cdecl __nocl_internal_stdatomic_or_64_explicit(volatile __int64 *obj, __int64 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int64 prev = *obj;
            *obj |= mask;
            return prev;
        default:
            return _InterlockedOr64(obj, mask);
    }
}

__inline __int64 __cdecl __nocl_internal_stdatomic_xor_64_explicit(volatile __int64 *obj, __int64 mask, const memory_order mo) {
    switch (mo) {
        case memory_order_relaxed:
            const __int64 prev = *obj;
            *obj ^= mask;
            return prev;
        default:
            return _InterlockedXor64(obj, mask);
    }
}

#endif

#endif

#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64)

#define atomic_exchange_explicit(obj,desired,mo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_exchange_8_explicit((obj), desired, mo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_exchange_16_explicit((obj), desired, mo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_exchange_32_explicit((obj), desired, mo) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_exchange_64_explicit((obj), desired, mo) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_store_explicit(obj,desired,mo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_store_8_explicit((obj), desired, mo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_store_16_explicit((obj), desired, mo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_store_32_explicit((obj), desired, mo) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_store_64_explicit((obj), desired, mo) : (void) NULL)))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_compare_exchange_strong_explicit(obj,expected,desired,smo,fmo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_compare_exchange_8_explicit((obj), expected, desired, smo, fmo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_compare_exchange_16_explicit((obj), expected, desired, smo, fmo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_compare_exchange_32_explicit((obj), expected, desired, smo, fmo) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_compare_exchange_64_explicit((obj), expected, desired, smo, fmo) : false)))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_compare_exchange_weak_explicit  atomic_compare_exchange_strong_explicit

#define atomic_fetch_add_explicit(obj,arg,mo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_exchange_add_8_explicit((obj), arg, mo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_exchange_add_16_explicit((obj), arg, mo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_exchange_add_32_explicit((obj), arg, mo) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_exchange_add_64_explicit((obj), arg, mo) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_fetch_sub_explicit(obj,arg,mo)  atomic_fetch_add_explicit(obj, -(arg), mo)

#define atomic_fetch_and_explicit(obj,mask,mo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_and_8_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_and_16_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_and_32_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_and_64_explicit((obj), mask, mo) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_fetch_or_explicit(obj,mask,mo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_or_8_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_or_16_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_or_32_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_or_64_explicit((obj), mask, mo) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_fetch_xor_explicit(obj,mask,mo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_xor_8_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_xor_16_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_xor_32_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 8) ? __nocl_internal_stdatomic_xor_64_explicit((obj), mask, mo) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#else

#define atomic_exchange_explicit(obj,desired,mo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_exchange_8_explicit((obj), desired, mo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_exchange_16_explicit((obj), desired, mo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_exchange_32_explicit((obj), desired, mo) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_store_explicit(obj,desired,mo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_store_8_explicit((obj), desired, mo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_store_16_explicit((obj), desired, mo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_store_32_explicit((obj), desired, mo) : (void) NULL)))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_compare_exchange_strong_explicit(obj,expected,desired,smo,fmo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_compare_exchange_8_explicit((obj), expected, desired, smo, fmo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_compare_exchange_16_explicit((obj), expected, desired, smo, fmo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_compare_exchange_32_explicit((obj), expected, desired, smo, fmo) : false)))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_compare_exchange_weak  atomic_compare_exchange_strong_explicit

#define atomic_fetch_add_explicit(obj,arg,mo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_exchange_add_8_explicit((obj), arg, mo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_exchange_add_16_explicit((obj), arg, mo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_exchange_add_32_explicit((obj), arg, mo) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_fetch_sub_explicit(obj,arg,mo)  atomic_fetch_add_explicit(obj, -(arg), mo)

#define atomic_fetch_and_explicit(obj,mask,mo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_and_8_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_and_16_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_and_32_explicit((obj), mask, mo) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_fetch_or_explicit(obj,mask,mo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_or_8_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_or_16_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_or_32_explicit((obj), mask, mo) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#define atomic_fetch_xor_explicit(obj,mask,mo) \
    __nocl_internal_stdatomic_pragma(warning (push)) \
    __nocl_internal_stdatomic_pragma(warning (disable:4133)) \
    ((sizeof(*(obj)) == 1) ? __nocl_internal_stdatomic_xor_8_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 2) ? __nocl_internal_stdatomic_xor_16_explicit((obj), mask, mo) : \
    ((sizeof(*(obj)) == 4) ? __nocl_internal_stdatomic_xor_32_explicit((obj), mask, mo) : *(obj))))) \
    __nocl_internal_stdatomic_pragma(warning (pop))

#endif

#define atomic_init(obj,val)  __nocl_internal_stdatomic_store_explicit(obj,val,memory_order_relaxed)

#define atomic_is_lock_free(obj) \
    ((bool) !!((sizeof(*(obj)) == 1) || (sizeof(*(obj)) == 2) || (sizeof(*(obj)) == 4) || (sizeof(*(obj)) == 8)))

__inline bool __cdecl atomic_flag_test_and_set(volatile atomic_flag* obj) {
    bool expected = false;
    return !(atomic_compare_exchange_strong(&obj->_Val, &expected, true));
}

__inline void __cdecl atomic_flag_clear(volatile atomic_flag* obj) {
    atomic_exchange_explicit(&obj->_Val, false, memory_order_release);
}

__inline bool __cdecl atomic_flag_test_and_set_explicit(volatile atomic_flag* obj, const memory_order mo) {
    bool expected = false;
    return !(atomic_compare_exchange_strong_explicit(&obj->_Val, &expected, true, mo, mo));
}

__inline void __cdecl atomic_flag_clear_explicit(volatile atomic_flag* obj, const memory_order mo) {
    atomic_exchange_explicit(&obj->_Val, false, mo);
}

#define kill_dependency(y)  (y)

#define atomic_thread_fence  __nocl_internal_stdatomic_thread_fence
#define atomic_signal_fence  __nocl_internal_stdatomic_signal_fence

#endif

#elif /* GCC 4.7.0 */ (defined(__GNUC__) && (__GNUC__ >= 5 || (defined(__GNUC_MINOR__) && __GNUC__ == 4 && __GNUC_MINOR__ >= 7)))

#include "inttypes.h"
#include "stdbool.h"

#if defined(NOCL_FEATURE_NO_INTTYPES) || defined(NOCL_FEATURE_NO_STDBOOL)

#define NOCL_FEATURE_NO_STDATOMIC

#else

#define _Atomic  volatile
#define atomic   _Atomic

#define ATOMIC_BOOL_LOCK_FREE     1
#define ATOMIC_CHAR_LOCK_FREE     1
#define ATOMIC_SHORT_LOCK_FREE    1
#define ATOMIC_INT_LOCK_FREE      1
#define ATOMIC_LONG_LOCK_FREE     1
#define ATOMIC_LLONG_LOCK_FREE    1
#define ATOMIC_POINTER_LOCK_FREE  2

#define ATOMIC_FLAG_INIT  0

#if /* C23 */ !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L

#define ATOMIC_VAR_INIT(value)  (value)

#endif

typedef enum __memory_order {
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_consume = __ATOMIC_CONSUME,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST
} memory_order;

typedef _Atomic bool atomic_bool;
typedef _Atomic char atomic_char;
typedef _Atomic signed char atomic_schar;
typedef _Atomic unsigned char atomic_uchar;
typedef _Atomic signed short atomic_short;
typedef _Atomic unsigned short atomic_ushort;
typedef _Atomic signed int atomic_int;
typedef _Atomic unsigned int atomic_uint;
typedef _Atomic signed long atomic_long;
typedef _Atomic unsigned long atomic_ulong;
typedef _Atomic signed long long atomic_llong;
typedef _Atomic unsigned long long atomic_ullong;
typedef _Atomic wchar_t atomic_wchar_t;
typedef _Atomic int_least8_t atomic_int_least8_t;
typedef _Atomic uint_least8_t atomic_uint_least8_t;
typedef _Atomic int_least16_t atomic_int_least16_t;
typedef _Atomic uint_least16_t atomic_uint_least16_t;
typedef _Atomic int_least32_t atomic_int_least32_t;
typedef _Atomic uint_least32_t atomic_uint_least32_t;
typedef _Atomic int_least64_t atomic_int_least64_t;
typedef _Atomic uint_least64_t atomic_uint_least64_t;
typedef _Atomic int_fast8_t atomic_int_fast8_t;
typedef _Atomic uint_fast8_t atomic_uint_fast8_t;
typedef _Atomic int_fast16_t atomic_int_fast16_t;
typedef _Atomic uint_fast16_t atomic_uint_fast16_t;
typedef _Atomic int_fast32_t atomic_int_fast32_t;
typedef _Atomic uint_fast32_t atomic_uint_fast32_t;
typedef _Atomic int_fast64_t atomic_int_fast64_t;
typedef _Atomic uint_fast64_t atomic_uint_fast64_t;
typedef _Atomic intptr_t atomic_intptr_t;
typedef _Atomic uintptr_t atomic_uintptr_t;
typedef _Atomic size_t atomic_size_t;
typedef _Atomic ptrdiff_t atomic_ptrdiff_t;
typedef _Atomic intmax_t atomic_intmax_t;
typedef _Atomic uintmax_t atomic_uintmax_t;

typedef atomic_int atomic_flag;

#define atomic_flag_test_and_set(obj)  __atomic_exchange_n((obj), 1, memory_order_seq_cst)
#define atomic_flag_clear(obj)         __atomic_store_n((obj), 0, memory_order_seq_cst)

#define atomic_flag_test_and_set_explicit(obj,mo)  __atomic_exchange_n((obj), 1, (mo))
#define atomic_flag_clear_explicit(obj,mo)         __atomic_store_n((obj), 0, (mo))

#define atomic_thread_fence  __atomic_thread_fence
#define atomic_signal_fence  __atomic_signal_fence

#define atomic_is_lock_free(obj)  __atomic_is_lock_free(sizeof(*(obj)), (obj))

#define atomic_store(obj,desired)     __atomic_store_n((obj), (desired), memory_order_seq_cst)
#define atomic_load(obj,desired)      __atomic_load_n((obj), (desired), memory_order_seq_cst)
#define atomic_exchange(obj,desired)  __atomic_exchange_n((obj), (desired), memory_order_seq_cst)

#define atomic_store_explicit     __atomic_store_n
#define atomic_load_explicit      __atomic_load_n
#define atomic_exchange_explicit  __atomic_exchange_n

#define atomic_compare_exchange_strong(obj,expected,desired)  __atomic_compare_exchange_n((obj), (expected), (desired), false, memory_order_seq_cst, memory_order_seq_cst)
#define atomic_compare_exchange_weak(obj,expected,desired)    __atomic_compare_exchange_n((obj), (expected), (desired), true, memory_order_seq_cst, memory_order_seq_cst)

#define atomic_compare_exchange_strong_explicit(obj,expected,desired,smo,fmo)  __atomic_compare_exchange_n((obj), (expected), (desired), false, (smo), (fmo))
#define atomic_compare_exchange_weak_explicit(obj,expected,desired,smo,fmo)    __atomic_compare_exchange_n((obj), (expected), (desired), true, (smo), (fmo))

#define atomic_fetch_add(obj,arg)   __atomic_fetch_add((obj), (arg), memory_order_seq_cst)
#define atomic_fetch_sub(obj,arg)   __atomic_fetch_sub((obj), (arg), memory_order_seq_cst)
#define atomic_fetch_and(obj,mask)  __atomic_fetch_and((obj), (mask), memory_order_seq_cst)
#define atomic_fetch_or(obj,mask)   __atomic_fetch_or((obj), (mask), memory_order_seq_cst)
#define atomic_fetch_xor(obj,mask)  __atomic_fetch_xor((obj), (mask), memory_order_seq_cst)

#define atomic_fetch_add_explicit  __atomic_fetch_add
#define atomic_fetch_sub_explicit  __atomic_fetch_sub
#define atomic_fetch_and_explicit  __atomic_fetch_and
#define atomic_fetch_or_explicit   __atomic_fetch_or
#define atomic_fetch_xor_explicit  __atomic_fetch_xor

#endif

#elif /* GCC 4.1.0 */ (defined(__GNUC__) && (__GNUC__ >= 5 || (defined(__GNUC_MINOR__) && __GNUC__ == 4 && __GNUC_MINOR__ >= 1)))

#include "inttypes.h"
#include "stdbool.h"

#if defined(NOCL_FEATURE_NO_INTTYPES) || defined(NOCL_FEATURE_NO_STDBOOL)

#define NOCL_FEATURE_NO_STDATOMIC

#else

#define _Atomic  volatile
#define atomic   _Atomic

#define ATOMIC_BOOL_LOCK_FREE     1
#define ATOMIC_CHAR_LOCK_FREE     1
#define ATOMIC_SHORT_LOCK_FREE    1
#define ATOMIC_INT_LOCK_FREE      1
#define ATOMIC_LONG_LOCK_FREE     1
#define ATOMIC_LLONG_LOCK_FREE    1
#define ATOMIC_POINTER_LOCK_FREE  2

#define ATOMIC_FLAG_INIT  0

#if /* C23 */ !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L

#define ATOMIC_VAR_INIT(value)  (value)

#endif

typedef enum __memory_order {
    memory_order_relaxed = 0,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst
} memory_order;

typedef _Atomic bool atomic_bool;
typedef _Atomic char atomic_char;
typedef _Atomic signed char atomic_schar;
typedef _Atomic unsigned char atomic_uchar;
typedef _Atomic signed short atomic_short;
typedef _Atomic unsigned short atomic_ushort;
typedef _Atomic signed int atomic_int;
typedef _Atomic unsigned int atomic_uint;
typedef _Atomic signed long atomic_long;
typedef _Atomic unsigned long atomic_ulong;
typedef _Atomic signed long long atomic_llong;
typedef _Atomic unsigned long long atomic_ullong;
typedef _Atomic wchar_t atomic_wchar_t;
typedef _Atomic int_least8_t atomic_int_least8_t;
typedef _Atomic uint_least8_t atomic_uint_least8_t;
typedef _Atomic int_least16_t atomic_int_least16_t;
typedef _Atomic uint_least16_t atomic_uint_least16_t;
typedef _Atomic int_least32_t atomic_int_least32_t;
typedef _Atomic uint_least32_t atomic_uint_least32_t;
typedef _Atomic int_least64_t atomic_int_least64_t;
typedef _Atomic uint_least64_t atomic_uint_least64_t;
typedef _Atomic int_fast8_t atomic_int_fast8_t;
typedef _Atomic uint_fast8_t atomic_uint_fast8_t;
typedef _Atomic int_fast16_t atomic_int_fast16_t;
typedef _Atomic uint_fast16_t atomic_uint_fast16_t;
typedef _Atomic int_fast32_t atomic_int_fast32_t;
typedef _Atomic uint_fast32_t atomic_uint_fast32_t;
typedef _Atomic int_fast64_t atomic_int_fast64_t;
typedef _Atomic uint_fast64_t atomic_uint_fast64_t;
typedef _Atomic intptr_t atomic_intptr_t;
typedef _Atomic uintptr_t atomic_uintptr_t;
typedef _Atomic size_t atomic_size_t;
typedef _Atomic ptrdiff_t atomic_ptrdiff_t;
typedef _Atomic intmax_t atomic_intmax_t;
typedef _Atomic uintmax_t atomic_uintmax_t;

typedef atomic_int atomic_flag;

#define atomic_flag_test_and_set(obj) \
    ({ \
    __sync_synchronize(); \
    (bool) !!(__sync_lock_test_and_set((obj), 1)); \
    })

#define atomic_flag_test_and_set_explicit(obj,mo) \
    ({ \
    if ((mo) != memory_order_acquire) __sync_synchronize(); \
    (bool) !!(__sync_lock_test_and_set((obj), 1)); \
    })

#define atomic_flag_clear(obj) \
    ({ \
    __sync_synchronize(); \
    __sync_lock_release((obj)); \
    })

#define atomic_flag_clear_explicit(obj,mo) \
    ({ \
    if ((mo) > memory_order_release) __sync_synchronize(); \
    __sync_lock_release((obj)); \
    })

#define atomic_thread_fence(mo) \
    ({ \
    if ((mo) != memory_order_relaxed) __sync_synchronize(); \
    })

#define atomic_signal_fence(mo) \
    ({ \
    if (mo != memory_order_relaxed) __asm__ __volatile__("":::"memory"); \
    })

/* Always using the strongest memory order. */

/* Atomic loads can be implemented in terms of a compare-and-swap. */
#define atomic_load(obj)  __sync_val_compare_and_swap((obj), 0, 0))
#define atomic_load_explicit(obj,mo) \
    ((mo) == memory_order_relaxed ? *(obj) : __sync_val_compare_and_swap((obj), 0, 0))

#define atomic_exchange(obj,desired)  __sync_lock_test_and_set(obj, desired)
#define atomic_exchange_explicit(obj,desired,mo) \
    ({ \
    if ((mo) != memory_order_acquire) __sync_synchronize(); \
    __sync_lock_test_and_set(obj, desired); \
    })

#define atomic_store           (void) atomic_exchange
#define atomic_store_explicit  (void) atomic_exchange_explicit

#define atomic_fetch_add(obj,arg)  __sync_fetch_and_add((obj), (arg))
#define atomic_fetch_add_explicit(obj,arg,mo) \
    ((mo) == memory_order_relaxed ? ((obj) + (arg)) : __sync_fetch_and_add((obj), (arg)))

#define atomic_fetch_sub(obj,arg)  __sync_fetch_and_sub((obj), (arg))
#define atomic_fetch_sub_explicit(obj,arg,mo) \
    ((mo) == memory_order_relaxed ? ((obj) - (arg)) : __sync_fetch_and_sub((obj), (arg)))

#define atomic_fetch_and(obj,mask)  __sync_fetch_and_and((obj), (mask))
#define atomic_fetch_and_explicit(obj,mask,mo) \
    ((mo) == memory_order_relaxed ? ((obj) & (mask)) : __sync_fetch_and_and((obj), (mask)))

#define atomic_fetch_or(obj,mask)  __sync_fetch_and_or((obj), (mask))
#define atomic_fetch_or_explicit(obj,mask,mo) \
    ((mo) == memory_order_relaxed ? ((obj) | (mask)) : __sync_fetch_and_or((obj), (mask)))

#define atomic_fetch_xor(obj,mask)  __sync_fetch_and_xor((obj), (mask))
#define atomic_fetch_xor_explicit(obj,mask,mo) \
    ((mo) == memory_order_relaxed ? ((obj) ^ (mask)) : __sync_fetch_and_xor((obj), (mask)))

#define atomic_compare_exchange_strong(obj,expected,desired) \
    ({ \
    bool retval; \
    const __typeof__(*(obj)) res = __sync_val_compare_and_swap((obj), *(expected), (desired)); \
    if (res == *(expected)) retval = true; \
    else { \
        *(expected) = res; \
        retval = false; \
    } \
    retval; \
    })

#define atomic_compare_exchange_strong_explicit(obj,expected,desired,smo,fmo) \
    ({ \
    bool retval; \
    if ((smo) == memory_order_relaxed && (fmo) == memory_order_relaxed) { \
        if (*(obj) == *(expected)) retval = true; \
        else { \
            *(expected) = *(obj); \
            retval = false; \
        } \
    } \
    else { \
        const __typeof__(*(obj)) res = __sync_val_compare_and_swap((obj), *(expected), (desired)); \
        if (res == *(expected)) retval = true; \
        else { \
            *(expected) = res; \
            retval = false; \
        } \
    } \
    retval; \
    })

#define atomic_compare_exchange_weak           atomic_compare_exchange_strong
#define atomic_compare_exchange_weak_explicit  atomic_compare_exchange_strong_explicit

#define atomic_is_lock_free(obj) \
    ((bool) !!((sizeof(*(obj)) == 1) || (sizeof(*(obj)) == 2) || (sizeof(*(obj)) == 4) || (sizeof(*(obj)) == 8)))

#endif

#else

#define NOCL_FEATURE_NO_STDATOMIC

#endif

#if defined(__cplusplus)

}

#endif

#endif
