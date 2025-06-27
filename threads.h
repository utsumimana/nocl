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

#if !defined(_NOCL_THREADS_H)
#define _NOCL_THREADS_H

#if defined(__cplusplus)

extern "C" {

#endif

#if defined(NOCL_HAS_THREADS_H) || \
    /* C11 */ (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__) && !defined(__MINGW32__))

#include <threads.h>

#elif /* Win32 */ defined(_WIN32) && \
	/* MSVC 2.0 */ ((defined(_MSC_VER) && _MSC_VER >= 900) || \
	/* MinGW/MinGW-w64 GCC 3.2.0 */ defined(__MINGW32__)

#include "time.h"
#include "stdlib.h"
#include "noinline.h"

#if defined(NOCL_FEATURE_NO_TIME) || defined(NOCL_FEATURE_NO_STDLIB)

#define NOCL_FEATURE_NO_THREADS

#else

#if !defined(WIN32_LEAN_AND_MEAN)

#define WIN32_LEAN_AND_MEAN

#endif

#include <windows.h>

typedef int (*thrd_start_t) (void *);
typedef void (*tss_dtor_t)  (void *);

enum {
	mtx_plain      = 0,
	mtx_recursive  = 1,
	mtx_timed      = 2
};

enum {
	thrd_success,
	thrd_timedout,
	thrd_busy,
	thrd_error,
	thrd_nomem
};

#if !defined(thread_local)

#define thread_local  __declspec(thread)

#endif

#define ONCE_FLAG_INIT       {0}
#define TSS_DTOR_ITERATIONS  4

typedef HANDLE thrd_t;

typedef CRITICAL_SECTION mtx_t;

typedef struct cnd_t {
	HANDLE mutex;
	HANDLE signal_sema;
	HANDLE broadcast_event;
	size_t wait_count;
} cnd_t;

typedef DWORD tss_t;

typedef volatile unsigned long *once_flag;

__inline int __cdecl thrd_create(thrd_t *thr, thrd_start_t func, void *arg) {
	HANDLE h = CreateThread(NULL, 0, (PTHREAD_START_ROUTINE) func, arg, 0, NULL);
	if (!h) return GetLastError() == ERROR_NOT_ENOUGH_MEMORY ? thrd_nomem : thrd_error;
	*thr = h;
	return thrd_success;
}

#if /* MSVC 7.1 */ defined(_MSC_VER) && _MSC_VER >= 1310

__inline __declspec(noreturn) void __cdecl thrd_exit(int res) {
	ExitThread(res);
}

#else

__inline void __cdecl thrd_exit(int res) {
	ExitThread(res);
}

#endif

__inline int __cdecl thrd_join(thrd_t thr, int *res) {
	int retval = thrd_error;

	if (thr) {
		if (WaitForSingleObject(thr, INFINITE) == WAIT_OBJECT_0 &&
			(!res || GetExitCodeThread(thr, (LPDWORD) res))) {
			retval = thrd_success;
		}
		CloseHandle(thr);
	}

	return retval;
}

__inline int __cdecl thrd_detach(thrd_t thr) {
	return thr && CloseHandle(thr) ? thrd_success : thrd_error;
}

__inline thrd_t __cdecl thrd_current(void) {
	return GetCurrentThread();
}

__inline int __cdecl thrd_equal(thrd_t a, thrd_t b) {
	return a == b;
}

#define __nocl_internal_threads_is_timespec32_valid(ts) \
  ((((struct __nocl_internal_time_timespec32 *) (ts))->tv_sec >= 0) && \
    (((struct __nocl_internal_time_timespec32 *) (ts))->tv_nsec >= 0) && \
      (((struct __nocl_internal_time_timespec32 *) (ts))->tv_nsec <= 999999999))

#define __nocl_internal_threads_is_timespec64_valid(ts) \
  ((((struct __nocl_internal_time_timespec64 *) (ts))->tv_sec >= 0) && \
    (((struct __nocl_internal_time_timespec64 *) (ts))->tv_nsec >= 0) && \
      (((struct __nocl_internal_time_timespec64 *) (ts))->tv_nsec <= 999999999))

/* Precondition: 'ts' validated. */
__inline __int64 __cdecl __nocl_internal_time_timespec32_to_file_time(const struct __nocl_internal_time_timespec32 *ts) {

	unsigned __int64 sec = (unsigned __int64) ts->tv_sec * (unsigned __int64) 10000000;

	/* Add another 100 ns if division yields remainder. */
	unsigned __int64 nsec = (unsigned long) ts->tv_nsec / 100ul + !!((unsigned long) ts->tv_nsec % 100ul);

	return sec + nsec;
}

/* Precondition: 'ts' validated. */
__inline __int64 __cdecl __nocl_internal_time_timespec64_to_file_time(const struct __nocl_internal_time_timespec64 *ts, size_t *periods) {

	*periods = (unsigned long) ((unsigned __int64) ts->tv_sec / (unsigned __int64) 922337203685);
	unsigned __int64 sec = (unsigned __int64) ts->tv_sec % (unsigned __int64) 922337203685 * (unsigned __int64) 10000000;

	/* Add another 100 ns if division yields remainder. */
	unsigned __int64 nsec = (unsigned long) ts->tv_nsec / 100ul + !!((unsigned long) ts->tv_nsec % 100ul);

	/* 64-bit time_t may cause overflow. */
	if (nsec > (unsigned __int64) - 1 - sec) {
		++ *periods;
		nsec -= (unsigned __int64) - 1 - sec;
		sec = 0;
	}

	unsigned __int64 retval = sec + nsec;

	if (*periods && !retval) {
		-- *periods;
		return (__int64) 9223372036850000000;
	}

	return retval;
}

__inline int __cdecl __nocl_internal_threads_thrd_sleep(__int64 file_time_in) {

	if (file_time_in < 0) return -ERROR_INTERNAL_ERROR;

	DWORD error;

	HANDLE timer = CreateWaitableTimerA(NULL, 1, NULL);
	if (!timer) {
		error = GetLastError();
		return error > 1 ? - (int) error : -ERROR_INTERNAL_ERROR;
	}

	LARGE_INTEGER due_time;
	due_time.QuadPart = - file_time_in;
	if (!SetWaitableTimer(timer, &due_time, 0, NULL, NULL, 0)) {
		error = GetLastError();
		CloseHandle(timer);
		return error > 1 ? - (int) error : -ERROR_INTERNAL_ERROR;
	}

	if (WaitForSingleObject(timer, INFINITE) != WAIT_OBJECT_0) {
		error = GetLastError();
		CloseHandle(timer);
		return error > 1 ? - (int) error : -ERROR_INTERNAL_ERROR;
	}

	CloseHandle(timer);
	return 0;
}

__inline int __cdecl __nocl_internal_threads_thrd_sleep32(const struct __nocl_internal_time_timespec32 *ts_in, struct __nocl_internal_time_timespec32 *rem_out) {
	(void) rem_out;

	if (!__nocl_internal_threads_is_timespec32_valid(ts_in)) return - ERROR_INVALID_PARAMETER;

	__int64 file_time = __nocl_internal_time_timespec32_to_file_time(ts_in);
	if (file_time < 0) return - ERROR_INVALID_PARAMETER;

	int retval = __nocl_internal_threads_thrd_sleep(file_time);

	return retval;
}

__inline int __cdecl __nocl_internal_threads_thrd_sleep64(const struct __nocl_internal_time_timespec64 *ts_in, struct __nocl_internal_time_timespec64 *rem_out) {
	(void) rem_out;

	if (!__nocl_internal_threads_is_timespec64_valid(ts_in)) return - ERROR_INVALID_PARAMETER;

	size_t periods;
	__int64 file_time = __nocl_internal_time_timespec64_to_file_time(ts_in, &periods);
	if (file_time < 0) return - ERROR_INVALID_PARAMETER;

restart_sleep:

	int retval = __nocl_internal_threads_thrd_sleep(file_time);

	if (!retval && periods) {
		-- periods;
		file_time = (__int64) 9223372036850000000;
		goto restart_sleep;
	}

	return retval;
}

__inline int __cdecl thrd_sleep(const struct timespec *ts_in, struct timespec *rem_out) {
	if (sizeof(ts_in->tv_sec) == 4)
		return __nocl_internal_threads_thrd_sleep32((const struct __nocl_internal_time_timespec32 *) ts_in, (struct __nocl_internal_time_timespec32 *) rem_out);
	else
		return __nocl_internal_threads_thrd_sleep64((const struct __nocl_internal_time_timespec64 *) ts_in, (struct __nocl_internal_time_timespec64 *) rem_out);
}

__inline void __cdecl thrd_yield(void) {
	SwitchToThread();
}

__inline int __cdecl mtx_init(mtx_t *mtx, int type) {
	(void) type;

#if defined(_MSC_VER)

#pragma warning(push)
#pragma warning(suppress: 28125) /* Warning C28125: The function 'InitializeCriticalSection' must be called from within a try/except block. */

#endif
	InitializeCriticalSection(mtx);

#if defined(_MSC_VER)

#pragma warning(pop)

#endif

	return thrd_success;
}

__inline void __cdecl mtx_destroy(mtx_t *mtx) {
	DeleteCriticalSection(mtx);
}

__inline int __cdecl mtx_lock(mtx_t *mtx) {
	EnterCriticalSection(mtx);
	return thrd_success;
}

__inline int __cdecl mtx_trylock(mtx_t *mtx) {
	return TryEnterCriticalSection(mtx) ? thrd_success : thrd_busy;
}

__inline int __cdecl __nocl_internal_threads_mtx_timedlock32(mtx_t *mtx, const struct __nocl_internal_time_timespec32 *ts) {

	if (!__nocl_internal_threads_is_timespec32_valid(ts)) return thrd_error;

	struct __nocl_internal_time_timespec32 ts_current;

	volatile int success = TryEnterCriticalSection(mtx);
	while (!success) {
		if (!__nocl_internal_time_timespec32_get(&ts_current, TIME_UTC)) return thrd_error;

		if (ts_current.tv_sec > ts->tv_sec ||
			(ts_current.tv_sec == ts->tv_sec && ts_current.tv_nsec >= ts->tv_nsec)) {
			return thrd_timedout;
		}

		Sleep(0);

		success = TryEnterCriticalSection(mtx);
	}

	return thrd_success;
}

__inline int __cdecl __nocl_internal_threads_mtx_timedlock64(mtx_t *mtx, const struct __nocl_internal_time_timespec64 *ts) {

	if (!__nocl_internal_threads_is_timespec64_valid(ts)) return thrd_error;

	struct __nocl_internal_time_timespec64 ts_current;

	int success = TryEnterCriticalSection(mtx);
	while (!success) {
		if (!__nocl_internal_time_timespec64_get(&ts_current, TIME_UTC)) return thrd_error;

		if (ts_current.tv_sec > ts->tv_sec ||
			(ts_current.tv_sec == ts->tv_sec && ts_current.tv_nsec >= ts->tv_nsec)) {
			return thrd_timedout;
		}

		Sleep(0);

		success = TryEnterCriticalSection(mtx);
	}

	return thrd_success;
}

__inline int __cdecl mtx_timedlock(mtx_t *mtx, const struct timespec *ts) {
	if (sizeof(ts->tv_sec) == 4)
		return __nocl_internal_threads_mtx_timedlock32(mtx, (const struct __nocl_internal_time_timespec32 *) ts);
	else
		return __nocl_internal_threads_mtx_timedlock64(mtx, (const struct __nocl_internal_time_timespec64 *) ts);
}

__inline int __cdecl mtx_unlock(mtx_t *mtx) {
	LeaveCriticalSection(mtx);
	return thrd_success;
}

__inline int __cdecl cnd_init(cnd_t *cond) {
	cond->mutex = CreateMutexA(NULL, 0, NULL);
	if (cond->mutex) {
		cond->signal_sema = CreateSemaphoreA(NULL, 0, 0x7fffffff, NULL);
		if (cond->signal_sema) {
			cond->broadcast_event = CreateEventA(NULL, 1, 0, NULL);
			if (cond->broadcast_event) {
				cond->wait_count = 0;
				return thrd_success;
			}
			CloseHandle(cond->signal_sema);
		}
		CloseHandle(cond->mutex);
	}
	return thrd_error;
}

__inline void __cdecl cnd_destroy(cnd_t *cond) {
	if (cond->wait_count) abort();
	CloseHandle(cond->mutex);
	CloseHandle(cond->signal_sema);
	CloseHandle(cond->broadcast_event);
	free(cond);
}

__inline int __cdecl cnd_signal(cnd_t *cond) {
	int success = 0;

	DWORD wait_status = WaitForSingleObject(cond->mutex, INFINITE);
	if (wait_status == WAIT_OBJECT_0) success = 1;
	else if (wait_status == WAIT_ABANDONED) abort();

	if (success) {
		if (cond->wait_count)
			success = ReleaseSemaphore(cond->signal_sema, 1, NULL) ||
				GetLastError() == ERROR_TOO_MANY_POSTS;
		if (!ReleaseMutex(cond->mutex))
			success = 0;
	}

	return success ? thrd_success : thrd_error;
}

__inline int __cdecl cnd_broadcast(cnd_t *cond) {
	int success = 0;

	DWORD wait_status = WaitForSingleObject(cond->mutex, INFINITE);
	if (wait_status == WAIT_OBJECT_0) success = 1;
	else if (wait_status == WAIT_ABANDONED) abort();

	if (success) {
		if (cond->wait_count) success = SetEvent(cond->broadcast_event);
		if (!ReleaseMutex(cond->mutex)) success = 0;
	}

	return success ? thrd_success : thrd_error;
}

__inline int _cdecl __nocl_internal_threads_cnd_wait(cnd_t *cond, mtx_t *mtx, DWORD wait_time, int clamped) {

	DWORD wait_status = WaitForSingleObject(cond->mutex, INFINITE);
	if (wait_status == WAIT_ABANDONED) abort();
	else if (wait_status != WAIT_OBJECT_0) return thrd_error;
	LeaveCriticalSection(mtx);
	++ cond->wait_count;
	if (!ReleaseMutex(cond->mutex)) abort();

	wait_status = WaitForMultipleObjects(2, &cond->signal_sema /* and cond->broadcast_event */, 0, wait_time);

	if (WaitForSingleObject(cond->mutex, INFINITE) != WAIT_OBJECT_0) abort();
	-- cond->wait_count;
	if (!cond->wait_count) {
		DWORD wait_status;
		do {
			wait_status = WaitForSingleObject(cond->signal_sema, 0);
		}
		while (wait_status == WAIT_OBJECT_0);
		if (wait_status != WAIT_TIMEOUT) abort();

		if (!ResetEvent(cond->broadcast_event)) abort();
	}
	if (!ReleaseMutex(cond->mutex)) abort();

	int retval = thrd_success;
	if (wait_status == WAIT_TIMEOUT) {
		if (!clamped) retval = thrd_timedout;
	}
	else if (wait_status != WAIT_OBJECT_0 && wait_status != WAIT_OBJECT_0 + 1) {
		retval = thrd_error;
	}

	EnterCriticalSection(mtx);

	return retval;
}

__inline int __cdecl cnd_wait(cnd_t *cond, mtx_t *mtx) {
	return __nocl_internal_threads_cnd_wait(cond, mtx, INFINITE, 0);
}

/* Precondition: 'ts' validated. */
__inline int __cdecl __nocl_internal_time_timespec32_to_milliseconds(const struct __nocl_internal_time_timespec32 *ts, unsigned long *ms) {
	/* Overflow. */
	if ((unsigned long) ts->tv_sec > (INFINITE - 1ul) / 1000ul) return 0;

	unsigned long sec = (unsigned long) ts->tv_sec * 1000ul;
	/* Add another millisecond if division yields remainder. */
	unsigned long nsec = (unsigned long) ts->tv_nsec / 1000000ul + !!((unsigned long) ts->tv_nsec % 1000000ul);

	/* Overflow. */
	if (nsec > INFINITE - 1ul - sec) return 0;

	*ms = sec + nsec;
	return 1;
}

/* Precondition: 'ts' validated. */
__inline int __cdecl __nocl_internal_time_timespec64_to_milliseconds(const struct __nocl_internal_time_timespec64 *ts, unsigned long *ms) {
	/* Overflow. */
	if ((unsigned __int64) ts->tv_sec > (INFINITE - 1ul) / 1000ul) return 0;

	unsigned long sec = (unsigned long) ts->tv_sec * 1000ul;
	/* Add another millisecond if division yields remainder. */
	unsigned long nsec = (unsigned long) ts->tv_nsec / 1000000ul + !!((unsigned long) ts->tv_nsec % 1000000ul);

	/* Overflow. */
	if (nsec > INFINITE - 1ul - sec) return 0;

	*ms = sec + nsec;
	return 1;
}

/* Precondition: 'current_time' and 'end_time' validated. */
__inline DWORD __cdecl __nocl_internal_threads_timepoint_to_millisecond_timespan32(const struct __nocl_internal_time_timespec32 *current_time, const struct __nocl_internal_time_timespec32 *end_time, int *clamped) {
	DWORD wait_time;

	*clamped = 0;
	if (current_time->tv_sec > end_time->tv_sec ||
		(current_time->tv_sec == end_time->tv_sec && current_time->tv_nsec >= end_time->tv_nsec))
		wait_time = 0;
	else {
		struct __nocl_internal_time_timespec32 ts;

		ts.tv_sec = end_time->tv_sec - current_time->tv_sec;
		ts.tv_nsec = end_time->tv_nsec - current_time->tv_nsec;
		if (ts.tv_nsec < 0) {
			-- ts.tv_sec;
			ts.tv_nsec += 1000000000;
		}

		if (!__nocl_internal_time_timespec32_to_milliseconds(&ts, &wait_time)) {
			/* Clamp wait_time. Pretend we've had a spurious wakeup if expired. */
			wait_time = INFINITE - 1;
			*clamped = 1;
		}
	}

	return wait_time;
}

/* Precondition: 'current_time' and 'end_time' validated. */
__inline DWORD __cdecl __nocl_internal_threads_timepoint_to_millisecond_timespan64(const struct __nocl_internal_time_timespec64 *current_time, const struct __nocl_internal_time_timespec64 *end_time, int *clamped) {
	DWORD wait_time;

	*clamped = 0;
	if (current_time->tv_sec > end_time->tv_sec
		|| (current_time->tv_sec == end_time->tv_sec && current_time->tv_nsec >= end_time->tv_nsec))
		wait_time = 0;
	else {
		struct __nocl_internal_time_timespec64 ts;

		ts.tv_sec = end_time->tv_sec - current_time->tv_sec;
		ts.tv_nsec = end_time->tv_nsec - current_time->tv_nsec;
		if (ts.tv_nsec < 0) {
			-- ts.tv_sec;
			ts.tv_nsec += 1000000000;
		}

		if (!__nocl_internal_time_timespec64_to_milliseconds(&ts, &wait_time)) {
			/* Clamp wait_time. Pretend we've had a spurious wakeup if expired. */
			wait_time = INFINITE - 1;
			*clamped = 1;
		}
	}

	return wait_time;
}

__inline int __cdecl __nocl_internal_threads_cnd_timedwait32(cnd_t *cond, mtx_t *mtx, const struct __nocl_internal_time_timespec32 *ts) {
	if (!__nocl_internal_threads_is_timespec32_valid(ts)) return thrd_error;

	struct __nocl_internal_time_timespec32 current_time;

	if (!__nocl_internal_time_timespec32_get(&current_time, TIME_UTC)) return thrd_error;

	int clamped;
	DWORD wait_time = __nocl_internal_threads_timepoint_to_millisecond_timespan32(&current_time, ts, &clamped);
	return __nocl_internal_threads_cnd_wait(cond, mtx, wait_time, clamped);
}

__inline int __cdecl __nocl_internal_threads_cnd_timedwait64(cnd_t *cond, mtx_t *mtx, const struct __nocl_internal_time_timespec64 *ts) {
	if (!__nocl_internal_threads_is_timespec64_valid(ts)) return thrd_error;

	struct __nocl_internal_time_timespec64 current_time;

	if (!__nocl_internal_time_timespec64_get(&current_time, TIME_UTC)) return thrd_error;

	int clamped;
	DWORD wait_time = __nocl_internal_threads_timepoint_to_millisecond_timespan64(&current_time, ts, &clamped);
	return __nocl_internal_threads_cnd_wait(cond, mtx, wait_time, clamped);
}

__inline int __cdecl cnd_timedwait(cnd_t *cond, mtx_t *mtx, const struct timespec *ts) {
	if (sizeof(ts->tv_sec) == 4)
		return __nocl_internal_threads_cnd_timedwait32(cond, mtx, (const struct __nocl_internal_time_timespec32 *) ts);
	else
		return __nocl_internal_threads_cnd_timedwait64(cond, mtx, (const struct __nocl_internal_time_timespec64 *) ts);
}

#define __NOCL_INTERNAL_THREADS_TSS_MAX  1024

struct __nocl_internal_threads_tss_dtor_entry {
	tss_t key;
	tss_dtor_t dtor;
};

#if \
	/* MSVC 7.0 */ (defined(_MSC_VER) && _MSC_VER >= 1300) || \
	/* MinGW/MinGW-w64 GCC 3.2.0 */ defined(__MINGW32__)

CRITICAL_SECTION __nocl_internal_threads_tss_dtor_list_critical_section;
struct __nocl_internal_threads_tss_dtor_entry *__nocl_internal_threads_tss_dtor_list;
size_t __nocl_internal_threads_tss_dtor_list_index;

__inline void __cdecl __nocl_internal_threads_ensure_tss_dtor_list_critical_section_initialized(void) {
	static volatile unsigned long initialized = 0;

	if (InterlockedCompareExchange(&initialized, 1, 0) == 0) {
		InitializeCriticalSection(&__nocl_internal_threads_tss_dtor_list_critical_section);
		InterlockedExchange(&initialized, 2);
	}
	else {

#if defined(_MSC_VER)

#pragma warning(push)
#pragma warning(suppress: 28112) /* Warning C28112: A variable (initialized) which is accessed via an Interlocked function must always be accessed via an Interlocked function. */

#endif

		while (initialized == 1) Sleep(0);

#if defined(_MSC_VER)

#pragma warning(pop)

#endif

	}
}

noinline VOID NTAPI __nocl_internal_threads_run_tss_dtors(PVOID pvDllHandle, DWORD dwReason, PVOID pvReserved) {

	(void) pvDllHandle;
	(void) pvReserved;

	if (dwReason == DLL_THREAD_DETACH) {
		__nocl_internal_threads_ensure_tss_dtor_list_critical_section_initialized();

		EnterCriticalSection(&__nocl_internal_threads_tss_dtor_list_critical_section);

		if (!__nocl_internal_threads_tss_dtor_list) return;
		size_t i;
		int run = 1;
		void *val;
		for (i = 0; (i < TSS_DTOR_ITERATIONS) && run; i ++) {
			run = 0;
			for (i = 0; i < __nocl_internal_threads_tss_dtor_list_index; i ++) {
				struct __nocl_internal_threads_tss_dtor_entry *entry =
					&__nocl_internal_threads_tss_dtor_list[i];
				val = TlsGetValue(entry->key);
				if (val) {
					TlsSetValue(entry->key, NULL);
					entry->dtor(val);
					run = 1;
				}
			}
		}

		LeaveCriticalSection(&__nocl_internal_threads_tss_dtor_list_critical_section);
	}

}

#if defined(_MSC_VER)

#pragma comment(linker, "/INCLUDE:_tls_used")
#pragma comment(linker, "/INCLUDE:__nocl_internal_threads_tls_callback")

#if defined(_WIN64)

#pragma const_seg(".CRT$XLF")

const

#else

#pragma data_seg(".CRT$XLF")

#endif

PIMAGE_TLS_CALLBACK __nocl_internal_threads_tls_callback = __nocl_internal_threads_run_tss_dtors;

#if defined(_WIN64)

#pragma const_seg()

#else

#pragma data_seg()

#endif

#elif defined(__MINGW32__)

__attribute((__section__(".CRT$XLF")))

PIMAGE_TLS_CALLBACK __nocl_internal_threads_tls_callback = __nocl_internal_threads_run_tss_dtors;

#else

#define NOCL_FEATURE_NO_TSS_DTOR

#endif

#else

#define NOCL_FEATURE_NO_TSS_DTOR

#endif

__inline int __cdecl tss_create(tss_t *key, tss_dtor_t dtor) {
	*key = TlsAlloc();
	if (*key == TLS_OUT_OF_INDEXES) return thrd_error;

#if defined(NOCL_FEATURE_NO_TSS_DTOR)

	(void) dtor;

#else

	if (dtor) {
		if (__nocl_internal_threads_tss_dtor_list_index >= __NOCL_INTERNAL_THREADS_TSS_MAX)
			return thrd_error;
		__nocl_internal_threads_ensure_tss_dtor_list_critical_section_initialized();

		EnterCriticalSection(&__nocl_internal_threads_tss_dtor_list_critical_section);

		if (!__nocl_internal_threads_tss_dtor_list)
			__nocl_internal_threads_tss_dtor_list = (struct __nocl_internal_threads_tss_dtor_entry *)
		        malloc(sizeof(struct __nocl_internal_threads_tss_dtor_entry) * __NOCL_INTERNAL_THREADS_TSS_MAX);
		if (!__nocl_internal_threads_tss_dtor_list) {
			TlsFree(*key);
			return thrd_nomem;
		}

		struct __nocl_internal_threads_tss_dtor_entry *entry =
			&__nocl_internal_threads_tss_dtor_list[__nocl_internal_threads_tss_dtor_list_index ++];
		entry->key = *key;
		entry->dtor = dtor;

		LeaveCriticalSection(&__nocl_internal_threads_tss_dtor_list_critical_section);
	}

#endif

	return thrd_success;
}

__inline void __cdecl tss_delete(tss_t key) {

#if !defined(NOCL_FEATURE_NO_TSS_DTOR)

	__nocl_internal_threads_ensure_tss_dtor_list_critical_section_initialized();

	EnterCriticalSection(&__nocl_internal_threads_tss_dtor_list_critical_section);

	if (!__nocl_internal_threads_tss_dtor_list) goto tls_free;
	size_t i;
	for (i = 0; i < __nocl_internal_threads_tss_dtor_list_index; i ++) {
		struct __nocl_internal_threads_tss_dtor_entry *entry =
			&__nocl_internal_threads_tss_dtor_list[i];
		if (entry->key == key) {
			memmove(__nocl_internal_threads_tss_dtor_list + i,
				__nocl_internal_threads_tss_dtor_list + i + 1,
				sizeof(struct __nocl_internal_threads_tss_dtor_entry) *
				__nocl_internal_threads_tss_dtor_list_index - i - 1);
			__nocl_internal_threads_tss_dtor_list_index --;
			break;
		}
	}

	LeaveCriticalSection(&__nocl_internal_threads_tss_dtor_list_critical_section);

tls_free:

#endif

	TlsFree(key);
}

__inline int __cdecl tss_set(tss_t key, void *val) {
	return TlsSetValue(key, val) ? thrd_success : thrd_error;
}

__inline void *__cdecl tss_get(tss_t key) {
	return TlsGetValue(key);
}

__inline void __cdecl call_once(once_flag *flag, void (*func) (void)) {
	if (InterlockedCompareExchange((volatile unsigned long *) flag, 1, 0) == 0) {
		func();
		InterlockedExchange((volatile unsigned long *) flag, 2);
	}
	else {
		while (*(volatile long *) flag == 1) Sleep(0);
	}
}

#if defined(_MSC_VER)

#pragma warning(pop)

#endif

#endif

#elif \
	/* GCC 3.3.0 */ (defined(__GNUC__) && (__GNUC__ >= 4 || (defined(__GNUC_MINOR__) && __GNUC__ == 3 && __GNUC_MINOR__ >= 3))) \
	/* POSIX.1-2001 */ ((defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L) || \
	/* UNIX03 */ (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 600))

#include "time.h"
#include "errno.h"

#if defined(NOCL_FEATURE_NO_TIME) || defined(NOCL_FEATURE_NO_ERRNO)

#define NOCL_FEATURE_NO_THREADS

#else

#include <pthread.h>
#include <sched.h>

typedef int (*thrd_start_t) (void *);
typedef void (*tss_dtor_t) (void *);

enum {
	mtx_plain      = 0,
	mtx_recursive  = 1,
	mtx_timed      = 2
};

enum {
	thrd_success,
	thrd_timedout,
	thrd_busy,
	thrd_error,
	thrd_nomem
};

#if !defined(thread_local)

#define thread_local  __thread

#endif

#define ONCE_FLAG_INIT	     PTHREAD_ONCE_INIT
#define TSS_DTOR_ITERATIONS  PTHREAD_DESTRUCTOR_ITERATIONS

typedef pthread_t thrd_t;
typedef pthread_mutex_t mtx_t;
typedef pthread_cond_t cnd_t;
typedef pthread_key_t tss_t;
typedef pthread_once_t once_flag;

__inline__ int thrd_create(thrd_t *thr, thrd_start_t func, void *arg) {
	int retval = pthread_create(thr, 0, (void *(*) (void *)) func, arg);
	if (retval == 0) return thrd_success;
	return retval == ENOMEM ? thrd_nomem : thrd_error;
}

__inline__ void thrd_exit(int res) {
	pthread_exit((void *) (intptr_t) res);
}

__inline__ int thrd_join(thrd_t thr, int *res) {
	void *retval;

	if (pthread_join(thr, &retval) != 0) return thrd_error;
	if (res) *res = (int) (intptr_t) retval;
	return thrd_success;
}

__inline__ int thrd_detach(thrd_t thr) {
	return pthread_detach(thr) == 0 ? thrd_success : thrd_error;
}

__inline__ thrd_t thrd_current(void) {
	return pthread_self();
}

__inline__ int thrd_equal(thrd_t a, thrd_t b) {
	return pthread_equal(a, b);
}

__inline__ int thrd_sleep(const struct timespec *ts_in, struct timespec *rem_out) {
	if (nanosleep(ts_in, rem_out) < 0) {
		if (errno == EINTR) return -1;
		return -2;
	}
	return 0;
}

__inline__ void thrd_yield(void) {
	sched_yield();
}

__inline__ int mtx_init(mtx_t *mtx, int type) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);

	if (type & mtx_timed) {

#if defined(PTHREAD_MUTEX_TIMED_NP)

		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_TIMED_NP);

