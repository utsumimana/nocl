/*
 *  Copyright (c) 2007 Ramiro Polla
 *  Copyright (c) 2015 Tiancheng Gu
 *  Copyright (c) 2019 Pali Rohár
 *  Copyright (c) 2020 Ralf Habacker
 *  Copyright (c) 2025 Mana Utsumi
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

#if !defined(_NOCL_DLFCN_H)
#define _NOCL_DLFCN_H

#if defined(__cplusplus)

extern "C" {

#endif

#if defined(NOCL_HAS_DLFCN_H) || \
    /* POSIX.1-2001 */ (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L) || \
	/* UNIX03 */ (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 600)

#include <dlfcn.h>

#elif /* Win32 */ defined(_WIN32) && \
   /* MSC 1.0 */ (defined(_MSC_VER) || \
   /* MinGW/MinGW-w64 GCC 4.5.0 */ defined(__MINGW32__) && defined(__GNUC__) && (__GNUC__ >= 5 || (defined(__GNUC_MINOR__) && __GNUC__ == 4 && __GNUC_MINOR__ >= 5)))

#include "stddef.h"
#include "stdlib.h"
#include "noinline.h"

#if defined(NOCL_FEATURE_NO_STDDEF) || defined(NOCL_FEATURE_NO_STDLIB)

#define NOCL_FEATURE_NO_DL

#else

#if !defined(WIN32_LEAN_AND_MEAN)

#define WIN32_LEAN_AND_MEAN

#endif

#include <windows.h>

/* Relocations are performed when the object is loaded. */
#define RTLD_NOW     0

/*
 * Relocations are performed at an implementation-defined time.
 *
 * Windows API does not support lazy symbol resolving (when first reference
 * to a given symbol occurs). So RTLD_LAZY implementation is same as RTLD_NOW.
 */
#define RTLD_LAZY    RTLD_NOW

/* All symbols are available for relocation processing of other modules. */
#define RTLD_GLOBAL  (1 << 1)

/*
 * All symbols are not made available for relocation processing by other modules.
 *
 * Windows API does not support local symbol loading. So RTLD_LOCAL implementation
 * is same as RTLD_GLOBAL.
 */
#define RTLD_LOCAL   RTLD_GLOBAL

/*
 * These two were added in The Open Group Base Specifications Issue 6.
 * Note: All other RTLD_* flags in any dlfcn.h are not standard compliant.
 */

/* The symbol lookup happens in the normal global scope. */
#define RTLD_DEFAULT  ((void *) 0)

/* Specifies the next object after this one that defines name. */
#define RTLD_NEXT     ((void *) -1)

/* Structure filled in by dladdr() */
typedef struct Dl_info {
   const char *dli_fname;  /* Filename of defining object (thread unsafe and reused on every call to dladdr) */
   void       *dli_fbase;  /* Load address of that object */
   const char *dli_sname;  /* Name of nearest lower symbol */
   void       *dli_saddr;  /* Exact value of nearest symbol */
} Dl_info;

#if !defined(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)

#define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS  0x4

#endif

#if !defined(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)

#define GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT  0x2

#endif

#if !defined(IMAGE_NT_OPTIONAL_HDR_MAGIC)

#if defined(_WIN64)

#define IMAGE_NT_OPTIONAL_HDR_MAGIC  0x20b

#else

#define IMAGE_NT_OPTIONAL_HDR_MAGIC  0x10b

#endif

#endif

#if !defined(IMAGE_DIRECTORY_ENTRY_IAT)

#define IMAGE_DIRECTORY_ENTRY_IAT  12

#endif

#if !defined(LOAD_WITH_ALTERED_SEARCH_PATH)

#define LOAD_WITH_ALTERED_SEARCH_PATH  0x8

#endif

#if defined(_MSC_VER)

#if /* MSVC 4.0 */ _MSC_VER >= 1000

#include <intrin.h>

#define __nocl_internal_dlfcn_ReturnAddress  _ReturnAddress

#else

