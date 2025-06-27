/*
 * Copyright (c) 2011 Dongsheng Song
 * Copyright (c) 2015 Daniel Tillett
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

#if !defined(_NOCL_SEMAPHORE_H)
#define _NOCL_SEMAPHORE_H

#if defined(__cplusplus)

extern "C" {

#endif

#if defined(NOCL_HAS_DIRENT_H) || \
    /* POSIX.1b-1993 */ ((defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 199309L) || \
	/* UNIX98 */ (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500))

#include <semaphore.h>

#elif /* Win32 */ defined(_WIN32) && \
	/* MSVC 2.0 */ ((defined(_MSC_VER) && _MSC_VER >= 900) || \
	defined(__MINGW32__))

#include "restrict.h"

#include "time.h"
#include "lasterror.h"

#if defined(NOCL_FEATURE_NO_TIME) || defined(NOCL_FEATURE_NO_LASTERROR)

#define NOCL_FEATURE_NO_SEMAPHORE

#else

#if !defined(WIN32_LEAN_AND_MEAN)

#define WIN32_LEAN_AND_MEAN

#endif

#include <windows.h>
#include <ctype.h>
#include <string.h>
#include <wctype.h>
#include <wchar.h>

#if !defined(ETIMEDOUT)

#define ETIMEDOUT  138  /* This is the value in UCRT. */

#endif

#if !defined(EOVERFLOW)

#define EOVERFLOW  132  /* This is the value in UCRT. */

#endif

#if !defined(_O_CREAT)

#define _O_CREAT  0x0100

#endif

#if !defined(_O_EXCL)

#define _O_EXCL  0x0400

#endif

#if !defined(O_CREAT)

#define O_CREAT  _O_CREAT

#endif

#if !defined(O_EXCL)

#define O_EXCL  _O_EXCL

#endif

typedef HANDLE sem_t;

#define SEM_FAILED     ((sem_t *) NULL)
#define SEM_VALUE_MAX  0x7fffffff

/* Initialize an unnamed semaphore */
__inline int __cdecl sem_init(sem_t *sem, int pshared, unsigned int value) {

	if (!sem || value > (unsigned int) SEM_VALUE_MAX) {
		SetLastError(ERROR_INVALID_PARAMETER);
		putstderrno(EINVAL);
		return -1;
	}

	if (pshared != 0) {
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		putstderrno(ENOSYS);
		return -1;
	}

	HANDLE h = CreateSemaphoreA(NULL, value, SEM_VALUE_MAX, NULL);

	if (!h) {
		putstderrno(ENOSPC);
		return -1;
	}

	*sem = h;

	return 0;
}

/* Decrement the semaphore. */
__inline int __cdecl sem_wait(sem_t *sem) {

	HANDLE h;

	if (!sem || !((h = *sem))) {
		SetLastError(ERROR_INVALID_PARAMETER);
		putstderrno(EINVAL);
		return -1;
	}

	if (WaitForSingleObject(h, INFINITE) != WAIT_OBJECT_0) {
		putstderrno(EINTR);
		return -1;
	}

	return 0;
}

/* Try to decrement the semaphore. */
__inline int __cdecl sem_trywait(sem_t *sem) {

	HANDLE h;

	if (!sem || !((h = *sem))) {
		SetLastError(ERROR_INVALID_PARAMETER);
		putstderrno(EINVAL);
		return -1;
	}

	DWORD retval;

	if ((retval = WaitForSingleObject(h, 0)) == WAIT_OBJECT_0)
		return 0;

	if (retval == WAIT_TIMEOUT) {
		putstderrno(EAGAIN);
		return -1;
	}

	putstderrno(EINVAL);
	return -1;
}

/* Try to decrement the semaphore. */
__inline int __cdecl sem_timedwait(sem_t *sem, const struct timespec *restrict abs_timeout) {

	HANDLE h;

	if (!sem || !((h = *sem))) {
		SetLastError(ERROR_INVALID_PARAMETER);
		putstderrno(EINVAL);
		return -1;
	}

	if (abs_timeout->tv_nsec < 0 || abs_timeout->tv_nsec >= 1000000000L) {
		putstderrno(EINVAL);
		return -1;
	}

	DWORD retval;

	if ((retval = WaitForSingleObject(h, arch_rel_time_in_ms(abs_timeout))) == WAIT_OBJECT_0)
		return 0;

	if (retval == WAIT_TIMEOUT) {
		putstderrno(ETIMEDOUT);
		return -1;
	}

	putstderrno(EINVAL);
	return -1;
}

