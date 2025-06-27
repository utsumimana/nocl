/*
 * Copyright (c) 1998-2019 Toni Ronkko
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

#if !defined(_NOCL_DIRENT_H)
#define _NOCL_DIRENT_H

#if defined(__cplusplus)

extern "C" {

#endif

#if defined(NOCL_HAS_DIRENT_H) || \
    /* POSIX.1-1988 */ ((defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 198808L) || \
	/* XPG3 */ (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 3))

#include <dirent.h>

#elif /* Win32 */ defined(_WIN32) && \
	defined(_MSC_VER) || defined(__MINGW32__)

#include "stddef.h"
#include "stdlib.h"
#include "lasterror.h"

#if defined(NOCL_FEATURE_NO_STDDEF) || defined(NOCL_FEATURE_NO_STDLIB) || defined(NOCL_FEATURE_NO_LASTERROR)

#define NOCL_FEATURE_NO_DIRENT

#else

#if defined(_DEBUG)

#define _CRTDBG_MAP_ALLOC

#include <malloc.h>
#include <crtdbg.h>

#endif

#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#if !defined(WIN32_LEAN_AND_MEAN)

#define WIN32_LEAN_AND_MEAN

#endif

#include <windows.h>

#if /* MinGW-w64 GCC 4.0.1 */ defined(__MINGW32__) && defined(__MINGW64_VERSION_MAJOR)

#include <dirent.h>

#elif defined(_MSC_VER)

/* Indicates that d_type field is available in dirent structure */
#define _DIRENT_HAVE_D_TYPE

/* Indicates that d_namlen field is available in dirent structure */
#define _DIRENT_HAVE_D_NAMLEN

/* Entries missing from MSVC 6.0 */
#if !defined(FILE_ATTRIBUTE_DEVICE)

#define FILE_ATTRIBUTE_DEVICE  0x40

#endif

/* File type and permission flags for stat(), general mask */
#if !defined(S_IFMT)

#define S_IFMT  _S_IFMT

#endif

/* Directory bit */
#if !defined(S_IFDIR)

#define S_IFDIR  _S_IFDIR

#endif

/* Character device bit */
#if !defined(S_IFCHR)

#define S_IFCHR  _S_IFCHR

#endif

/* Pipe bit */
#if !defined(S_IFFIFO)

#define S_IFFIFO  _S_IFFIFO

#endif

/* Regular file bit */
#if !defined(S_IFREG)

#define S_IFREG  _S_IFREG

#endif

/* Read permission */
#if !defined(S_IREAD)

#define S_IREAD  _S_IREAD

#endif

/* Write permission */
#if !defined(S_IWRITE)

#define S_IWRITE  _S_IWRITE

#endif

/* Execute permission */
#if !defined(S_IEXEC)

#define S_IEXEC  _S_IEXEC

#endif

/* Pipe */
#if !defined(S_IFIFO)

#define S_IFIFO  _S_IFIFO

#endif

/* Block device */
#if !defined(S_IFBLK)

#define S_IFBLK  0

#endif

/*
 * Symbolic link.  Be ware that S_IFLNK value and S_ISLNK() macro are only
 * usable with dirent - they do not work with stat() function call!
 */
#if !defined(S_IFLNK)

#define S_IFLNK  (_S_IFDIR | _S_IFREG)

#endif

/* Socket */
#if !defined(S_IFSOCK)

#define S_IFSOCK  0

#endif

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

/* Maximum length of file name */
#if !defined(PATH_MAX)

#define PATH_MAX  MAX_PATH

#endif

#if !defined(FILENAME_MAX)

#define FILENAME_MAX  MAX_PATH

#endif

#if !defined(NAME_MAX)

#define NAME_MAX  FILENAME_MAX

#endif

/* File type flags for d_type */
#define DT_UNKNOWN  0
#define DT_REG      S_IFREG
#define DT_DIR      S_IFDIR
#define DT_FIFO     S_IFIFO
#define DT_SOCK     S_IFSOCK
#define DT_CHR      S_IFCHR
#define DT_BLK      S_IFBLK
#define DT_LNK      S_IFLNK

/* Macros for converting between st_mode and d_type */
#define IFTODT(mode)  ((mode) & S_IFMT)
#define DTTOIF(type)  (type)

/*
 * File type macros.  Note that block devices and sockets cannot be
 * distinguished on Windows, and the macros S_ISBLK and S_ISSOCK are only
 * defined for compatibility.  These macros should always return false on
 * Windows.
 */
#if !defined(S_ISFIFO)

#define S_ISFIFO(mode)  (((mode) & S_IFMT) == S_IFIFO)

#endif

#if !defined(S_ISDIR)

#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)

#endif

#if !defined(S_ISREG)

#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)

#endif

#if !defined(S_ISLNK)

