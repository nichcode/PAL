
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
 * @brief An opaque type used to identify a video system
 *
 * @ingroup video
 */
typedef struct PalVideoSystem PalVideoSystem;

/**
 * @struct PalVideoSystemCreateInfo
 * @brief A type used to identify a video system create info
 *
 * @ingroup video
 */
typedef struct {
    PalAllocator* allocator;         /** < custom allocator or null for default*/
} PalVideoSystemCreateInfo;

/**
 * @brief Create an instance of the video system
 * 
 * A video system is neeed to query displays, query supported video features,
 * query supported window stylesm enumerate displays and other platform-specific video capabilities
 * 
 * The caller must call this function before any other video related functionality.
 *
 * @param[in] info A pointer to a PalVideoSystemCreateInfo struct with creation parameters
 * @param[out] outVideoSystem A pointer to a PalVideoSystem* 
 * that will receive the created video system instance
 * 
 * @return PAL_SUCESS on success or a result code on failure
 *
 * @note This function is not thread-safe. If multiple threads will call this function,
 * then the caller is responsible for synchronization
 * 
 * @note The created PalVideoSystem must be destroyed using `PalDestroyVideoSystem()`
 * when no longer needed
 * 
 * @sa PalVideoSystemCreateInfo
 * @sa palDestroyVideoSystem()
 * @ingroup video
 */
_PAPI PalResult _PCALL palCreateVideoSystem(
    const PalVideoSystemCreateInfo* info,
    PalVideoSystem** outVideoSystem);

/**
 * @brief Destroy an instance of the video system
 * 
 * After this call, the pointer will be invalid and should not be used anymore
 * 
 * If the pointer is invalid, the function returns silently without doing anything
 * 
 * @param[in] videoSystem A pointer to the PalVideoSystem to destroy. 
 *
 * @note This function is not thread-safe. If multiple threads will call this function,
 * then the caller is responsible for synchronization.
 * 
 * @note All resources created through the video system should be destroyed before this function.
 * 
 * @sa palCreateVideoSystem()
 * @ingroup video
 */
_PAPI void _PCALL palDestroyVideoSystem(
    PalVideoSystem* videoSystem);

#endif // _PAL_VIDEO_H