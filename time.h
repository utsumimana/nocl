/*
 * Copyright (c) 2012-2023 John Tsiombikas
 * Copyright (c) 2023 Oliver Old
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

#if !defined(_NOCL_TIME_H)
#define _NOCL_TIME_H

#if defined(__cplusplus)

extern "C" {

#endif

#if defined(NOCL_HAS_TIME_H) || \
    /* C89 */ defined(__STDC__) || \
    /* C++98 */ defined(__cplusplus) || \
    /* MSC 1.0 */ defined(_MSC_VER) || \
    /* MinGW/MinGW-w64 GCC 3.2.0 */ defined(__MINGW32__)

#include <time.h>

#if !defined(TIME_UTC)

#define TIME_UTC  1

#endif

#if \
	/* C11 */ !(defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) && \
	/* C++17 */ !(defined(__cplusplus) && __cplusplus >= 201703L)

#if /* Win32 */ defined(_WIN32) && \
	/* MSVC 2.0 */ ((defined(_MSC_VER) && _MSC_VER >= 900) || \
	/* MinGW/MinGW-w64 with GCC 3.2.0 */ defined(__MINGW32__))

#if !defined(WIN32_LEAN_AND_MEAN)

#define WIN32_LEAN_AND_MEAN

#endif

#include <windows.h>

struct __nocl_internal_time_timespec32 {
	long tv_sec;
	long tv_nsec;
};

struct __nocl_internal_time_timespec64 {
	__int64 tv_sec;
	long tv_nsec;
};

#if \
	/* MSVC 14.0 */ !(defined(_MSC_VER) && _MSC_VER >= 1900) && \
	/* MinGW/MinGW-w64 UCRT */ !(defined(__MINGW32__) && (defined(_UCRT) || defined(_TIMESPEC_DEFINED)))

#if defined(_USE_32BIT_TIME_T)

struct timespec {
	long tv_sec;
	long tv_nsec;
};

#else

struct timespec {
	__int64 tv_sec;
	long tv_nsec;
};

#endif

#endif

__inline int __cdecl __nocl_internal_time_timespec32_get(struct __nocl_internal_time_timespec32 *ts, int base) {
	if (base != TIME_UTC) return 0;

	FILETIME ft;
	ULARGE_INTEGER li;

	GetSystemTimeAsFileTime(&ft);

	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	/* Also subtract difference between FILETIME and UNIX time epoch. It's 369 years by the way. */
	ts->tv_sec = (long) (li.QuadPart / (unsigned __int64) 10000000 - (unsigned __int64) 11644473600);
	ts->tv_nsec = (long) (li.QuadPart % (unsigned __int64) 10000000) * 100;

	return base;
}

__inline int __cdecl __nocl_internal_time_timespec64_get(struct __nocl_internal_time_timespec64 *ts, int base) {
	if (base != TIME_UTC) return 0;

	FILETIME ft;
	ULARGE_INTEGER li;

	GetSystemTimeAsFileTime(&ft);

	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	/* Also subtract difference between FILETIME and UNIX time epoch. It's 369 years by the way. */
	ts->tv_sec = li.QuadPart / (unsigned __int64) 10000000 - (unsigned __int64) 11644473600;
	ts->tv_nsec = (long) (li.QuadPart % (unsigned __int64) 10000000) * 100;

	return base;
}

#if \
	/* MSVC 14.0 */ !(defined(_MSC_VER) && _MSC_VER >= 1900) && \
	/* MinGW/MinGW-w64 UCRT */ !(defined(__MINGW32__) && (defined(_UCRT) || defined(_TIMESPEC_DEFINED)))

__inline int __cdecl timespec_get(struct timespec *ts, int base) {
	if (sizeof(ts->tv_sec) == 4)
		return __nocl_internal_time_timespec32_get((struct __nocl_internal_time_timespec32 *) ts, base);
	else
		return __nocl_internal_time_timespec64_get((struct __nocl_internal_time_timespec64 *) ts, base);
}

#endif

#elif defined(__GNUC__) && \
	/* POSIX.1-2001 */ ((defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L) || \
	/* UNIX03 */ (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 600))

#include <sys/time.h>

#if defined(__APPLE__)

#include <Availability.h>

#endif

#if !defined(__APPLE__) || !defined(__MAC_10_15)

__inline__ int timespec_get(struct timespec *ts, int base) {
	struct timeval tv;

	if (base != TIME_UTC) return 0;

	if (gettimeofday(&tv, 0) == -1) return 0;

	ts->tv_sec = tv.tv_sec;
	ts->tv_nsec = tv.tv_usec * 1000;
	return base;
}

#endif

#else

#define NOCL_FEATURE_NO_TIMESPEC

#endif

#endif

#if /* C23 */ !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L

#if defined(NOCL_FEATURE_NO_TIMESPEC)

#define NOCL_FEATURE_NO_TIMERES

#elif /* Win32 */ defined(_WIN32) && \
	(defined(_MSC_VER) || defined(__MINGW32__))

#if !defined(WIN32_LEAN_AND_MEAN)

#define WIN32_LEAN_AND_MEAN

#endif

#include <windows.h>

__inline int __cdecl timespec_getres(struct timespec *ts, int base) {
	if (base == TIME_UTC) {
		LARGE_INTEGER frequency;
		if (QueryPerformanceFrequency(&frequency) != 0) {
			if (ts) {
				ts->tv_sec = 0;
				ts->tv_nsec = 1000000000 / frequency.QuadPart;
			}
			return base;
		}
	}
	return 0;
}

#elif defined(__GNUC__) && \
	/* POSIX.1-2001 */ ((defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L) || \
	/* UNIX03 */ (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 600))

#include <unistd.h>

__inline__ int timespec_getres(struct timespec *ts, int base) {
	if (base == TIME_UTC) {
		long clk_tck = sysconf(_SC_CLK_TCK);
		if (clk_tck != -1) {
			if (ts) {
				ts->tv_sec = 0;
				ts->tv_nsec = 1000000000 / clk_tck;
			}
			return base;
		}
	}
	return 0;
}

#else

#define NOCL_FEATURE_NO_TIMERES

#endif

#endif

#else

#define NOCL_FEATURE_NO_TIME
#define NOCL_FEATURE_NO_TIMESPEC
#define NOCL_FEATURE_NO_TIMERES

#endif

#if defined(__cplusplus)

}

#endif

#endif