#define S_ISLNK(mode)  (((mode) & S_IFMT) == S_IFLNK)

#endif

#if !defined(S_ISSOCK)

#define S_ISSOCK(mode)  (((mode) & S_IFMT) == S_IFSOCK)

#endif

#if !defined(S_ISCHR)

#define S_ISCHR(mode)  (((mode) & S_IFMT) == S_IFCHR)

#endif

#if !defined(S_ISBLK)

#define S_ISBLK(mode)  (((mode) & S_IFMT) == S_IFBLK)

#endif

/* Return the exact length of the file name without zero terminator */
#define _D_EXACT_NAMLEN(p)  ((p)->d_namlen)

/* Return the maximum size of a file name */
#define _D_ALLOC_NAMLEN(p)  ((PATH_MAX) + 1)

/* Multi-byte character version */

typedef struct dirent {
	/* Always zero */
	long d_ino;

	/* Position of next file in a directory stream */
	long d_off;

	/* Structure size */
	unsigned short d_reclen;

	/* Length of name without \0 */
	size_t d_namlen;

	/* File type */
	int d_type;

	/* File name */
	char d_name[PATH_MAX + 1];
} dirent;

typedef struct DIR {
	/* Current directory entry */
	dirent ent;

	/* Private file data */
	WIN32_FIND_DATAA data;

	/* True if data is valid */
	int cached;

	/* True if next entry is invalid */
	int invalid;

	/* Win32 search handle */
	HANDLE handle;

	/* Initial directory name */
	char *patt;
} DIR;

/* Wide character version */

typedef struct _wdirent {
	/* Always zero */
	long d_ino;

	/* Position of next file in a directory stream */
	long d_off;

	/* Structure size */
	unsigned short d_reclen;

	/* Length of name without \0 */
	size_t d_namlen;

	/* File type */
	int d_type;

	/* File name */
	wchar_t d_name[PATH_MAX + 1];
} _wdirent;

typedef struct _WDIR {
	/* Current directory entry */
	_wdirent ent;

	/* Private file data */
	WIN32_FIND_DATAW data;

	/* True if data is valid */
	int cached;

	/* True if next entry is invalid */
	int invalid;

	/* Win32 search handle */
	HANDLE handle;

	/* Initial directory name */
	wchar_t *patt;
} _WDIR;

/* Get first directory entry */
__inline WIN32_FIND_DATAA *__cdecl __nocl_internal_dirent_first(DIR *dir) {

	/* Open directory and retrieve the first entry */
	dir->handle = FindFirstFileExA(
		dir->patt, FindExInfoStandard, &dir->data,
		FindExSearchNameMatch, NULL, 0);

	if (dir->handle == INVALID_HANDLE_VALUE) goto error;

	/* A directory entry is now waiting in memory */
	dir->cached = 1;
	return &dir->data;

error:

	/* Failed to open directory: no directory entry in memory */
	dir->cached = 0;
	dir->invalid = 1;

	/* Set error code */
	DWORD dwError = GetLastError();
	switch (dwError) {
		case ERROR_ACCESS_DENIED:
			/* No read access to directory */
			putstderrno(EACCES);
			break;
		case ERROR_DIRECTORY:
			/* Directory name is invalid */
			putstderrno(ENOTDIR);
			break;
		case ERROR_PATH_NOT_FOUND:
		default:
			/* Cannot find the file */
			putstderrno(ENOENT);
			break;
	}

	return NULL;
}

/* Get first directory entry */
__inline WIN32_FIND_DATAW *__cdecl __nocl_internal_dirent_wfirst(_WDIR *dir) {

	/* Open directory and retrieve the first entry */
	dir->handle = FindFirstFileExW(
		dir->patt, FindExInfoStandard, &dir->data,
		FindExSearchNameMatch, NULL, 0);

	if (dir->handle == INVALID_HANDLE_VALUE) goto error;

	/* A directory entry is now waiting in memory */
	dir->cached = 1;
	return &dir->data;

error:

	/* Failed to open directory: no directory entry in memory */
	dir->cached = 0;
	dir->invalid = 1;

	/* Set error code */
	DWORD dwError = GetLastError();
	switch (dwError) {
		case ERROR_ACCESS_DENIED:
			/* No read access to directory */
			putstderrno(EACCES);
			break;
		case ERROR_DIRECTORY:
			/* Directory name is invalid */
			putstderrno(ENOTDIR);
			break;
		case ERROR_PATH_NOT_FOUND:
		default:
			/* Cannot find the file */
			putstderrno(ENOENT);
			break;
	}

	return NULL;
}

