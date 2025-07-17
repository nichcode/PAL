
#ifndef _PAL_CORE_H
#define _PAL_CORE_H

// using from Cpp
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef _WIN32
#define _PCALL __stdcall
#ifdef PAL_EXPORT
#define _PAPI __declspec(dllexport)
#else 
#define _PAPI __declspec(dllimport)
#endif // PAL_EXPORT
#else
    // other platforms
#define _PCALL
#ifdef PAL_EXPORT
#define _PAPI extern "C" __attribute__((visibility("default")))
#else 
#define _PAPI
#endif // PAL_EXPORT
#endif // _WIN32

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus
#define PAL_NULL nullptr
#define PAL_TRUE true
#define PAL_FALSE false
#else
#define PAL_NULL ((void *)0)
#define PAL_TRUE 1
#define PAL_FALSE 0
 typedef _Bool bool;
#endif // __cplusplus

#define PAL_BIT(x) 1 << x
#define PAL_DECLARE_HANDLE(name) typedef struct name name;

typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;
typedef unsigned long long Uint64;
typedef long long Int64;

typedef enum PalResult {
    PAL_SUCCESS,
    PAL_ERROR_NULL_POINTER,
    PAL_ERROR_INVALID_ARGUMENT,
    PAL_ERROR_INVALID_ALLOCATOR,
    PAL_ERROR_OUT_OF_MEMORY,
    PAL_ERROR_UNSUPPORTED,
    PAL_ERROR_RESOURCE_IN_USE,
    PAL_ERROR_TIMEOUT
} PalResult;

typedef struct PalVersion {
    int major;
    int minor;
    int patch;
} PalVersion;

_PAPI PalVersion _PCALL palGetVersion();
_PAPI const char* _PCALL palGetVersionString();

_PAPI const char* _PCALL palResultToString(PalResult result);

#endif // _PAL_CORE_H