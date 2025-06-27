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

#if !defined(_NOCL_PREDICT_H)
#define _NOCL_PREDICT_H

#if defined(__cplusplus)

extern "C" {

#endif

#if /* C++20 */ defined(__cplusplus) && __cplusplus >= 202002L

#define _Likely(expr) ( \
    ([](bool b) { switch (b) { [[likely]] case true: return true; [[unlikely]] case false: return false; } }) \
    (expr))
#define _Unlikely(expr) ( \
    ([](bool b) { switch (b) { [[unlikely]] case true: return true; [[likely]] case false: return false; } }) \
    (expr))

#elif /* GCC 3.0.0 */ defined(__GNUC__) && __GNUC__ >= 3

#define _Likely(expr)    __builtin_expect(!!(expr), 1)
#define _Unlikely(expr)  __builtin_expect(!!(expr), 0)

#else

#define _Likely(expr)    (!!(expr))
#define _Unlikely(expr)  (!!(expr))

#endif

#define likely    _Likely
#define unlikely  _Unlikely

#if defined(__cplusplus)

}

#endif

#endif