/* Get next directory entry */
__inline WIN32_FIND_DATAA *__cdecl __nocl_internal_dirent_next(DIR *dir) {

	/* Return NULL if seek position was invalid */
	if (dir->invalid) return NULL;

	/* Is the next directory entry already in cache? */
	if (dir->cached) {
		/* Yes, a valid directory entry found in memory */
		dir->cached = 0;
		return &dir->data;
	}

	/* Read the next directory entry from stream */
	if (!FindNextFileA(dir->handle, &dir->data)) {
		/* End of directory stream */
		return NULL;
	}

	/* Success */
	return &dir->data;
}

/* Get next directory entry */
__inline WIN32_FIND_DATAW *__cdecl __nocl_internal_dirent_wnext(_WDIR *dir) {

	/* Return NULL if seek position was invalid */
	if (dir->invalid) return NULL;

	/* Is the next directory entry already in cache? */
	if (dir->cached) {
		/* Yes, a valid directory entry found in memory */
		dir->cached = 0;
		return &dir->data;
	}

	/* Read the next directory entry from stream */
	if (!FindNextFileW(dir->handle, &dir->data)) {
		/* End of directory stream */
		return NULL;
	}

	/* Success */
	return &dir->data;
}

/* Compute 31-bit hash of file name. */
__inline long __cdecl __nocl_internal_dirent_hash(const WIN32_FIND_DATAA *data) {

	unsigned long hash = 5381;
	unsigned long c;
	const char *p = data->cFileName;
	const char *e = p + MAX_PATH;

	while (p != e && (c = *p ++) != 0) {
		hash = (hash << 5) + hash + c;
	}

	return (long) (hash & ((~0UL) >> 1));
}

/* Compute 31-bit hash of file name. */
__inline long __cdecl __nocl_internal_dirent_whash(const WIN32_FIND_DATAW *data) {

	unsigned long hash = 5381;
	unsigned long c;
	const wchar_t *p = data->cFileName;
	const wchar_t *e = p + MAX_PATH;

	while (p != e && (c = *p ++) != 0) {
		hash = (hash << 5) + hash + c;
	}

	return (long) (hash & ((~0UL) >> 1));
}

/*
 * Close directory stream opened by opendir() function.  This invalidates the
 * DIR structure as well as any directory entry read previously by
 * readdir().
 */
__inline int __cdecl closedir(DIR *dir) {

	if (!dir) {
		putstderrno(EBADF);
		return -1 /* failure */;
	}

	/*
	 * Release search handle if we have one.  Being able to handle
	 * partially initialized DIR structure allows us to use this
	 * function to handle errors occurring within opendir.
	 */
	if (dir->handle != INVALID_HANDLE_VALUE) FindClose(dir->handle);

	/*
	 * Release search pattern.  Note that we don't need to care if
	 * dir->patt is NULL or not: function free is guaranteed to act
	 * appropriately.
	 */
	free(dir->patt);

	/* Release directory structure */
	free(dir);

	return 0 /* success */;
}

/*
 * Close directory stream opened by _wopendir() function.  This invalidates the
 * _WDIR structure as well as any directory entry read previously by
 * _wreaddir().
 */
__inline int __cdecl _wclosedir(_WDIR *dir) {

	if (!dir) {
		putstderrno(EBADF);
		return -1 /* failure */;
	}

	/*
	 * Release search handle if we have one.  Being able to handle
	 * partially initialized DIR structure allows us to use this
	 * function to handle errors occurring within opendir.
	 */
	if (dir->handle != INVALID_HANDLE_VALUE) FindClose(dir->handle);

	/*
	 * Release search pattern.  Note that we don't need to care if
	 * dir->patt is NULL or not: function free is guaranteed to act
	 * appropriately.
	 */
	free(dir->patt);

	/* Release directory structure */
	free(dir);

	return 0 /* success */;
}

/*
 * Open directory stream DIRNAME for read and return a pointer to the
 * internal working area that is used to retrieve individual directory
 * entries.
 */
