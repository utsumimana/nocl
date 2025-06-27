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

#if !defined(_NOCL_LASTERROR_H)
#define _NOCL_LASTERROR_H

#if defined(__cplusplus)

extern "C" {

#endif

#include "errno.h"
#include "string.h"
#include "inline.h"
#include "callconv.h"

#if defined(NOCL_FEATURE_NO_ERRNO) || defined(NOCL_FEATURE_NO_STRING)

#define NOCL_FEATURE_NO_LASTERROR

#else

typedef int stderrno_t;

#if /* Win32 */ defined(_WIN32) && \
    /* MSVC 8.0 */ (defined(_MSC_VER) && _MSC_VER >= 1400)

#define putstderrno  _set_errno
#define getstderrno  _get_errno

#else

inline errno_t cdecl putstderrno(int errnum) {
    errno = errnum;
    return 0;
}

inline errno_t cdecl getstderrno(int *errnum) {
    if (!errnum) return (errno = EINVAL);
    *errnum = errno;
    return 0;
}

#endif

#if \
	/* GCC 2.13.0 */ (defined(__GNUC__) && (__GNUC__ >= 3 || (defined(__GNUC_MINOR__) && __GNUC__ == 2 && __GNUC_MINOR__ >= 13))) && \
	/* POSIX.1-2001 */ ((defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L) || \
	/* UNIX03 */ (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 600)) && \
	!defined(_GNU_SOURCE)

#define strstderror(errnum,buf,buflen)  strerror_r((errnum), (buf), (buflen))

#else

inline errno_t cdecl strstderror(int errnum, char *buf, size_t buflen) {
    if (buflen == 0) return 0;
    if (!buf) return (errno = EINVAL);
    char *str = strerror(errnum);
    if (buflen < strlen(str)) return (errno = ERANGE);
    memcpy(buf, str, buflen);
    return 0;
}

#endif

#if defined(_WIN32)

#if !defined(WIN32_LEAN_AND_MEAN)

#define WIN32_LEAN_AND_MEAN

#endif

#include <windows.h>

typedef DWORD syserrno_t;

inline errno_t cdecl putsyserrno(syserrno_t errnum) {
    SetLastError(errnum);
    return 0;
}

inline errno_t cdecl getsyserrno(syserrno_t *errnum) {
    if (!errnum) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return ERROR_INVALID_PARAMETER;
    }
    *errnum = GetLastError();
    return 0;
}

inline errno_t cdecl strsyserror(syserrno_t errnum, char *buf, size_t buflen) {
    if (buflen > 65536) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return ERROR_INVALID_PARAMETER;
    }

    if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        errnum,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buf,
        buflen,
        NULL)) {
        return 0;
    }
    else {
        DWORD dwError = GetLastError();
        return dwError > 0 ? (int) dwError : ERROR_INTERNAL_ERROR;
    }
}

inline errno_t cdecl _wcssyserror(syserrno_t errnum, wchar_t *buf, size_t buflen) {
    if (buflen > 65536) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return ERROR_INVALID_PARAMETER;
    }

    if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        errnum,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buf,
        buflen,
        NULL)) {
        return 0;
    }
    else {
        DWORD dwError = GetLastError();
        return dwError > 0 ? (int) dwError : ERROR_INTERNAL_ERROR;
    }
}

#else

#define putsyserrno  putstderrno
#define getsyserrno  getstderrno
#define strsyserror  strstderror

#endif

#endif

#if defined(__cplusplus)

}

#endif

#endif