/*
 * On older versions read return address from the value on stack pointer + 4 of
 * the caller. Caller stack pointer is stored in EBP register but only when
 * the EBP register is not optimized out. Usage of _alloca() function prevent
 * EBP register optimization. Read value of EBP + 4 via inline assembly. And
 * because inline assembly does not have a return value, put it into naked
 * function which does not have prologue and epilogue and preserve registers.
 */
__declspec(naked) static void *__nocl_internal_dlfcn_ReturnAddress(void) {
    __asm mov eax, [ebp+4] __asm ret
}

#define __nocl_internal_dlfcn_ReturnAddress()  (_alloca(1), __nocl_internal_dlfcn_ReturnAddress())

#endif

#else

/* https://gcc.gnu.org/onlinedocs/gcc/Return-Address.html */
#define __nocl_internal_dlfcn_ReturnAddress()  __builtin_extract_return_addr(__builtin_return_address(0))

#endif

/*
 * Note:
 * MSDN says these functions are not thread-safe. We make no efforts to have
 * any kind of thread safety.
 */

/*
 * POSIX says dlerror() doesn't have to be thread-safe, so we use one
 * shared buffer.
 *
 * MSDN says the buffer cannot be larger than 128K bytes, so we set it to
 * the limit.
 */
wchar_t __nocl_internal_dlfcn_error_buffer[65535];
BOOL __nocl_internal_dlfcn_error_occurred;

__inline void __cdecl __nocl_internal_dlfcn_wsave_error(const wchar_t *str, DWORD dwMessageId) {

    /*
     * Format error message to:
     * "<argument to function that failed>": <Windows localized error message>
     */
    size_t pos = 0;
    __nocl_internal_dlfcn_error_buffer[pos ++] = L'"';
    size_t i;
    for (i = 0; i < (sizeof(__nocl_internal_dlfcn_error_buffer) / sizeof(wchar_t)) - 5 && str[i] != L'\0'; i ++) {
        __nocl_internal_dlfcn_error_buffer[pos ++] = str[i];
    }
    __nocl_internal_dlfcn_error_buffer[pos ++] = L'"';
    __nocl_internal_dlfcn_error_buffer[pos ++] = L':';
    __nocl_internal_dlfcn_error_buffer[pos ++] = L' ';

    DWORD retval = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwMessageId,
                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               __nocl_internal_dlfcn_error_buffer + pos,
                               (DWORD) ((sizeof(__nocl_internal_dlfcn_error_buffer) / sizeof(wchar_t)) - pos), NULL);
    pos += retval;

    /*
     * When FormatMessageW() fails it returns zero and does not touch buffer
     * so add trailing null byte
     */
    if (retval == 0) __nocl_internal_dlfcn_error_buffer[pos] = L'\0';

    if (pos > 1) {
        /* POSIX says the string must not have trailing <newline> */
        if (__nocl_internal_dlfcn_error_buffer[pos - 2] == L'\r' &&
            __nocl_internal_dlfcn_error_buffer[pos - 1] == L'\n')
            __nocl_internal_dlfcn_error_buffer[pos - 2] = L'\0';
    }

    __nocl_internal_dlfcn_error_occurred = TRUE;
}

__inline void __cdecl __nocl_internal_dlfcn_save_error(const char *str, DWORD dwMessageId) {

    static char lpBuffer[65535];

    /*
     * Format error message to:
     * "<argument to function that failed>": <Windows localized error message>
     */
    size_t pos = 0;
    lpBuffer[pos ++] = '"';
    size_t i;
    for (i = 0; i < sizeof(lpBuffer) - 5 && str[i] != '\0'; i ++) {
        lpBuffer[pos ++] = str[i];
    }
    lpBuffer[pos ++] = '"';
    lpBuffer[pos ++] = ':';
    lpBuffer[pos ++] = ' ';

    DWORD retval = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwMessageId,
                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               lpBuffer + pos,
                               (DWORD) (sizeof(lpBuffer) - pos), NULL);
    pos += retval;

    /*
     * When FormatMessageA() fails it returns zero and does not touch buffer
     * so add trailing null byte
     */
    if (retval == 0) lpBuffer[pos] = '\0';

    if (pos > 1) {
        /* POSIX says the string must not have trailing <newline> */
        if (lpBuffer[pos - 2] == '\r' &&
            lpBuffer[pos - 1] == '\n')
            lpBuffer[pos - 2] = '\0';
    }

    int n = MultiByteToWideChar(CP_ACP, 0, lpBuffer, -1, NULL, 0);
    if (!n) __nocl_internal_dlfcn_wsave_error(L"(null)", GetLastError());
    n = MultiByteToWideChar(CP_ACP, 0, lpBuffer, -1,
        __nocl_internal_dlfcn_error_buffer, n);
    if (!n) __nocl_internal_dlfcn_wsave_error(L"(null)", GetLastError());

    __nocl_internal_dlfcn_error_occurred = TRUE;
}

