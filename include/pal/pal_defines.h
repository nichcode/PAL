
#ifndef _PAL_DEFINES_H
#define _PAL_DEFINES_H

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

#define PAL_MAKE_VERSION(major, minor, patch) (((major) << 22) | ((minor) << 12) | (patch))
#define PAL_GET_VERSION_MAJOR(versionInt) ((versionInt) >> 22)
#define PAL_GET_VERSION_MINOR(versionInt) (((versionInt) >> 12) & 0x3FF)
#define PAL_GET_VERSION_PATCH(versionInt) ((versionInt) & 0xFFF)

#define PAL_VERSION_1_0 PAL_MAKE_VERSION(1, 0, 0)

#define PAL_DEFINE_HANDLE(name) typedef struct name name;

typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;
typedef unsigned long long Uint64;
typedef long long Int64;

// this can use like a bool
typedef enum PalResult
{
    PAL_RESULT_FAIL,
    PAL_RESULT_OK
} PalResult;

#endif // _PAL_DEFINES_H