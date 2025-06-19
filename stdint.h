/*
 * Copyright (c) 2006-2013 Alexander Chemeris
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

#if !defined(_NOCL_STDINT_H)
#define _NOCL_STDINT_H

#if defined(__cplusplus)

extern "C" {

#endif

#if defined(NOCL_HAS_STDINT_H) || \
    /* C99 */ (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
    /* C++11 */ (defined(__cplusplus) && __cplusplus >= 201103L) || \
    /* MSVC 10.0 */ (defined(_MSC_VER) && _MSC_VER >= 1600) || \
    /* GCC 4.5.0 */ defined(__GNUC__) && (__GNUC__ >= 5 || (defined(__GNUC_MINOR__) && __GNUC__ == 4 && __GNUC_MINOR__ >= 5))

#include <stdint.h>

#elif /* MSVC 2.0 */ (defined(_MSC_VER) && _MSC_VER >= 900)

#include "limits.h"
#include "wchar.h"

#if defined(NOCL_FEATURE_NO_LIMITS) || defined(NOCL_FEATURE_NO_WCHAR)

#define NOCL_FEATURE_NO_STDINT

#else

#if _MSC_VER < 1300

typedef signed char       int8_t;
typedef signed short      int16_t;
typedef signed int        int32_t;
typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
typedef unsigned int      uint32_t;

#else

typedef signed __int8     int8_t;
typedef signed __int16    int16_t;
typedef signed __int32    int32_t;
typedef unsigned __int8   uint8_t;
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;

#endif

typedef signed __int64    int64_t;
typedef unsigned __int64  uint64_t;

typedef int8_t    int_least8_t;
typedef int16_t   int_least16_t;
typedef int32_t   int_least32_t;
typedef int64_t   int_least64_t;
typedef uint8_t   uint_least8_t;
typedef uint16_t  uint_least16_t;
typedef uint32_t  uint_least32_t;
typedef uint64_t  uint_least64_t;

typedef int8_t    int_fast8_t;
typedef int16_t   int_fast16_t;
typedef int32_t   int_fast32_t;
typedef int64_t   int_fast64_t;
typedef uint8_t   uint_fast8_t;
typedef uint16_t  uint_fast16_t;
typedef uint32_t  uint_fast32_t;
typedef uint64_t  uint_fast64_t;

#if defined(_WIN64)

typedef signed __int64     intptr_t;
typedef unsigned __int64   uintptr_t;

#else

#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300

typedef __w64 signed int   intptr_t;
typedef __w64 unsigned int uintptr_t;

#else

typedef signed int         intptr_t;
typedef unsigned int       uintptr_t;

#endif

#endif

typedef int64_t   intmax_t;
typedef uint64_t  uintmax_t;

#if !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS)

#define INT8_MIN     (_I8_MIN + 0)
#define INT8_MAX     (_I8_MAX + 0)
#define INT16_MIN    (_I16_MIN + 0)
#define INT16_MAX    (_I16_MAX + 0)
#define INT32_MIN    (_I32_MIN + 0)
#define INT32_MAX    (_I32_MAX + 0)
#define INT64_MIN    (_I64_MIN + 0)
#define INT64_MAX    (_I64_MAX + 0)
#define UINT8_MAX    (_UI8_MAX + 0)
#define UINT16_MAX   (_UI16_MAX + 0)
#define UINT32_MAX   (_UI32_MAX + 0)
#define UINT64_MAX   (_UI64_MAX + 0)

#define INT_LEAST8_MIN    INT8_MIN
#define INT_LEAST8_MAX    INT8_MAX
#define INT_LEAST16_MIN   INT16_MIN
#define INT_LEAST16_MAX   INT16_MAX
#define INT_LEAST32_MIN   INT32_MIN
#define INT_LEAST32_MAX   INT32_MAX
#define INT_LEAST64_MIN   INT64_MIN
#define INT_LEAST64_MAX   INT64_MAX
#define UINT_LEAST8_MAX   UINT8_MAX
#define UINT_LEAST16_MAX  UINT16_MAX
#define UINT_LEAST32_MAX  UINT32_MAX
#define UINT_LEAST64_MAX  UINT64_MAX

#define INT_FAST8_MIN    INT8_MIN
#define INT_FAST8_MAX    INT8_MAX
#define INT_FAST16_MIN   INT16_MIN
#define INT_FAST16_MAX   INT16_MAX
#define INT_FAST32_MIN   INT32_MIN
#define INT_FAST32_MAX   INT32_MAX
#define INT_FAST64_MIN   INT64_MIN
#define INT_FAST64_MAX   INT64_MAX
#define UINT_FAST8_MAX   UINT8_MAX
#define UINT_FAST16_MAX  UINT16_MAX
#define UINT_FAST32_MAX  UINT32_MAX
#define UINT_FAST64_MAX  UINT64_MAX

#if defined(_WIN64)

#define INTPTR_MIN   INT64_MIN
#define INTPTR_MAX   INT64_MAX
#define UINTPTR_MAX  UINT64_MAX

#else

#define INTPTR_MIN   INT32_MIN
#define INTPTR_MAX   INT32_MAX
#define UINTPTR_MAX  UINT32_MAX

#endif

#define INTMAX_MIN   INT64_MIN
#define INTMAX_MAX   INT64_MAX
#define UINTMAX_MAX  UINT64_MAX

#if defined(_WIN64)

#define PTRDIFF_MIN  _I64_MIN
#define PTRDIFF_MAX  _I64_MAX

#else

#define PTRDIFF_MIN  _I32_MIN
#define PTRDIFF_MAX  _I32_MAX

#endif

#define SIG_ATOMIC_MIN  INT_MIN
#define SIG_ATOMIC_MAX  INT_MAX

#if !defined(SIZE_MAX)

#if defined(_WIN64)

#define SIZE_MAX  _UI64_MAX

#else

#define SIZE_MAX  _UI32_MAX

#endif

#endif

#if !defined(__STDC_SECURE_LIB__) && !defined(__STDC_LIB_EXT1__)

#define RSIZE_MAX  SIZE_MAX

#endif

#if !defined(WCHAR_MIN)

#define WCHAR_MIN  0

#endif

#if !defined(WCHAR_MAX)

#define WCHAR_MAX  _UI16_MAX

#endif

#define WINT_MIN  0
#define WINT_MAX  _UI16_MAX

#endif

#if !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS)

#define INT8_C(i)     i ## i8
#define INT16_C(i)    i ## i16
#define INT32_C(i)    i ## i32
#define INT64_C(i)    i ## i64

#define UINT8_C(i)    i ## ui8
#define UINT16_C(i)   i ## ui16
#define UINT32_C(i)   i ## ui32
#define UINT64_C(i)   i ## ui64

#define INTMAX_C(i)   INT64_C(i)
#define UINTMAX_C(i)  UINT64_C(i)

#endif

#endif

#else

#define NOCL_FEATURE_NO_STDINT

#endif

#if defined(__cplusplus)

}

#endif

#endif
