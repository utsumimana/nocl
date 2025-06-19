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

#if !defined(_NOCL_CTYPE_H)
#define _NOCL_CTYPE_H

#if defined(__cplusplus)

extern "C" {

#endif

#if defined(NOCL_HAS_CTYPE_H) || \
    /* C89 */ defined(__STDC__) || \
    /* C++98 */ defined(__cplusplus) || \
    /* MSC 1.0 */ defined(_MSC_VER) || \
    /* MinGW/MinGW-w64 GCC 3.2.0 */ defined(__MINGW32__)

#include <ctype.h>

#if defined(_MSC_VER) || defined(__MINGW32__)

#if \
    /* MSVC 14.0 */ (defined(_MSC_VER) && _MSC_VER < 1900) || \
    /* MinGW/MinGW-w64 GCC 4.5.0 */ (defined(__MINGW32__) && defined(__GNUC__) && (__GNUC__ < 3 || (defined(__GNUC_MINOR__) && __GNUC__ == 4 && __GNUC_MINOR__ < 5)))

__inline int __cdecl isblank(int c) {
    return c == ' ' || c == '\t';
}

#endif

#elif defined(__GNUC__)

#if /* C99 */ !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L

__inline__ int isblank(int c) {
    return c == ' ' || c == '\t';
}

#endif

#else

#define NOCL_FEATURE_NO_ISBLANK

#endif

#else

#define NOCL_FEATURE_NO_CTYPE
#define NOCL_FEATURE_NO_ISBLANK

#endif

#if defined(__cplusplus)

}

#endif

#endif