/* Increment the semaphore. */
__inline int __cdecl sem_post(sem_t *sem) {

	HANDLE h;

	if (!sem || !((h = *sem))) {
		SetLastError(ERROR_INVALID_PARAMETER);
		putstderrno(EINVAL);
		return -1;
	}

	if (!ReleaseSemaphore(h, 1, NULL)) {
		DWORD dwError = GetLastError();
		if (dwError == ERROR_TOO_MANY_POSTS) putstderrno(EOVERFLOW);
		else putstderrno(EINVAL);
		return -1;
	}

	return 0;
}

/* Get the value of the semaphore. */
__inline int __cdecl sem_getvalue(sem_t *restrict sem, int *restrict sval) {

	HANDLE h;

	if (!sem || !((h = *sem))) {
		SetLastError(ERROR_INVALID_PARAMETER);
		putstderrno(EINVAL);
		return -1;
	}

	long prev;
	switch (WaitForSingleObject(h, 0)) {
		case WAIT_OBJECT_0:
			if (!ReleaseSemaphore(h, 1, &prev)) {
				putstderrno(EINVAL);
				return -1;
			}
			*sval = prev + 1;
			return 0;
		case WAIT_TIMEOUT:
			*sval = 0;
			return 0;
		default:
			putstderrno(EINVAL);
			return -1;
	}

}

/* Destroy the unnamed semaphore. */
__inline int __cdecl sem_destroy(sem_t *sem) {

	HANDLE h;

	if (!sem || !((h = *sem))) {
		SetLastError(ERROR_INVALID_PARAMETER);
		putstderrno(EINVAL);
		return -1;
	}

	if (CloseHandle(h) == 0) {
		putstderrno(EINVAL);
		return -1;
	}

	*sem = NULL;
	return 0;
}

/* Open a named semaphore. */
__inline sem_t *__cdecl sem_open(const char *name, int oflag, DWORD dwDesiredAccess, unsigned int value) {

	int len;
	char buffer[MAX_PATH];

	if (value > (unsigned int) SEM_VALUE_MAX || (len = strlen(name)) > (int) sizeof(buffer) - 8 || len < 1) {
		putstderrno(EINVAL);

		return NULL;
	}

	memmove(buffer, "Global\\", 7);
	memmove(buffer + 7, name, len);
	buffer[len + 7] = '\0';

	HANDLE h;

	if (!((h = CreateSemaphoreExA(NULL, value, SEM_VALUE_MAX, buffer, 0, dwDesiredAccess)))) {
		switch (GetLastError()) {
			case ERROR_ACCESS_DENIED:
				putstderrno(EACCES);
				break;
			case ERROR_INVALID_HANDLE:
				putstderrno(ENOENT);
				break;
			default:
				putstderrno(ENOSPC);
				break;
		}

		return NULL;
	}
	else {
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			if ((oflag & O_CREAT) && (oflag & O_EXCL)) {
				CloseHandle(h);
				putstderrno(EEXIST);

				return NULL;
			}

			return h;
		}
		else {
			if (!(oflag & O_CREAT)) {
				putstderrno(ENOENT);

				return NULL;
			}
		}
	}

	return h;
}

/* Open a named semaphore. */
__inline sem_t *__cdecl _wsem_open(const wchar_t *name, int oflag, DWORD dwDesiredAccess, unsigned int value) {

	int len;
	wchar_t buffer[MAX_PATH];

	if (value > (unsigned int) SEM_VALUE_MAX || (len = wcslen(name)) > (int) (sizeof(buffer) / sizeof(wchar_t)) - 8 || len < 1) {
		putstderrno(EINVAL);

		return NULL;
	}

	wmemmove(buffer, L"Global\\", 7);
	wmemmove(buffer + 7, name, len);
	buffer[len + 7] = L'\0';

	HANDLE h;

	if (!((h = CreateSemaphoreExW(NULL, value, SEM_VALUE_MAX, buffer, 0, dwDesiredAccess)))) {
		switch (GetLastError()) {
			case ERROR_ACCESS_DENIED:
				putstderrno(EACCES);
				break;
			case ERROR_INVALID_HANDLE:
				putstderrno(ENOENT);
				break;
			default:
				putstderrno(ENOSPC);
				break;
		}

		return NULL;
	}
	else {
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			if ((oflag & O_CREAT) && (oflag & O_EXCL)) {
				CloseHandle(h);
				putstderrno(EEXIST);

				return NULL;
			}

			return h;
		}
		else {
			if (!(oflag & O_CREAT)) {
				putstderrno(ENOENT);

				return NULL;
			}
		}
	}

	return h;
}

/* Close the named semaphore. */
__inline int __cdecl sem_close(sem_t *sem) {
	return sem_destroy(sem);
}

/* Unlink the named semaphore. */
__inline int __cdecl sem_unlink(const char *name) {
	(void) name;
	return 0;
}

#endif

#else

#define NOCL_FEATURE_NO_SEMAPHORE

#endif

#if defined(__cplusplus)

}

#endif

#endif