#else

		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);

#endif

	}

	if (type & mtx_recursive) {
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	}

	int retval = pthread_mutex_init(mtx, &attr) == 0 ? thrd_success : thrd_error;
	pthread_mutexattr_destroy(&attr);
	return retval;
}

__inline__ void mtx_destroy(mtx_t *mtx) {
	pthread_mutex_destroy(mtx);
}

__inline__ int mtx_lock(mtx_t *mtx) {
	int retval = pthread_mutex_lock(mtx);
	return retval == 0 ? thrd_success : thrd_error;
}

__inline__ int mtx_trylock(mtx_t *mtx) {
	int retval = pthread_mutex_trylock(mtx);
	if (retval == EBUSY) return thrd_busy;
	return retval == 0 ? thrd_success : thrd_error;
}

__inline__ int mtx_timedlock(mtx_t *mtx, const struct timespec *ts) {
	int retval = 0;
	
#if defined(__APPLE__)
	/* Darwin doesn't implement timed mutexes currently */
	
	/* fake a timedlock by polling trylock in a loop and waiting for a bit */
	struct timeval now;
	struct timespec sleeptime;

	sleeptime.tv_sec = 0;
	sleeptime.tv_nsec = 5000000;

	while ((retval = pthread_mutex_trylock(mtx)) == EBUSY) {
		gettimeofday(&now, NULL);

		if (now.tv_sec > ts->tv_sec || (now.tv_sec == ts->tv_sec &&
			(now.tv_usec * 1000) >= ts->tv_nsec)) {
			return thrd_timedout;
		}

		nanosleep(&sleeptime, NULL);
	}
	
#else
	
	if ((retval = pthread_mutex_timedlock(mtx, ts)) == ETIMEDOUT) {
		return thrd_timedout;
	}
	
#endif
	
	return retval == 0 ? thrd_success : thrd_error;
}