__inline void __cdecl __nocl_internal_dlfcn_save_handle_error(const void *ptr, DWORD dwMessageId) {

    wchar_t ptr_buf[2 + 2 * sizeof(ptr) + 1];

    ptr_buf[0] = L'0';
    ptr_buf[1] = L'x';

    size_t i;
    for (i = 0; i < 2 * sizeof(ptr); i ++) {
        wchar_t num = (wchar_t) ((((ULONG) ptr) >> (8 * sizeof(ptr) - 4 * (i + 1))) & 0xF);
        ptr_buf[2 + i] = (wchar_t) (num + ((num < 0xA) ? L'0' : (L'A' - 0xA)));
    }

    ptr_buf[2 + 2 * sizeof(ptr)] = L'\0';

    __nocl_internal_dlfcn_wsave_error(ptr_buf, dwMessageId);
}
    
__inline UINT __stdcall __nocl_internal_dlfcn_SetErrorMode(UINT uMode) {

    static BOOL (WINAPI *pSetThreadErrorMode) (DWORD, DWORD *) = NULL;

    static BOOL failed = FALSE;

    if (!failed && !pSetThreadErrorMode) {
        HMODULE hKernel32 = GetModuleHandleA("Kernel32.dll");
        if (hKernel32) pSetThreadErrorMode = (BOOL (WINAPI *) (DWORD, DWORD *)) (LPVOID) GetProcAddress(hKernel32, "SetThreadErrorMode");
        if (!pSetThreadErrorMode) failed = TRUE;
    }

    if (!failed) {
        DWORD dwErrorMode;
        if (!pSetThreadErrorMode(uMode, &dwErrorMode)) return 0;
        return dwErrorMode;
    }

    return SetErrorMode(uMode);
}

__inline HMODULE __stdcall __nocl_internal_dlfcn_GetModuleHandleFromAddress(const void *addr) {

    static BOOL (WINAPI *pGetModuleHandleExA) (DWORD, LPCSTR, HMODULE *) = NULL;
    static BOOL failed = FALSE;

    if (!failed && !pGetModuleHandleExA) {
        HMODULE hKernel32 = GetModuleHandleA("Kernel32.dll");
        if (hKernel32) pGetModuleHandleExA = (BOOL (WINAPI *) (DWORD, LPCSTR, HMODULE *)) (LPVOID) GetProcAddress(hKernel32, "GetModuleHandleExA");
        if (!pGetModuleHandleExA) failed = TRUE;
    }

    HMODULE hModule;

    if (!failed) {
        /* If GetModuleHandleExA is available use it with GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS */
        if (!pGetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (const char *) addr, &hModule))
            return NULL;
    }
    else {
        /*
         * To get HMODULE from address use undocumented hack from https://stackoverflow.com/a/2396380
         * The HMODULE of a DLL is the same value as the module's base address.
         */
        MEMORY_BASIC_INFORMATION info;
        size_t len = VirtualQuery(addr, &info, sizeof(info));
        if (len != sizeof(info)) return NULL;
        hModule = (HMODULE) info.AllocationBase;
    }

    return hModule;
}