__inline DIR *__cdecl opendir(const char *dirname) {

	char *p;

	/* Must have directory name */
	if (!dirname || dirname[0] == '\0') {
		putstderrno(ENOENT);
		return NULL;
	}

	/* Allocate new DIR structure */
	DIR *dir = malloc(sizeof(struct DIR));
	if (!dir) return NULL;

	/* Reset DIR structure */
	dir->handle = INVALID_HANDLE_VALUE;
	dir->patt = NULL;
	dir->cached = 0;
	dir->invalid = 0;

	/*
	 * Compute the length of full path plus zero terminator
	 *
	 * Note that on WinRT there's no way to convert relative paths
	 * into absolute paths, so just assume it is an absolute path.
	 */
#if !defined(WINAPI_FAMILY_PARTITION) || WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)

	/* Desktop */
	DWORD n = GetFullPathNameA(dirname, 0, NULL, NULL);

#else

	/* WinRT */
	size_t n = strlen(dirname);
#endif

	/* Allocate room for absolute directory name and search pattern */
	dir->patt = (char *) malloc(n + 8);
	if (!dir->patt) goto exit_closedir;

	/*
	 * Convert relative directory name to an absolute one.  This
	 * allows rewinddir() to function correctly even when current
	 * working directory is changed between opendir() and rewinddir().
	 *
	 * Note that on WinRT there's no way to convert relative paths
	 * into absolute paths, so just assume it is an absolute path.
	 */
#if !defined(WINAPI_FAMILY_PARTITION) || WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)

	/* Desktop */
	n = GetFullPathNameA(dirname, n, dir->patt, NULL);
	if (n <= 0) goto exit_closedir;

#else

	/* WinRT */
	strncpy_s(dir->patt, n + 1, dirname, n);

#endif

	/* Append search pattern \* to the directory name */
	p = dir->patt + n;
	switch (p[-1]) {
		case '\\':
		case '/':
		case ':':
			/* Directory ends in path separator, e.g. C:\temp\ */
			/* NOP */
			break;
		default:
			/* Directory name doesn't end in path separator */
			*p ++ = '\\';
			break;
	}
	*p ++ = '*';
	*p = '\0';

	/* Open directory stream and retrieve the first entry */
	if (!__nocl_internal_dirent_first(dir))
		goto exit_closedir;

	/* Success */
	return dir;

	/* Failure */
exit_closedir:
	closedir(dir);

	return NULL;
}

/*
 * Open directory stream DIRNAME for read and return a pointer to the
 * internal working area that is used to retrieve individual directory
 * entries.
 */
__inline _WDIR *__cdecl _wopendir(const wchar_t *dirname) {

	wchar_t *p;

	/* Must have directory name */
	if (!dirname || dirname[0] == L'\0') {
		putstderrno(ENOENT);
		return NULL;
	}

	/* Allocate new _WDIR structure */
	_WDIR *dir = malloc(sizeof(struct _WDIR));
	if (!dir) return NULL;

	/* Reset _WDIR structure */
	dir->handle = INVALID_HANDLE_VALUE;
	dir->patt = NULL;
	dir->cached = 0;
	dir->invalid = 0;

	/*
	 * Compute the length of full path plus zero terminator
	 *
	 * Note that on WinRT there's no way to convert relative paths
	 * into absolute paths, so just assume it is an absolute path.
	 */
#if !defined(WINAPI_FAMILY_PARTITION) || WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)

	/* Desktop */
	DWORD n = GetFullPathNameW(dirname, 0, NULL, NULL);

#else

	/* WinRT */
	size_t n = wcslen(dirname);
#endif

	/* Allocate room for absolute directory name and search pattern */
	dir->patt = (wchar_t *) malloc(n + 16);
	if (!dir->patt) goto exit_closedir;

	/*
	 * Convert relative directory name to an absolute one.  This
	 * allows rewinddir() to function correctly even when current
	 * working directory is changed between opendir() and rewinddir().
	 *
	 * Note that on WinRT there's no way to convert relative paths
	 * into absolute paths, so just assume it is an absolute path.
	 */
#if !defined(WINAPI_FAMILY_PARTITION) || WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)

	/* Desktop */
	n = GetFullPathNameW(dirname, n, dir->patt, NULL);
	if (n <= 0) goto exit_closedir;

#else

	/* WinRT */
	wcsncpy_s(dir->patt, n + 1, dirname, n);

#endif

	/* Append search pattern \* to the directory name */
	p = dir->patt + n;
	switch (p[-1]) {
		case L'\\':
		case L'/':
		case L':':
			/* Directory ends in path separator, e.g. C:\temp\ */
			/* NOP */
			break;
		default:
			/* Directory name doesn't end in path separator */
			*p ++ = L'\\';
			break;
	}
	*p ++ = L'*';
	*p = L'\0';

	/* Open directory stream and retrieve the first entry */
	if (!__nocl_internal_dirent_wfirst(dir))
		goto exit_closedir;

	/* Success */
	return dir;

	/* Failure */
exit_closedir:
	_wclosedir(dir);

	return NULL;
}

/*
 * Read next directory entry into called-allocated buffer.
 *
 * Returns zero on success.  If end of directory stream is reached, then sets
 * result to NULL and returns zero.
 */
