/**
 * @file sbt.h
 * 
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

#include "types.h"

/**
 * @struct PalShaderBindingTableRecordInfo
 * @brief Shader binding table record information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalShaderBindingTableRecordInfo {
    void* localData;        /**< Local data, or `nullptr` if `localDataSize` is `0`. */
    uint32_t groupIndex;    /**< Index into the shader groups array used to create the ray tracing pipeline. */
    uint32_t localDataSize; /**< Size of `localData` in bytes, or `0`. */
} PalShaderBindingTableRecordInfo;

/**
 * @struct PalShaderBindingTableCreateInfo
 * @brief Shader binding table creation parameters.
 *
 * This struct is used only during @ref palCreateShaderBindingTable() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalShaderBindingTableCreateInfo {
    PalShaderBindingTableRecordInfo* records; /**< Shader binding table records. */
    PalPipeline* rayTracingPipeline;          /**< Ray tracing pipeline. */
    uint32_t recordCount;                     /**< Number of shader binding table records. */
    uint32_t reserved;                        /**< Must be set to `0`. */
} PalShaderBindingTableCreateInfo;

/**
 * @brief Creates a shader binding table.
 *
 * The created shader binding table must be destroyed using @ref palDestroyShaderBindingTable().
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * PalShaderBindingTableCreateInfo::recordCount must match the shader group count of
 * PalShaderBindingTableCreateInfo::rayTracingPipeline.
 *
 * @param[in] device Device that creates the shader binding table.
 * @param[in] info Pointer to a PalShaderBindingTableCreateInfo struct that specifies parameters.
 * @param[out] outSbt Pointer to a PalShaderBindingTable to receive the created shader binding
 * table.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palDestroyShaderBindingTable
 */
PAL_API PalResult PAL_CALL palCreateShaderBindingTable(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt);

/**
 * @brief Destroys a shader binding table.
 *
 * @param[in] sbt Shader binding table to destroy.
 *
 * @thread-safety The device used to create `sbt` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateShaderBindingTable
 */
PAL_API void PAL_CALL palDestroyShaderBindingTable(PalShaderBindingTable* sbt);

/**
 * @brief Updates a shader binding table record payloads.
 *
 * This call does not update shader handles. It only updates the payload associated
 * with the record. PalShaderBindingTableRecordInfo::groupIndex is the index into
 * the shader groups used to create the ray tracing pipeline.
 *
 * @param[in] sbt The shader binding table to update.
 * @param[in] count Capacity of the PalShaderBindingTableRecordInfo array.
 * @param[in] infos Array of PalShaderBindingTableRecordInfo to update.
 *
 * @thread-safety `sbt` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palUpdateShaderBindingTable(
    PalShaderBindingTable* sbt,
    uint32_t count,
    PalShaderBindingTableRecordInfo* infos);

#endif // PAL_GRAPHICS_SBT_H