/* Load Psapi.dll at runtime, this avoids linking caveat */
__inline BOOL __stdcall __nocl_internal_dlfcn_EnumProcessModules(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded) {

    static BOOL (WINAPI *pEnumProcessModules) (HANDLE, HMODULE *, DWORD, LPDWORD) = NULL;
    static BOOL failed = FALSE;

    if (failed) return FALSE;

    if (!pEnumProcessModules) {
        /* Windows 7 and newer versions have K32EnumProcessModules in Kernel32.dll which is always pre-loaded */
        HMODULE psapi = GetModuleHandleA("Kernel32.dll");
        if (psapi) pEnumProcessModules = (BOOL (WINAPI *) (HANDLE, HMODULE *, DWORD, LPDWORD)) (LPVOID) GetProcAddress(psapi, "K32EnumProcessModules");

        /* Windows Vista and older version have EnumProcessModules in Psapi.dll which needs to be loaded */
        if (!pEnumProcessModules) {
            /* Do not let Windows display the critical-error-handler message box */
            UINT uMode = __nocl_internal_dlfcn_SetErrorMode(SEM_FAILCRITICALERRORS);
            psapi = LoadLibraryA("Psapi.dll");
            if (psapi) {
                pEnumProcessModules = (BOOL (WINAPI *) (HANDLE, HMODULE *, DWORD, LPDWORD)) (LPVOID) GetProcAddress(psapi, "EnumProcessModules");
                if (!pEnumProcessModules) FreeLibrary(psapi);
            }
            __nocl_internal_dlfcn_SetErrorMode(uMode);
        }

        if (!pEnumProcessModules) {
            failed = TRUE;
            return FALSE;
        }
    }

    return pEnumProcessModules(hProcess, lphModule, cb, lpcbNeeded);
}

/* Open a symbol table handle. */
__inline void *__cdecl _wdlopen(const wchar_t *file, int mode) {

    (void) mode;

    HMODULE hModule;

    /* Do not let Windows display the critical-error-handler message box */
    UINT uMode = __nocl_internal_dlfcn_SetErrorMode(SEM_FAILCRITICALERRORS);

    if (!file) {
        /*
         * POSIX says that if the value of file is NULL, a handle on a global
         * symbol object must be provided. That object must be able to access
         * all symbols from the original program file, and any objects loaded
         * with the RTLD_GLOBAL flag.
         * The return value from GetModuleHandle() allows us to retrieve
         * symbols only from the original program file. EnumProcessModules() is
         * used to access symbols from other libraries. For objects loaded
         * with the RTLD_LOCAL flag, we create our own list later on. They are
         * excluded from EnumProcessModules() iteration.
         */
        hModule = GetModuleHandle(NULL);

        if (!hModule) __nocl_internal_dlfcn_wsave_error(L"(null)", GetLastError());
    }
    else {
        wchar_t lpFileName[MAX_PATH];
        size_t len = wcslen(file);

        if (len >= (sizeof(lpFileName) / sizeof(wchar_t))) {
            SetLastError(ERROR_FILENAME_EXCED_RANGE);
            __nocl_internal_dlfcn_wsave_error(file, ERROR_FILENAME_EXCED_RANGE);
            hModule = NULL;
        }
        else {
            /* MSDN says backslashes *must* be used instead of forward slashes. */
            size_t i;
            for (i = 0; i < len; i ++) {
                if (file[i] == L'/') lpFileName[i] = L'\\';
                else lpFileName[i] = file[i];
            }
            lpFileName[len] = L'\0';

            /*
             * POSIX says the search path is implementation-defined.
             * LOAD_WITH_ALTERED_SEARCH_PATH is used to make it behave more closely
             * to UNIX's search paths (start with system folders instead of current
             * folder).
             */
            hModule = LoadLibraryExW(lpFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

            if (!hModule) __nocl_internal_dlfcn_wsave_error(lpFileName, GetLastError());
        }
    }

    /* Return to previous state of the error-mode bit flags. */
    __nocl_internal_dlfcn_SetErrorMode(uMode);

    return hModule;
}

/* Open a symbol table handle. */
__inline void *__cdecl dlopen(const char *file, int mode) {

    wchar_t lpFileName[MAX_PATH];
    size_t len = strlen(file);

    if (len >= (sizeof(lpFileName) / sizeof(wchar_t))) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        __nocl_internal_dlfcn_save_error(file, ERROR_FILENAME_EXCED_RANGE);
        return NULL;
    }

    return _wdlopen(lpFileName, mode);
}