__inline int __cdecl readdir_r(DIR *dir, dirent *entry, dirent **res) {

	/* Validate directory handle */
	if (!dir || dir->handle == INVALID_HANDLE_VALUE || !dir->patt) {
		putstderrno(EBADF);
		*res = NULL;
		return -1;
	}

	/* Read next directory entry */
	WIN32_FIND_DATAA *data = __nocl_internal_dirent_next(dir);

	if (!data) {
		/* Return NULL to indicate end of directory */
		*res = NULL;
		return 0 /* OK */;
	}

	/*
	 * Copy file name as wide-character string.  If the file name is too
	 * long to fit in to the destination buffer, then truncate file name
	 * to PATH_MAX characters and zero-terminate the buffer.
	 */
	size_t i = 0;
	while (i < PATH_MAX && data->cFileName[i] != 0) {
		entry->d_name[i] = data->cFileName[i];
		i ++;
	}
	entry->d_name[i] = 0;

	/* Length of file name excluding zero terminator */
	entry->d_namlen = i;

	/* Determine file type */
	DWORD attr = data->dwFileAttributes;
	if (attr & FILE_ATTRIBUTE_DEVICE) entry->d_type = DT_CHR;
	else if (attr & FILE_ATTRIBUTE_REPARSE_POINT) entry->d_type = DT_LNK;
	else if (attr & FILE_ATTRIBUTE_DIRECTORY) entry->d_type = DT_DIR;
	else entry->d_type = DT_REG;

	/* Read the next directory entry to cache */
	data = __nocl_internal_dirent_next(dir);
	if (data) {
		/* Compute 31-bit hash of the next directory entry */
		entry->d_off = __nocl_internal_dirent_hash(data);

		/* Push the next directory entry back to cache */
		dir->cached = 1;
	}
	else {
		/* End of directory stream */
		entry->d_off = (long) ((~0UL) >> 1);
	}

	/* Reset other fields */
	entry->d_ino = 0;
	entry->d_reclen = sizeof(struct dirent);

	/* Set result address */
	*res = entry;

	return 0 /* OK */;
}

/*
 * Read next directory entry into called-allocated buffer.
 *
 * Returns zero on success.  If end of directory stream is reached, then sets
 * result to NULL and returns zero.
 */
__inline int __cdecl _wreaddir_r(_WDIR *dir, _wdirent *entry, _wdirent **res) {

	/* Validate directory handle */
	if (!dir || dir->handle == INVALID_HANDLE_VALUE || !dir->patt) {
		putstderrno(EBADF);
		*res = NULL;
		return -1;
	}

	/* Read next directory entry */
	WIN32_FIND_DATAW *data = __nocl_internal_dirent_wnext(dir);

	if (!data) {
		/* Return NULL to indicate end of directory */
		*res = NULL;
		return 0 /* OK */;
	}

	/*
	 * Copy file name as wide-character string.  If the file name is too
	 * long to fit in to the destination buffer, then truncate file name
	 * to PATH_MAX characters and zero-terminate the buffer.
	 */
	size_t i = 0;
	while (i < PATH_MAX && data->cFileName[i] != 0) {
		entry->d_name[i] = data->cFileName[i];
		i ++;
	}
	entry->d_name[i] = 0;

	/* Length of file name excluding zero terminator */
	entry->d_namlen = i;

	/* Determine file type */
	DWORD attr = data->dwFileAttributes;
	if (attr & FILE_ATTRIBUTE_DEVICE) entry->d_type = DT_CHR;
	else if (attr & FILE_ATTRIBUTE_REPARSE_POINT) entry->d_type = DT_LNK;
	else if (attr & FILE_ATTRIBUTE_DIRECTORY) entry->d_type = DT_DIR;
	else entry->d_type = DT_REG;

	/* Read the next directory entry to cache */
	data = __nocl_internal_dirent_wnext(dir);
	if (data) {
		/* Compute 31-bit hash of the next directory entry */
		entry->d_off = __nocl_internal_dirent_whash(data);

		/* Push the next directory entry back to cache */
		dir->cached = 1;
	}
	else {
		/* End of directory stream */
		entry->d_off = (long) ((~0UL) >> 1);
	}

	/* Reset other fields */
	entry->d_ino = 0;
	entry->d_reclen = sizeof(struct _wdirent);

	/* Set result address */
	*res = entry;

	return 0 /* OK */;
}

/*
 * Read next directory entry.
 *
 * Returns pointer to static directory entry which may be overwritten by
 * subsequent calls to readdir().
 */
__inline dirent *__cdecl readdir(DIR *dir) {

	/*
	 * Read directory entry to buffer.  We can safely ignore the return
	 * value as entry will be set to NULL in case of error.
	 */
	dirent *entry;
	(void) readdir_r(dir, &dir->ent, &entry);

	/* Return pointer to statically allocated directory entry */

	return entry;
}

/*
 * Read next directory entry.
 *
 * Returns pointer to static directory entry which may be overwritten by
 * subsequent calls to _wreaddir().
 */
__inline _wdirent *__cdecl _wreaddir(_WDIR *dir) {

	/*
	 * Read directory entry to buffer.  We can safely ignore the return
	 * value as entry will be set to NULL in case of error.
	 */
	_wdirent *entry;
	(void) _wreaddir_r(dir, &dir->ent, &entry);

	/* Return pointer to statically allocated directory entry */

	return entry;
}

