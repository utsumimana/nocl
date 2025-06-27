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

#if !defined(_NOCL_INLINE_H)
#define _NOCL_INLINE_H

#if defined(__cplusplus)

extern "C" {

#endif

#if /* C99 */ (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
    /* C++98 */ defined(__cplusplus)

#define _Inline  inline

#else

#if /* MSVC 4.0 */ defined(_MSC_VER) && _MSC_VER >= 1000

#define _Inline  __inline

#if /* MSVC 14.0 */ defined(_MSC_VER) && _MSC_VER < 1900

#define inline  _Inline

#endif

#elif /* GCC 2.7.0 */ defined(__GNUC__) && (__GNUC__ >= 3 || (defined(__GNUC_MINOR__) && __GNUC__ == 2 && __GNUC_MINOR__ >= 7))

#define _Inline  __inline__

#if /* MinGW/MinGW-w64 GCC 3.2.0 */ !defined(__MINGW32__)

#define inline  _Inline

#endif

#elif /* C99 */ !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L

#define _Inline

#define inline  _Inline

#endif

#endif

#if /* MSVC 7.0 */ defined(_MSC_VER) && _MSC_VER >= 1200

#define _Forceinline  __forceinline inline

#elif /* GCC 4.0.0 */ defined(__GNUC__) && __GNUC__ >= 4

#define _Forceinline  __attribute__((__always_inline__)) inline

#else

#define _Forceinline  inline

#endif

#define forceinline  _Forceinline

#if defined(__cplusplus)

}

#endif

#endif