/* Close a symbol table handle. */
__inline int __cdecl dlclose(void *handle) {
    const HMODULE hModule = handle;

    BOOL retval = FreeLibrary(hModule);

    if (!retval) __nocl_internal_dlfcn_save_handle_error(handle, GetLastError());

    /* dlclose's return value in inverted in relation to FreeLibrary's. */
    retval = !retval;

    return retval;
}

noinline void *__cdecl __nocl_internal_dlfcn_dlsym(void *handle, const char *name) {

    FARPROC symbol = NULL;
    HMODULE hCaller = NULL;
    HMODULE hModule = GetModuleHandle(NULL);
    DWORD dwMessageId = 0;

    if (handle == RTLD_DEFAULT) {
        /*
         * The symbol lookup happens in the normal global scope; that is,
         * a search for a symbol using this handle would find the same
         * definition as a direct use of this symbol in the program code.
         * So use same lookup procedure as when filename is NULL.
         */
        handle = hModule;
    }
    else if (handle == RTLD_NEXT) {
        /*
         * Specifies the next object after this one that defines name.
         * This one refers to the object containing the invocation of dlsym().
         * The next object is the one found upon the application of a load
         * order symbol resolution algorithm. To get caller function of dlsym()
         * use _ReturnAddress() intrinsic. To get HMODULE of caller function
         * use MyGetModuleHandleFromAddress() which calls either standard
         * GetModuleHandleExA() function or hack via VirtualQuery().
         */
        hCaller = __nocl_internal_dlfcn_GetModuleHandleFromAddress(__nocl_internal_dlfcn_ReturnAddress());

        if (!hCaller) {
            dwMessageId = ERROR_INVALID_PARAMETER;
            goto end;
        }
    }

    if (handle != RTLD_NEXT) {
        symbol = GetProcAddress(handle, name);

        if (symbol) goto end;
    }

    /*
     * If the handle for the original program file is passed, also search
     * in all globally loaded objects.
     */

    if (hModule == handle || handle == RTLD_NEXT) {

        /*
         * GetModuleHandle(NULL) only returns the current program file. So
         * if we want to get ALL loaded module including those in linked DLLs,
         * we have to use EnumProcessModules().
         */
        HANDLE hCurrentProcess = GetCurrentProcess();
        DWORD dwSize;
        if (__nocl_internal_dlfcn_EnumProcessModules(hCurrentProcess, NULL, 0, &dwSize) != 0) {
            HMODULE *modules = malloc(dwSize);
            if (modules) {
                DWORD cbNeeded;
                if (__nocl_internal_dlfcn_EnumProcessModules(hCurrentProcess, modules, dwSize, &cbNeeded) != 0 && dwSize == cbNeeded) {
                    size_t i;
                    for (i = 0; i < dwSize / sizeof(HMODULE); i ++) {
                        if (handle == RTLD_NEXT && hCaller) {
                            /* Next modules can be used for RTLD_NEXT */
                            if (hCaller == modules[i]) hCaller = NULL;
                            continue;
                        }
                        symbol = GetProcAddress(modules[i], name);
                        if (symbol != NULL) {
                            free(modules);
                            goto end;
                        }
                    }
                }
                free(modules);
            }
            else {
                dwMessageId = ERROR_NOT_ENOUGH_MEMORY;
                goto end;
            }
        }
    }

end:
    if (!symbol) {
        if (!dwMessageId) dwMessageId = ERROR_PROC_NOT_FOUND;
        SetLastError(dwMessageId);
        __nocl_internal_dlfcn_save_error(name, dwMessageId);
    }

    return *(void **) (&symbol);
}

/* Get the address of a symbol from a symbol table handle. */
#define dlsym  __nocl_internal_dlfcn_dlsym