/*
 * Rewind directory stream such that readdir() returns the very first
 * file name again.
 */
__inline void __cdecl rewinddir(DIR *dir) {

	/* Check directory pointer */
	if (!dir || dir->handle == INVALID_HANDLE_VALUE || !dir->patt) return;

	/* Release existing search handle */
	FindClose(dir->handle);

	/* Open new search handle */
	__nocl_internal_dirent_first(dir);
}

/*
 * Rewind directory stream such that readdir() returns the very first
 * file name again.
 */
__inline void __cdecl _wrewinddir(_WDIR *dir) {

	/* Check directory pointer */
	if (!dir || dir->handle == INVALID_HANDLE_VALUE || !dir->patt) return;

	/* Release existing search handle */
	FindClose(dir->handle);

	/* Open new search handle */
	__nocl_internal_dirent_wfirst(dir);
}

/* Get position of directory stream */
__inline long __cdecl telldir(DIR *dir) {

	if (!dir || dir->handle == INVALID_HANDLE_VALUE) {
		putstderrno(EBADF);
		return -1 /* failure */;
	}

	/* Read next file entry */
	WIN32_FIND_DATAA *data = __nocl_internal_dirent_next(dir);
	if (!data) {
		/* End of directory stream */
		return ((~0UL) >> 1);
	}

	/* Store file entry to cache for readdir() */
	dir->cached = 1;

	/* Return the 31-bit hash code to be used as stream position */
	return __nocl_internal_dirent_hash(data);
}

/* Get position of directory stream */
__inline long __cdecl _wtelldir(_WDIR *dir) {

	if (!dir || dir->handle == INVALID_HANDLE_VALUE) {
		putstderrno(EBADF);
		return -1 /* failure */;
	}

	/* Read next file entry */
	WIN32_FIND_DATAW *data = __nocl_internal_dirent_wnext(dir);
	if (!data) {
		/* End of directory stream */
		return ((~0UL) >> 1);
	}

	/* Store file entry to cache for readdir() */
	dir->cached = 1;

	/* Return the 31-bit hash code to be used as stream position */
	return __nocl_internal_dirent_whash(data);
}

/* Seek directory stream to offset */
__inline void __cdecl seekdir(DIR *dir, long loc) {

	if (!dir) return;

	/* Directory must be open */
	if (dir->handle == INVALID_HANDLE_VALUE) goto exit_failure;

	/* Ensure that seek position is valid */
	if (loc < 0) goto exit_failure;

	/* Restart directory stream from the beginning */
	FindClose(dir->handle);
	if (!__nocl_internal_dirent_first(dir)) goto exit_failure;

	/* Reset invalid flag so that we can read from the stream again */
	dir->invalid = 0;

	/*
	 * Read directory entries from the beginning until the hash matches a
	 * file name.  Be ware that hash code is only 31 bits longs and
	 * duplicates are possible: the hash code cannot return the position
	 * with 100.00% accuracy! Moreover, the method is slow for large
	 * directories.
	 */
	long hash;
	do {
		/* Read next directory entry */
		WIN32_FIND_DATAA *data = __nocl_internal_dirent_next(dir);
		if (!data) {
			/*
			 * End of directory stream was reached before finding
			 * the requested location.  Perhaps the file in
			 * question was deleted or moved out of the directory.
			 */
			goto exit_failure;
		}

		/* Does the file name match the hash? */
		hash = __nocl_internal_dirent_hash(data);
	}
	while (hash != loc);

	/*
	 * File name matches the hash!  Push the directory entry back to cache
	 * from where next readdir() will return it.
	 */
	dir->cached = 1;
	dir->invalid = 0;
	return;

exit_failure:
	/* Ensure that readdir will return NULL */
	dir->invalid = 1;
}

/* Seek directory stream to offset */
__inline void __cdecl _wseekdir(_WDIR *dir, long loc) {

	if (!dir) return;

	/* Directory must be open */
	if (dir->handle == INVALID_HANDLE_VALUE) goto exit_failure;

	/* Ensure that seek position is valid */
	if (loc < 0) goto exit_failure;

	/* Restart directory stream from the beginning */
	FindClose(dir->handle);
	if (!__nocl_internal_dirent_wfirst(dir)) goto exit_failure;

	/* Reset invalid flag so that we can read from the stream again */
	dir->invalid = 0;

	/*
	 * Read directory entries from the beginning until the hash matches a
	 * file name.  Be ware that hash code is only 31 bits longs and
	 * duplicates are possible: the hash code cannot return the position
	 * with 100.00% accuracy! Moreover, the method is slow for large
	 * directories.
	 */
	long hash;
	do {
		/* Read next directory entry */
		WIN32_FIND_DATAW *data = __nocl_internal_dirent_wnext(dir);
		if (!data) {
			/*
			 * End of directory stream was reached before finding
			 * the requested location.  Perhaps the file in
			 * question was deleted or moved out of the directory.
			 */
			goto exit_failure;
		}

		/* Does the file name match the hash? */
		hash = __nocl_internal_dirent_whash(data);
	}
	while (hash != loc);

	/*
	 * File name matches the hash!  Push the directory entry back to cache
	 * from where next readdir() will return it.
	 */
	dir->cached = 1;
	dir->invalid = 0;
	return;

exit_failure:
	/* Ensure that readdir will return NULL */
	dir->invalid = 1;
}

