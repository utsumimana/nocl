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

#if !defined(_NOCL_STDCKDINT_H)
#define _NOCL_STDCKDINT_H

#if defined(__cplusplus)

extern "C" {

#endif

#if defined(NOCL_HAS_STDCKDINT_H) || \
    /* C23 */ (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)

#include <stdckdint.h>

#elif /* GCC 5.0.0 */ defined(__GNUC__) && __GNUC__ >= 5

#include "stdbool.h"

#if defined(NOCL_FEATURE_NO_STDBOOL)

#define NOCL_FEATURE_NO_STDCKDINT

#else

#define ckd_add(r,a,b)  ((bool) __builtin_add_overflow(a,b,r))
#define ckd_sub(r,a,b)  ((bool) __builtin_sub_overflow(a,b,r))
#define ckd_mul(r,a,b)  ((bool) __builtin_mul_overflow(a,b,r))

#endif

#else

#define NOCL_FEATURE_NO_STDCKDINT

#endif

#if defined(__cplusplus)

}

#endif

#endif