noinline void *__cdecl __nocl_internal_dlfcn_wdlsym(void *handle, const wchar_t *name) {

    if (!name) return __nocl_internal_dlfcn_dlsym(handle, NULL);

    int n = WideCharToMultiByte(CP_ACP, 0, name, -1, NULL, 0, NULL, NULL);
    if (!n) __nocl_internal_dlfcn_wsave_error(name, GetLastError());

    char *str = malloc(n);

    if (!str) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        __nocl_internal_dlfcn_wsave_error(name, ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }

    n = WideCharToMultiByte(CP_ACP, 0, name, -1, str, n, NULL, NULL);
    if (!n) __nocl_internal_dlfcn_wsave_error(name, GetLastError());

    void *symbol = __nocl_internal_dlfcn_dlsym(handle, str);

    free(str);

    return symbol;
}

/* Get the address of a symbol from a symbol table handle. */
#define _wdlsym  __nocl_internal_dlfcn_wdlsym

/* Get diagnostic information. */
__inline char *__cdecl dlerror(void) {

    /* If this is the second consecutive call to dlerror(), return NULL */
    if (!__nocl_internal_dlfcn_error_occurred) return NULL;

    /*
     * POSIX says that invoking dlerror() a second time, immediately following
     * a prior invocation, shall result in NULL being returned.
     */
    __nocl_internal_dlfcn_error_occurred = FALSE;

    return __nocl_internal_dlfcn_error_buffer;
}

/*
 * See https://docs.microsoft.com/en-us/archive/msdn-magazine/2002/march/inside-windows-an-in-depth-look-into-the-win32-portable-executable-file-format-part-2
 * for details
 */

/* Get specific image section */
__inline BOOL __cdecl __nocl_internal_dlfcn_get_image_section(HMODULE module, int index, void **ptr, DWORD *size) {

    IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER *) module;

    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) return FALSE;

    IMAGE_NT_HEADERS *ntHeaders = (IMAGE_NT_HEADERS *) ((BYTE *) dosHeader + dosHeader->e_lfanew);

    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) return FALSE;

    IMAGE_OPTIONAL_HEADER *optionalHeader = &ntHeaders->OptionalHeader;

    if (optionalHeader->Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC) return FALSE;

    if (index < 0 || index >= IMAGE_NUMBEROF_DIRECTORY_ENTRIES || index >= optionalHeader->NumberOfRvaAndSizes)
        return FALSE;

    if (optionalHeader->DataDirectory[index].Size == 0 || optionalHeader->DataDirectory[index].VirtualAddress == 0)
        return FALSE;

    if (size != NULL) *size = optionalHeader->DataDirectory[index].Size;

    *ptr = (void *) ((BYTE *) module + optionalHeader->DataDirectory[index].VirtualAddress);

    return TRUE;
}

/* Return symbol name for a given address from export table */
__inline const char *__cdecl __nocl_internal_dlfcn_get_export_symbol_name(HMODULE module, IMAGE_EXPORT_DIRECTORY *ied, const void *addr, void **func_address) {

    DWORD i;
    void *candidateAddr = NULL;
    signed __int64 candidateIndex = -1;
    BYTE *base = (BYTE *) module;
    DWORD *functionAddressesOffsets = (DWORD *) (base + ied->AddressOfFunctions);
    DWORD *functionNamesOffsets = (DWORD *) (base + ied->AddressOfNames);
    USHORT *functionNameOrdinalsIndexes = (USHORT *) (base + ied->AddressOfNameOrdinals);

    for (i = 0; i < ied->NumberOfFunctions; i ++) {
        if ((void *) (base + functionAddressesOffsets[i] ) > addr || candidateAddr >= (void *) (base + functionAddressesOffsets[i]))
            continue;

        candidateAddr = (void *) (base + functionAddressesOffsets[i]);
        candidateIndex = i;
    }

    if (candidateIndex == -1) return NULL;

    *func_address = candidateAddr;

    for (i = 0; i < ied->NumberOfNames; i ++) {
        if (functionNameOrdinalsIndexes[i] == candidateIndex)
            return (const char *) (base + functionNamesOffsets[i]);
    }

    return NULL;
}

