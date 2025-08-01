
/**

Copyright (C) 2025 Nicholas Agbo
  
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
  
1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required. 
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

/**
 * @file pal_core.h
 * 
 * Header file for core functions, macros, enum and structs
 * 
 * @defgroup core
 * 
 */

#ifndef _PAL_CORE_H
#define _PAL_CORE_H

#include "pal_defines.h"

/**
 * @typedef PalAllocateFn
 * @brief A type used to identify an allocate function pointer
 * 
 * @param userData The user data
 * @param size The size in bytes of memory to allocate
 * @ingroup core
 */
typedef void* (*PalAllocateFn)(void* userData, Uint64 size);

/**
 * @typedef PalFreeFn
 * @brief A type used to identify a free function pointer
 * 
 * @param userData The user data
 * @param ptr The pointer to the memory block to free
 * @ingroup core
 */
typedef void (*PalFreeFn)(void* userData, void* ptr);

/**
 * @struct PalVersion
 * @brief A type used to identify version
 * 
 * @ingroup core
 */
typedef struct {
   int major;         /** < Major version*/
   int minor;         /** < Minor version */
   int revision;      /** < Revision version */
} PalVersion;

/**
 * @struct PalAllocator
 * @brief A type used to identify an allocator
 * 
 * @ingroup core
 */
typedef struct {
   PalAllocateFn allocate;    /** < Allocate function pointer*/
   PalFreeFn free;            /** < Free function pointer*/
   void* userData;            /** < Data passed in function pointers*/
} PalAllocator;

/**
 * @brief Get the compiled runtime version of the library
 * 
 * @note This function is thread-safe
 * 
 * @sa palGetVersionString()
 * @ingroup core
 * @return A copy of the version 
 */
_PAPI PalVersion _PCALL palGetVersion();

/**
 * @brief Get the compiled runtime version string of the library.
 * 
 * The format of the string will be: `1.2.3`.
 * 
 * Where `1` is the major, `2` is the minor and `3` is the revision versions respectively
 * 
 * @sa palGetVersion()
 * @ingroup core
 * @note This function is thread-safe
 * 
 * @return A copy of the version string
 */
_PAPI const char* _PCALL palGetVersionString();

/**
 * @brief Allocate memory with a custom or default allocator
 * 
 * @param allocator This can be custom(user provided) or PAL_NULL to use default
 * @param size The size in bytes of memory to allocate
 * 
 * @sa palFree()
 * @note This function is thread-safe
 * @ingroup core
 * 
 * @return A memory block or PAL_NULL if failed
 */
_PAPI void* _PCALL palAllocate(
   PalAllocator* allocator, 
   Uint64 size);

/**
 * @brief Free memory with a custom or default allocator
 * 
 * @warning free the memory with the same allocator used for the allocation
 * 
 * @param allocator This can be custom(user provided) or PAL_NULL to use default
 * @param ptr The pointer to the memory block to free
 * 
 * @note This function is thread-safe
 * @ingroup core
 */
_PAPI void _PCALL palFree(
   PalAllocator* allocator, 
   void* ptr);

/**
 * @brief Set a block of memory to a specific byte value
 * 
 * @param ptr The pointer to the memory block to fill
 * @param value The byte value to set(casted to unsigned char)
 * @param size The number or size of bytes to set
 * 
 * @note This function is thread-safe
 * @ingroup core
 */
_PAPI void _PCALL palSetMemory(
   void* ptr, 
   int value, 
   Uint64 size);

/**
 * @brief Set a block of memory to zero
 * 
 * @param ptr The pointer to the memory block to fill
 * @param size The number or size of bytes to set
 * 
 * @note This function is thread-safe
 * @ingroup core
 */
_PAPI void _PCALL palZeroMemory(
   void* ptr, 
   Uint64 size);

/**
 * @brief Copy a block of memory to another block of memory
 * 
 * @param dest The pointer to the memory block to copy to
 * @param src The pointer to the memory block to copy from
 * @param size The number or size of bytes to copy
 * 
 * @note This function is thread-safe
 * @ingroup core
 */
_PAPI void _PCALL palCopyMemory(
   void* dest, 
   const void* src, 
   Uint64 size);

/**
 * @brief Move a block of memory to another block of memory
 * 
 * @param dest The pointer to the memory block to move to
 * @param src The pointer to the memory block to move from
 * @param size The number or size of bytes to move
 * 
 * @note This function is thread-safe
 * @ingroup core
 */
_PAPI void _PCALL palMoveMemory(
   void* dest, 
   const void* src, 
   Uint64 size);

#endif // _PAL_CORE_H