__inline__ int mtx_unlock(mtx_t *mtx) {
	return pthread_mutex_unlock(mtx) == 0 ? thrd_success : thrd_error;
}

__inline__ int cnd_init(cnd_t *cond) {
	return pthread_cond_init(cond, 0) == 0 ? thrd_success : thrd_error;
}

__inline__ void cnd_destroy(cnd_t *cond) {
	pthread_cond_destroy(cond);
}

__inline__ int cnd_signal(cnd_t *cond) {
	return pthread_cond_signal(cond) == 0 ? thrd_success : thrd_error;
}

__inline__ int cnd_broadcast(cnd_t *cond) {
	return pthread_cond_broadcast(cond) == 0 ? thrd_success : thrd_error;
}

__inline__ int cnd_wait(cnd_t *cond, mtx_t *mtx) {
	return pthread_cond_wait(cond, mtx) == 0 ? thrd_success : thrd_error;
}

__inline__ int cnd_timedwait(cnd_t *cond, mtx_t *mtx, const struct timespec *ts) {
	int retval;

	if ((retval = pthread_cond_timedwait(cond, mtx, ts)) != 0) {
		return retval == ETIMEDOUT ? thrd_timedout : thrd_error;
	}
	return thrd_success;
}

__inline__ int tss_create(tss_t *key, tss_dtor_t dtor) {
	return pthread_key_create(key, dtor) == 0 ? thrd_success : thrd_error;
}

__inline__ void tss_delete(tss_t key) {
	pthread_key_delete(key);
}

__inline__ int tss_set(tss_t key, void *val) {
	return pthread_setspecific(key, val) == 0 ? thrd_success : thrd_error;
}

__inline__ void *tss_get(tss_t key) {
	return pthread_getspecific(key);
}

__inline__ void call_once(once_flag *flag, void (*func) (void)) {
	pthread_once(flag, func);
}

#endif

#else

#define NOCL_FEATURE_NO_THREADS

#endif

#if defined(__cplusplus)

}

#endif

#endif