__inline BOOL __cdecl __nocl_internal_dlfcn_is_valid_address(const void *addr) {

    MEMORY_BASIC_INFORMATION info;

    if (!addr) return FALSE;

    /* check valid pointer */
    size_t res = VirtualQuery(addr, &info, sizeof(info));

    if (res == 0 || !info.AllocationBase || info.AllocationProtect == 0 || info.AllocationProtect == PAGE_NOACCESS)
        return FALSE;

    return TRUE;
}

#if defined(_M_ARM64) || defined(__aarch64__)

__inline INT64 __cdecl __nocl_internal_dlfcn_sign_extend(UINT64 value, UINT bits) {

    const UINT left = 64 - bits;
    const INT64 m1 = -1;
    const INT64 wide = (INT64) (value << left);
    const INT64 sign = (wide < 0) ? (m1 << left) : 0;

    return value | sign;
}

#endif

/*
 * Return state if address points to an import thunk
 *
 * On x86, an import thunk is setup with a 'jmp' instruction followed by an
 * absolute address (32bit) or relative offset (64bit) pointing into
 * the import address table (iat), which is partially maintained by
 * the runtime linker.
 *
 * On ARM64, an import thunk is also a relative jump pointing into the
 * import address table, implemented by the following three instructions:
 *
 *      adrp x16, [page_offset]
 * Calculates the page address (aligned to 4KB) the IAT is at, based
 * on the value of x16, with page_offset.
 *
 *      ldr  x16, [x16, offset]
 * Calculates the final IAT address, x16 <- x16 + offset.
 *
 *      br   x16
 * Jump to the address in x16.
 *
 * The register used here is hardcoded to be x16.
 */
__inline BOOL __cdecl __nocl_internal_dlfcn_is_import_thunk(const void *addr) {

#if defined(_M_ARM64) || defined(__aarch64__)

    ULONG opCode1 = * (ULONG *) ((BYTE *) addr);
    ULONG opCode2 = * (ULONG *) ((BYTE *) addr + 4);
    ULONG opCode3 = * (ULONG *) ((BYTE *) addr + 8);

    return (opCode1 & 0x9f00001f) == 0x90000010    /* adrp x16, [page_offset] */
        && (opCode2 & 0xffe003ff) == 0xf9400210    /* ldr  x16, [x16, offset] */
        && opCode3 == 0xd61f0200                   /* br   x16 */
        ? TRUE : FALSE;

#else

    return *(short *) addr == 0x25ff ? TRUE : FALSE;

#endif

}

/*
 * Return address from the import address table (iat),
 * if the original address points to a thunk table entry.
 */
__inline void __cdecl *__nocl_internal_dlfcn_get_address_from_import_address_table(void *iat, DWORD iat_size, const void *addr) {
    BYTE *thkp = (BYTE *) addr;

#if defined(_M_ARM64) || defined(__aarch64__)

    /*
     *  typical import thunk in ARM64:
     *  0x7ff772ae78c0 <+25760>: adrp   x16, 1
     *  0x7ff772ae78c4 <+25764>: ldr    x16, [x16, #0xdc0]
     *  0x7ff772ae78c8 <+25768>: br     x16
     */
    ULONG opCode1 = * (ULONG *) ((BYTE *) addr);
    ULONG opCode2 = * (ULONG *) ((BYTE *) addr + 4);

    /* Extract the offset from adrp instruction */
    UINT64 pageLow2 = (opCode1 >> 29) & 3;
    UINT64 pageHigh19 = (opCode1 >> 5) & ~(~0ull << 19);
    INT64 page = __nocl_internal_dlfcn_sign_extend((pageHigh19 << 2) | pageLow2, 21) << 12;

    /* Extract the offset from ldr instruction */
    UINT64 offset = ((opCode2 >> 10) & ~(~0ull << 12)) << 3;

    /* Calculate the final address */
    BYTE *ptr = ((ULONG64) thkp & ~0xfffull) + page + offset;

#else

    /*
     * Get offset from thunk table (after instruction 0xff 0x25)
     *   4018c8 <_VirtualQuery>: ff 25 4a 8a 00 00
     */
    ULONG offset = *(ULONG *) (thkp + 2);

#if defined(_M_AMD64) || defined(__x86_64__)

    /*
     * On 64 bit the offset is relative
     *      4018c8:   ff 25 4a 8a 00 00    jmpq    *0x8a4a(%rip)    # 40a318 <__imp_VirtualQuery>
     * And can be also negative (MSVC in WDK)
     *   100002f20:   ff 25 3a e1 ff ff    jmpq   *-0x1ec6(%rip)    # 0x100001060
     * So cast to signed LONG type
     */
    BYTE *ptr = thkp + 6 + (LONG) offset;

#else

    /*
     * On 32 bit the offset is absolute
     *   4019b4:    ff 25 90 71 40 00    jmp    *0x40719
     */
    BYTE *ptr = (BYTE *) offset;

#endif

#endif

    if (!__nocl_internal_dlfcn_is_valid_address( ptr ) || ptr < (BYTE *) iat || ptr > (BYTE *) iat + iat_size)
        return NULL;

    return *(void **) ptr;
}

