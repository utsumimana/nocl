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

#if !defined(_NOCL_DIRECT_H)
#define _NOCL_DIRECT_H

#if defined(__cplusplus)

extern "C" {

#endif

#if defined(NOCL_HAS_DIRECT_H) || \
    /* Win32 */ defined(_WIN32)

/* Read user permission */
#if !defined(S_IRUSR)

#define S_IRUSR  S_IREAD

#endif

/* Write user permission */
#if !defined(S_IWUSR)

#define S_IWUSR  S_IWRITE

#endif

/* Execute user permission */
#if !defined(S_IXUSR)

#define S_IXUSR  0

#endif

/* User full permissions */
#if !defined(S_IRWXU)

#define S_IRWXU  (S_IRUSR | S_IWUSR | S_IXUSR)

#endif

/* Read group permission */
#if !defined(S_IRGRP)

#define S_IRGRP  0

#endif

/* Write group permission */
#if !defined(S_IWGRP)

#define S_IWGRP  0

#endif

/* Execute group permission */
#if !defined(S_IXGRP)

#define S_IXGRP  0

#endif

/* Group full permissions */
#if !defined(S_IRWXG)

#define S_IRWXG  (S_IRGRP | S_IWGRP | S_IXGRP)

#endif

/* Read others permission */
#if !defined(S_IROTH)

#define S_IROTH  0

#endif

/* Write others permission */
#if !defined(S_IWOTH)

#define S_IWOTH  0

#endif

/* Execute others permission */
#if !defined(S_IXOTH)

#define S_IXOTH  0

#endif

/* Other full permissions */
#if !defined(S_IRWXO)

#define S_IRWXO  (S_IROTH | S_IWOTH | S_IXOTH)

#endif

#include <direct.h>

#define getcwd  _getcwd
#define chdir   _chdir
#define rmdir   _rmdir

#define mkdir(path,mode)  _mkdir((path))

#elif \
    /* POSIX.1-2001 */ ((defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L) || \
	/* UNIX03 */ (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 600))

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#else

#define NOCL_FEATURE_NO_DIRECT

#endif

#if defined(__cplusplus)

}

#endif

#endif
