/*
 * Copyright (c) 2015 Nick Strupat
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

#if !defined(_NOCL_ALLOCATOR_H)
#define _NOCL_ALLOCATOR_H

#if defined(__cplusplus)

extern "C" {

#endif

#include "stddef.h"
#include "stdlib.h"
#include "inline.h"
#include "callconv.h"

#if defined(NOCL_FEATURE_NO_STDDEF) || defined(NOCL_FEATURE_NO_STDLIB)

#define NOCL_FEATURE_NO_ALLOCATOR

#else

#if defined(_WIN32)

#if defined(_DEBUG)

#define _CRTDBG_MAP_ALLOC

#include <malloc.h>
#include <crtdbg.h>

#endif

#include <malloc.h>
#include <string.h>
#include <intsafe.h>

#define aligned_malloc   _aligned_malloc

__inline void *__cdecl aligned_calloc(size_t num, size_t size, size_t alignment) {
    size_t n;
    if (SizeTMult(num, size, &n)) return NULL;
    void *res = _aligned_malloc(n, alignment);
    if (!res) return NULL;
    memset(res, 0, n);
    return res;
}

#define aligned_realloc  _aligned_realloc
#define aligned_free     _aligned_free

#elif \
    /* POSIX.1-2001 */ ((defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L) || \
	/* UNIX03 */ (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 600))

inline void *aligned_malloc(size_t size, size_t alignment) {
    void *res = malloc(size);
    if (!res) return NULL;
    int retval = posix_memalign(&res, alignment, size);
    if (retval) errno = retval;
    return res;
}

inline void *aligned_calloc(size_t num, size_t size, size_t alignment) {
    void *res = calloc(size);
    if (!res) return NULL;
    int retval = posix_memalign(&res, alignment, size);
    if (retval) errno = retval;
    return res;
}

inline void *aligned_malloc(void *ptr, size_t size, size_t alignment) {
    void *res = realloc(ptr, size);
    if (!res) return NULL;
    int retval = posix_memalign(&res, alignment, size);
    if (retval) errno = retval;
    return res;
}

#define aligned_free  free

#else

#include "stdlib.h"

#if !defined(NOCL_FEATURE_NO_STDLIB)

inline void *aligned_malloc(size_t size, size_t alignment) {
    void *original;
    void **aligned;
    int offset = alignment - 1 + sizeof(void *);
    size_t n = size + offset;
    if (n < size) return NULL;
    if (!((original = malloc(n)))) return NULL;
    aligned = (void **) (((size_t) (original) + offset) & ~(alignment - 1));
    aligned[-1] = original;
    return aligned;
}

inline void *aligned_calloc(size_t num, size_t size, size_t alignment) {
    void *original;
    void **aligned;
    int offset = alignment - 1 + sizeof(void *);
    size_t n = size + offset;
    if (n < size) return NULL;
    if (!((original = calloc(num, n)))) return NULL;
    aligned = (void **) (((size_t) (original) + offset) & ~(alignment - 1));
    aligned[-1] = original;
    return aligned;
}

inline void *aligned_realloc(void *ptr, size_t size, size_t alignment) {
    void *original;
    void **aligned;
    int offset = alignment - 1 + sizeof(void *);
    size_t n = size + offset;
    if (n < size) return NULL;
    if (!((original = realloc(ptr, n)))) return NULL;
    aligned = (void **) (((size_t) (original) + offset) & ~(alignment - 1));
    aligned[-1] = original;
    return aligned;
}

inline void aligned_free(void *ptr) {
    free(((void **) ptr)[-1]);
}

#endif

#endif

typedef void *(*ator_malloc_t)  (size_t);
typedef void *(*ator_calloc_t)  (size_t, size_t);
typedef void *(*ator_realloc_t) (void *, size_t);
typedef void  (*ator_free_t)    (void *);

typedef struct ator_t {
    ator_malloc_t  f_malloc;
    ator_calloc_t  f_calloc;
    ator_realloc_t f_realloc;
    ator_free_t    f_free;
} ator_t;

#define ATOR_DEFAULT  ((void *)  0)
#define ATOR_ALIGNED  ((void *) -1)

inline ator_t cdecl *ator_create(
    ator_malloc_t malloc_fn,
    ator_calloc_t calloc_fn,
    ator_realloc_t realloc_fn,
    ator_free_t free_fn
    ) {

    ator_t *ator = malloc(sizeof(ator_t));
    if (!ator) return NULL;

    ator->f_malloc  = malloc_fn  ? malloc_fn  : malloc;
    ator->f_calloc  = calloc_fn  ? calloc_fn  : calloc;
    ator->f_realloc = realloc_fn ? realloc_fn : realloc;
    ator->f_free    = free_fn    ? free_fn    : free;

    return ator;
}

inline void cdecl ator_destroy(ator_t *ator) {
    if (!ator || ator == ATOR_ALIGNED) return;
    free(ator);
}

inline void *cdecl ator_malloc(ator_t *ator, size_t size) {
    if (!ator) return malloc(size);
    else if (ator == ATOR_ALIGNED) return aligned_malloc(size, sizeof(void *));
    else return ator->f_malloc(size);
}

inline void *cdecl ator_calloc(ator_t *ator, size_t num, size_t size) {
    if (!ator) return calloc(num, size);
    else if (ator == ATOR_ALIGNED) return aligned_calloc(num, size, sizeof(void *));
    else return ator->f_calloc(num, size);
}

inline void *cdecl ator_realloc(ator_t *ator, void *ptr, size_t size) {
    if (!ator) return realloc(ptr, size);
    else if (ator == ATOR_ALIGNED) return aligned_realloc(ptr, size, sizeof(void *));
    else return ator->f_realloc(ptr, size);
}

inline void cdecl ator_free(ator_t *ator, void *ptr) {
    if (!ator) free(ptr);
    else if (ator == ATOR_ALIGNED) return aligned_free(ptr);
    else ator->f_free(ptr);
}

#endif

#if defined(__cplusplus)

}

#endif

#endif
