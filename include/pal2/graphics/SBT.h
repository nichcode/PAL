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

#ifndef PAL_GRAPHICS_SBT_H
#define PAL_GRAPHICS_SBT_H

#include "pipeline.h"

/**
 * @struct PalShaderBindingTable
 * @brief Opaque handle to a shader binding table.
 *
 * @since Added in version 2.0
 */
typedef struct PalShaderBindingTable PalShaderBindingTable;

/**
 * @struct PalShaderBindingTableRecordInfo
 * @brief Contains information about a shader binding record.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * The records array must be in this order [raygen][miss][hitgroup][callable].
 *
 * @since Added in version 2.0
 * 
 * @var PalShaderBindingTableRecordInfo::localData
 * The local data of the record. This will be ignored of ::localDataSize is `0`
 * 
 * @var PalShaderBindingTableRecordInfo::groupIndex
 * The index into the shader groups used to create the ray tracing pipeline.
 * 
 * @var PalShaderBindingTableRecordInfo::localDataSize
 * The local data size.
 */
typedef struct PalShaderBindingTableRecordInfo
{
    void* localData;
    uint32_t groupIndex;
    uint32_t localDataSize;
} PalShaderBindingTableRecordInfo;

/**
 * @struct PalShaderBindingTableCreateInfo
 * @brief Contains creation parameters of a shader binding table.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalShaderBindingTableCreateInfo::records
 * The shader binding table records.
 * 
 * @var PalShaderBindingTableCreateInfo::rayTracingPipeline
 * The ray tracing pipeline.
 * 
 * @var PalShaderBindingTableCreateInfo::recordCount
 * The number of shader binding table records.
 * 
 * @var PalShaderBindingTableCreateInfo::reserved
 * Not used. Set to `0`.
 */
typedef struct PalShaderBindingTableCreateInfo
{
    PalShaderBindingTableRecordInfo* records;
    PalPipeline* rayTracingPipeline;
    uint32_t recordCount;
    uint32_t reserved;
} PalShaderBindingTableCreateInfo;

/**
 * @brief Create a shader binding table.
 *
 * The created shader binding table must be destroyed using `palDestroyShaderBindingTable()`.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * PalShaderBindingTableCreateInfo::recordCount must match the shader group count of
 * PalShaderBindingTableCreateInfo::rayTracingPipeline.
 *
 * @param[in] device Device that creates the shader binding table.
 * @param[in] info Pointer to a PalShaderBindingTableCreateInfo struct that specifies parameters.
 * @param[out] outSbt Pointer to a PalShaderBindingTable to recieve the created shader binding
 * table.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @note The records array must be in this order [raygen][miss][hitgroup][callable].
 *
 * @since Added in version 2.0
 * @sa palDestroyShaderBindingTable
 */
PAL_API PalResult PAL_CALL palCreateShaderBindingTable(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt);

/**
 * @brief Destroy a shader binding table.
 *
 * @param[in] sbt Shader binding table to destroy.
 *
 * Thread safety: Thread safe if the device used to create the shader binding table is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateShaderBindingTable
 */
PAL_API void PAL_CALL palDestroyShaderBindingTable(PalShaderBindingTable* sbt);

/**
 * @brief Update a shader binding table record payloads.
 *
 * This call does not update shader handles. It only updates the payload associated
 * with the record. PalShaderBindingTableRecordInfo::groupIndex is the index into
 * the shader groups used to create the ray tracing pipeline.
 *
 * @param[in] sbt The shader binding table to update.
 * @param[in] count Capacity of the PalShaderBindingTableRecordInfo array.
 * @param[in] infos Array of PalShaderBindingTableRecordInfo to update.
 *
 * Thread safety: Thread safe if `sbt` is externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palUpdateShaderBindingTable(
    PalShaderBindingTable* sbt,
    uint32_t count,
    PalShaderBindingTableRecordInfo* infos);

#endif // PAL_GRAPHICS_SBT_H