__inline BOOL __cdecl __nocl_internal_dlfcn_fill_info(const void *addr, Dl_info *info) {

    /* Holds module filename */
    static char module_filename[2 * MAX_PATH];

    IMAGE_EXPORT_DIRECTORY *ied;
    void *funcAddress = NULL;

    /* Get module of the specified address */
    HMODULE hModule = __nocl_internal_dlfcn_GetModuleHandleFromAddress(addr);

    if (!hModule) return FALSE;

    DWORD dwSize = GetModuleFileNameA(hModule, module_filename, sizeof(module_filename));

    if (dwSize == 0 || dwSize == sizeof(module_filename)) return FALSE;

    info->dli_fname = module_filename;
    info->dli_fbase = (void *) hModule;

    /* Find function name and function address in module's export table */
    if (__nocl_internal_dlfcn_get_image_section(hModule, IMAGE_DIRECTORY_ENTRY_EXPORT, (void **) &ied, NULL))
        info->dli_sname = __nocl_internal_dlfcn_get_export_symbol_name(hModule, ied, addr, &funcAddress);
    else info->dli_sname = NULL;

    info->dli_saddr = !info->dli_sname ? NULL : funcAddress != NULL ? funcAddress : (void *) addr;

    return TRUE;
}

/* Translate address to symbolic information (not POSIX standard). */
__inline int __cdecl dladdr(const void *addr, Dl_info *info) {
    if (!info) return 0;

    if (!__nocl_internal_dlfcn_is_valid_address(addr)) return 0;

    if (__nocl_internal_dlfcn_is_import_thunk(addr)) {

        /* Get module of the import thunk address */
        HMODULE hModule = __nocl_internal_dlfcn_GetModuleHandleFromAddress(addr);

        if (!hModule) return 0;

        void *iat;
        DWORD iatSize;

        if (!__nocl_internal_dlfcn_get_image_section(hModule, IMAGE_DIRECTORY_ENTRY_IAT, &iat, &iatSize)) {
            /*
             * Fallback for cases where the iat is not defined,
             * for example i586-mingw32msvc-gcc
             */
            IMAGE_IMPORT_DESCRIPTOR *iid;
            DWORD iidSize;

            if (!__nocl_internal_dlfcn_get_image_section( hModule, IMAGE_DIRECTORY_ENTRY_IMPORT, (void **) &iid, &iidSize))
                return 0;

            if (!iid || iid->Characteristics == 0 || iid->FirstThunk == 0)
                return 0;

            iat = (void *) ((BYTE *) hModule + (DWORD) iid->FirstThunk);

            /* We assume that in this case iid and iat's are in linear order */
            iatSize = iidSize - (DWORD) ((BYTE *) iat - (BYTE *) iid);
        }

        addr = __nocl_internal_dlfcn_get_address_from_import_address_table(iat, iatSize, addr);

        if (!__nocl_internal_dlfcn_is_valid_address(addr)) return 0;
    }

    if (!__nocl_internal_dlfcn_fill_info(addr, info)) return 0;

    return 1;
}

#endif

#else

#define NOCL_FEATURE_NO_DL

#endif

#if defined(__cplusplus)

}

#endif

#endif