/* Scan directory for entries */
__inline int __cdecl scandir(const char *dirname, dirent ***namelist,
	int (*filter) (const dirent *), int (*compare) (const dirent **, const dirent **)) {

	int retval;

	/* Open directory stream */
	DIR *dir = opendir(dirname);
	if (!dir) {
		/* Cannot open directory */
		return -1 /* Error */;
	}

	/* Read directory entries to memory */
	dirent *tmp = NULL;
	dirent **files = NULL;
	size_t size = 0;
	size_t allocated = 0;
	while (1) {
		/* Allocate room for a temporary directory entry */
		if (!tmp) {
			tmp = (dirent*) malloc(sizeof(dirent));
			if (!tmp) goto exit_failure;
		}

		/* Read directory entry to temporary area */
		struct dirent *entry;
		if (readdir_r(dir, tmp, &entry) != 0 /* OK */)
			goto exit_failure;

		/* Stop if we already read the last directory entry */
		if (!entry) goto exit_success;

		/* Determine whether to include the entry in results */
		if (filter && !filter(tmp)) continue;

		/* Enlarge pointer table to make room for another pointer */
		if (size >= allocated) {
			/* Compute number of entries in the new table */
			size_t num_entries = size * 2 + 16;

			/* Allocate new pointer table or enlarge existing */
			void *p = realloc(files, sizeof(void *) * num_entries);
			if (!p)
				goto exit_failure;

			/* Got the memory */
			files = (dirent **) p;
			allocated = num_entries;
		}

		/* Store the temporary entry to ptr table */
		files[size ++] = tmp;
		tmp = NULL;
	}

exit_failure:
	/* Release allocated entries */
	for (size_t i = 0; i < size; i ++) {
		free(files[i]);
	}

	/* Release the pointer table */
	free(files);
	files = NULL;

	/* Exit with error code */
	retval = -1 /* error */;
	goto exit_status;

exit_success:
	/* Sort directory entries */
	if (size > 1 && compare) {
		qsort(files, size, sizeof(void *),
			(int (*) (const void *, const void *)) compare);
	}

	/* Pass pointer table to caller */
	if (namelist) *namelist = files;

	/* Return the number of directory entries read */
	retval = (int) size;

exit_status:
	/* Release temporary directory entry, if we had one */
	free(tmp);

	/* Close directory stream */
	closedir(dir);

	return retval;
}

/* Scan directory for entries */
__inline int __cdecl _wscandir(const wchar_t *dirname, _wdirent ***namelist,
	int (*filter) (const _wdirent *), int (*compare) (const _wdirent **, const _wdirent **)) {

	int retval;

	/* Open directory stream */
	_WDIR *dir = _wopendir(dirname);
	if (!dir) {
		/* Cannot open directory */
		return -1 /* Error */;
	}

	/* Read directory entries to memory */
	_wdirent *tmp = NULL;
	_wdirent **files = NULL;
	size_t size = 0;
	size_t allocated = 0;
	while (1) {
		/* Allocate room for a temporary directory entry */
		if (!tmp) {
			tmp = (_wdirent*) malloc(sizeof(_wdirent));
			if (!tmp) goto exit_failure;
		}

		/* Read directory entry to temporary area */
		struct _wdirent *entry;
		if (_wreaddir_r(dir, tmp, &entry) != 0 /* OK */)
			goto exit_failure;

		/* Stop if we already read the last directory entry */
		if (!entry) goto exit_success;

		/* Determine whether to include the entry in results */
		if (filter && !filter(tmp)) continue;

		/* Enlarge pointer table to make room for another pointer */
		if (size >= allocated) {
			/* Compute number of entries in the new table */
			size_t num_entries = size * 2 + 16;

			/* Allocate new pointer table or enlarge existing */
			void *p = realloc(files, sizeof(void *) * num_entries);
			if (!p)
				goto exit_failure;

			/* Got the memory */
			files = (_wdirent **) p;
			allocated = num_entries;
		}

		/* Store the temporary entry to ptr table */
		files[size ++] = tmp;
		tmp = NULL;
	}

exit_failure:
	/* Release allocated entries */
	for (size_t i = 0; i < size; i ++) {
		free(files[i]);
	}

	/* Release the pointer table */
	free(files);
	files = NULL;

	/* Exit with error code */
	retval = -1 /* error */;
	goto exit_status;

exit_success:
	/* Sort directory entries */
	if (size > 1 && compare) {
		qsort(files, size, sizeof(void *),
			(int (*) (const void *, const void *)) compare);
	}

	/* Pass pointer table to caller */
	if (namelist) *namelist = files;

	/* Return the number of directory entries read */
	retval = (int) size;

exit_status:
	/* Release temporary directory entry, if we had one */
	free(tmp);

	/* Close directory stream */
	_wclosedir(dir);

	return retval;
}

