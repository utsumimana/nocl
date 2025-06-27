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

#if !defined(_NOCL_NODISCARD_H)
#define _NOCL_NODISCARD_H

#if defined(__cplusplus)

extern "C" {

#endif

#if /* C23 */ (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L) || \
    /* C++17 */ defined(__cplusplus) && __cplusplus >= 201703L

#define _Nodiscard  [[nodiscard]]

#elif /* GCC 3.4.0 */ defined(__GNUC__) && (__GNUC__ >= 4 || (defined(__GNUC_MINOR__) && __GNUC__ == 3 && __GNUC_MINOR__ >= 4))

#define _Nodiscard  __attribute__((__warn_unused_result__))

#elif /* SAL */ defined(_Check_return_)

#define _Nodiscard  _Check_return_

#else

#define _Nodiscard

#endif

#define nodiscard  _Nodiscard

#if defined(__cplusplus)

}

#endif

#endif
