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

#if !defined(_NOCL_RESTRICT_H)
#define _NOCL_RESTRICT_H

#if defined(__cplusplus)

extern "C" {

#endif

#if /* C99 */ (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)

#define _Restrict  restrict

#elif defined(_MSC_VER)

#if _MSC_VER >= 1928

#define _Restrict  restrict

#else

#if defined(__cplusplus)

#if /* MSVC 14.0 */ _MSC_VER >= 1900

#define _Restrict  __restrict

#else

#define _Restrict

#endif

#else

#if /* MSVC 6.0 */ _MSC_VER >= 1200

#define _Restrict  __restrict

#else

#define _Restrict

#endif

#endif

#define restrict  _Restrict

#endif

#elif /* GCC 2.95.0 */ defined(__GNUC__) && (__GNUC__ >= 3 || (defined(__GNUC_MINOR__) && __GNUC__ == 2 && __GNUC_MINOR__ >= 95))

#define _Restrict  __restrict__
#define restrict  _Restrict

#else

#define _Restrict
#define restrict  _Restrict

#endif

#if defined(__cplusplus)

}

#endif

#endif