/* Alphabetical sorting */
__inline int __cdecl alphasort(const dirent **a, const dirent **b) {
	return strcoll((*a)->d_name, (*b)->d_name);
}

/* Alphabetical sorting */
__inline int __cdecl _walphasort(const _wdirent **a, const _wdirent **b) {
	return wcscoll((*a)->d_name, (*b)->d_name);
}

/* Compare strings */
__inline int __cdecl __nocl_internal_dirent_strverscmp(const char *a, const char *b) {
	size_t i = 0;
	size_t j;

	/* Find first difference */
	while (a[i] == b[i]) {
		if (a[i] == '\0') {
			/* No difference */
			return 0;
		}
		++ i;
	}

	/* Count backwards and find the leftmost digit */
	j = i;
	while (j > 0 && isdigit(a[j-1])) {
		-- j;
	}

	/* Determine mode of comparison */
	if (a[j] == '0' || b[j] == '0') {
		/* Find the next non-zero digit */
		while (a[j] == '0' && a[j] == b[j]) {
			j ++;
		}

		/* String with more digits is smaller, e.g 002 < 01 */
		if (isdigit(a[j])) {
			if (!isdigit(b[j])) {
				return -1;
			}
		}
		else if (isdigit(b[j])) {
			return 1;
		}
	}
	else if (isdigit(a[j]) && isdigit(b[j])) {
		/* Numeric comparison */
		size_t k1 = j;
		size_t k2 = j;

		/* Compute number of digits in each string */
		while (isdigit(a[k1])) {
			k1 ++;
		}
		while (isdigit(b[k2])) {
			k2 ++;
		}

		/* Number with more digits is bigger, e.g 999 < 1000 */
		if (k1 < k2)
			return -1;
		else if (k1 > k2)
			return 1;
	}

	/* Alphabetical comparison */
	return (int) ((unsigned char) a[i]) - ((unsigned char) b[i]);
}

/* Sort versions */
__inline int __cdecl versionsort(const dirent **a, const dirent **b) {
	return __nocl_internal_dirent_strverscmp((*a)->d_name, (*b)->d_name);
}

/* Compare strings */
__inline int __cdecl __nocl_internal_dirent_wcsverscmp(const wchar_t *a, const wchar_t *b) {
	size_t i = 0;
	size_t j;

	/* Find first difference */
	while (a[i] == b[i]) {
		if (a[i] == L'\0') {
			/* No difference */
			return 0;
		}
		++ i;
	}

	/* Count backwards and find the leftmost digit */
	j = i;
	while (j > 0 && iswdigit(a[j-1])) {
		-- j;
	}

	/* Determine mode of comparison */
	if (a[j] == L'0' || b[j] == L'0') {
		/* Find the next non-zero digit */
		while (a[j] == L'0' && a[j] == b[j]) {
			j ++;
		}

		/* String with more digits is smaller, e.g 002 < 01 */
		if (iswdigit(a[j])) {
			if (!iswdigit(b[j])) {
				return -1;
			}
		}
		else if (iswdigit(b[j])) {
			return 1;
		}
	}
	else if (iswdigit(a[j]) && iswdigit(b[j])) {
		/* Numeric comparison */
		size_t k1 = j;
		size_t k2 = j;

		/* Compute number of digits in each string */
		while (iswdigit(a[k1])) {
			k1 ++;
		}
		while (iswdigit(b[k2])) {
			k2 ++;
		}

		/* Number with more digits is bigger, e.g 999 < 1000 */
		if (k1 < k2)
			return -1;
		else if (k1 > k2)
			return 1;
	}

	/* Alphabetical comparison */
	return (int) ((unsigned short) a[i]) - ((unsigned short) b[i]);
}

/* Sort versions */
__inline int __cdecl _wversionsort(const _wdirent **a, const _wdirent **b) {
	return __nocl_internal_dirent_wcsverscmp((*a)->d_name, (*b)->d_name);
}

#endif

#endif

#else

#define NOCL_FEATURE_NO_DIRENT

#endif

#if defined(__cplusplus)

}

#endif

#endif
