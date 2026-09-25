/**
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef PAL_GRAPHICS_UTILS_H
#define PAL_GRAPHICS_UTILS_H

#include "pal2/core/defines.h"

/**
 * @struct PalWorkGroupBuildData
 * @brief Contains build information about a workgroup.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalWorkGroupBuildData::workCount
 * The workload per dimension.
 * 
 * @var PalWorkGroupBuildData::workGroupSize
 * The maximum workgroup size per dimension of the adapter.
 * 
 * @var PalWorkGroupBuildData::workGroupCount
 * The maximum number of workgroups per dimension of the adapter.
 */
typedef struct PalWorkGroupBuildData
{
    uint32_t workCount[3];
    uint32_t workGroupSize[3];
    uint32_t workGroupCount[3];
} PalWorkGroupBuildData;

/**
 * @struct PalWorkGroupInfo
 * @brief Contains information about a dispatch tile.
 * 
 * @since Added in version 2.0
 * 
 * @var PalWorkGroupInfo::workGroupBase
 * The offsets per dimension of the dispatch tile.
 * 
 * @var PalWorkGroupInfo::workGroupBase
 * The number of workgroups per dimension of the dispatch tile.
 */
typedef struct PalWorkGroupInfo
{
    uint32_t workGroupBase[3];
    uint32_t workGroupCount[3];
} PalWorkGroupInfo;

/**
 * @brief Build work group info(s) from work inputs specified in pixels, vertices etc.
 *
 * Call this function first with PalWorkGroupInfo array set to `nullptr` to get the number of work
 * group infos. Allocate memory for the PalWorkGroupInfo array and passed in the count and the
 * allocated array. If the count of the array is less than the number of work group infos, PAL will
 * write upto that limit.
 *
 * If the count is 0 and the PalWorkGroupInfo array is `nullptr`, the function fails
 * and returns `PAL_FALSE`.
 *
 * This function works the maths for how many work groups to dispatch in each axis and how many
 * times it needs to be dispatch in order for the work to be done. It works well with
 * palCmdDispatchBase() since its also gives the base for each work group.
 *
 * @param[in] data Pointer to a PalWorkGroupBuildData with parameters.
 * @param[in, out] count Capacity of the PalWorkGroupInfo array.
 * @param[out] infos Pointer to an Array of PalWorkGroupInfo.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palCmdDrawMeshTasks
 * @sa palCmdDrawMeshTasksIndirect
 * @sa palCmdDrawMeshTasksIndirectCount
 * @sa palCmdDispatch
 * @sa palCmdDispatchBase
 */
PAL_API void PAL_CALL palBuildWorkGroupInfo(
    const PalWorkGroupBuildData* data,
    uint32_t* count,
    PalWorkGroupInfo* info);

/**
 * @brief Check if a constant is supported in a mask.
 *
 * This function is used to check all masks in `supported_**` format in most of the capabilities
 * query structs.
 *
 * Example:
 *
 * To check if `PAL_PRESENT_MODE_IMMEDIATE` is supported after querying `PalSurfaceCapabilities`
 * capabilities of a surface, PalSurfaceCapabilities::supportedPresentModes should be the `mask`
 * parameter and `PAL_PRESENT_MODE_IMMEDIATE` as the value parameter.
 *
 * @param[in] mask The supported mask.
 * @param[in, out] value The value to check in the supported mask.
 *
 * @return `PAL_TRUE` on success otherwise `PAL_FALSE`.
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 */
static inline PalBool PAL_CALL palIsSupported(
    uint32_t mask,
    uint32_t value)
{
    return (mask & (1U << value)) != 0;
}

#endif // PAL_GRAPHICS_UTILS_H