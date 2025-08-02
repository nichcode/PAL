
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
 * @file pal_video.h
 * 
 * Header file for video functions, macros, enum and structs
 * 
 * @defgroup video 
 */

#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal_core.h"

/**
 * @struct PalVideoSystem
 * @brief Opaque handle to a video system.
 *
 * @ingroup video
 */
typedef struct PalVideoSystem PalVideoSystem;

/**
 * @struct PalVideoSystemCreateInfo
 * @brief Specifies options for creating an instance of the video system.
 * 
 * This struct must be initialized and passed to palCreateVideoSystem().
 * 
 * All fields must be explicitly set by the user.
 * 
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @ingroup video
 */
typedef struct {
    PalAllocator* allocator;         /** < user allocator or nullptr for default.*/
} PalVideoSystemCreateInfo;

/**
 * @brief Create an instance of the video system.
 * 
 * A video system is neeed to enumerate connected displays (monitors), 
 * get supported video features, get supported window styles 
 * and window creation.
 * 
 * The user must call this function before any other video related functionality,
 * to obtain the video system handle which will be passed in other video related functions.
 *
 * @param[in] info A pointer to a PalVideoSystemCreateInfo struct with creation options.
 * @param[out] outVideo Pointer to the created video system handle.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is not thread-safe. If multiple threads will call this function,
 * then the user is responsible for synchronization.
 * 
 * @note The created video system instance must be destroyed with `PalDestroyVideoSystem()`
 * when no longer needed.
 * 
 * @sa PalVideoSystemCreateInfo, palDestroyVideoSystem()
 * @ingroup video
 */
_PAPI PalResult _PCALL palCreateVideoSystem(
    const PalVideoSystemCreateInfo* info,
    PalVideoSystem** outVideo);

/**
 * @brief Destroy the instance of the video system.
 * 
 * After this call, the pointer will be invalid and should not be used anymore.
 * 
 * This function can be called multiple times without any undefined behavior.
 * If the video system handle is invalid or a nullptr, the function returns silently.
 * 
 * @param[in] video A pointer to the video system handle to destroy.
 *
 * @note This function is not thread-safe. If multiple threads will call this function,
 * then the caller is responsible for synchronization.
 * All resources created through the video system must be destroyed before this call.
 * 
 * @sa palCreateVideoSystem()
 * @ingroup video
 */
_PAPI void _PCALL palDestroyVideoSystem(
    PalVideoSystem* video);

#endif // _PAL_VIDEO_H