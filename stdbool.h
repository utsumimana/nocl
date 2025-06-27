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

#if !defined(_NOCL_STDBOOL_H)
#define _NOCL_STDBOOL_H

#if defined(__cplusplus)

extern "C" {

#endif

#if defined(NOCL_HAS_STDBOOL_H) || \
    /* C99 */ (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
    /* C++11 */ (defined(__cplusplus) && __cplusplus >= 201103L) || \
    /* MSVC 14.0 */ (defined(_MSC_VER) && _MSC_VER >= 1900) || \
    /* GCC 3.0.0 */ (defined(__GNUC__) && __GNUC__ >= 3)

#include <stdbool.h>

#if /* C++98 */ defined(__cplusplus) || \
    /* C23 */ (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)

#define _Bool  bool

#endif

#elif defined(_MSC_VER) || defined(__MINGW32__)

#if !defined(__cplusplus)

#if !defined(WIN32_LEAN_AND_MEAN)

#define WIN32_LEAN_AND_MEAN

#endif

#include <windows.h>

#define false  FALSE
#define true   TRUE

typedef BOOL _Bool;

#define bool  _Bool

#endif

#define __bool_true_false_are_defined  1

#elif /* C89 */ defined(__STDC__)

#if defined(__GNUC__)

#if !defined(__cplusplus)

#define false  0
#define true   (!(false))

#if /* GCC 2.95.0 */ __GNUC__ < 2 || !defined(__GNUC_MINOR__) || __GNUC__ != 2 || __GNUC_MINOR__ < 95

typedef unsigned char _Bool;

#endif

#define bool  _Bool

#endif

#define __bool_true_false_are_defined  1

#else

#define NOCL_FEATURE_NO_STDBOOL

#endif

#else

#define NOCL_FEATURE_NO_STDBOOL

#endif

#if defined(__cplusplus)

}

#endif

#endif
