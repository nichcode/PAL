/**
 * @file next.h
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

#ifndef PAL_GRAPHICS_SHADER_H
#define PAL_GRAPHICS_SHADER_H

#include "types.h"

/**
 * @brief Maximum size of a shader entry name, including the NULL terminator.
 * @ingroup pal_graphics
 */
#define PAL_SHADER_ENTRY_NAME_SIZE 32

/**
 * @defgroup shader_stages Shader Stages
 * @ingroup pal_graphics
 * @{
 */
#define PAL_SHADER_STAGE_UNDEFINED 0 /**< Invalid shader stage. */
#define PAL_SHADER_STAGE_VERTEX 1
#define PAL_SHADER_STAGE_FRAGMENT 2
#define PAL_SHADER_STAGE_COMPUTE 3
#define PAL_SHADER_STAGE_GEOMETRY 4
#define PAL_SHADER_STAGE_MESH 5
#define PAL_SHADER_STAGE_TASK 6
#define PAL_SHADER_STAGE_TESSELLATION_CONTROL 7
#define PAL_SHADER_STAGE_TESSELLATION_EVALUATION 8
#define PAL_SHADER_STAGE_RAYGEN 9
#define PAL_SHADER_STAGE_CLOSEST_HIT 10
#define PAL_SHADER_STAGE_ANY_HIT 11
#define PAL_SHADER_STAGE_MISS 12
#define PAL_SHADER_STAGE_INTERSECTION 13
#define PAL_SHADER_STAGE_CALLABLE 14
#define PAL_SHADER_STAGE_COUNT 15 /**< Number of shader stages. */
/** @} */

/**
 * @struct PalShaderEntryInfo
 * @brief Shader entry information.
 *
 * `entryName` must not be greater than `PAL_SHADER_ENTRY_NAME_SIZE`.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalShaderEntryInfo {
    const char* entryName;       /**< Shader entry name (NULL-terminated). */
    PalShaderStage stage;        /**< Shader entry stage. */
    uint32_t patchControlPoints; /**< Patch control points for tessellation shaders. */
} PalShaderEntryInfo;

/**
 * @struct PalShaderCreateInfo
 * @brief Shader creation parameters.
 *
 * This struct is used only during @ref palCreateShader() and may be
 * discarded after the function returns.
 *
 * The following shader stages requires:
 * - PAL_SHADER_STAGE_GEOMETRY - @ref PAL_ADAPTER_FEATURE_GEOMETRY_SHADER feature to be supported
 * - PAL_SHADER_STAGE_MESH - @ref PAL_ADAPTER_FEATURE_MESH_SHADER feature to be supported
 * - PAL_SHADER_STAGE_TASK - @ref PAL_ADAPTER_FEATURE_MESH_SHADER feature to be supported
 * - PAL_SHADER_STAGE_TESSELLATION_CONTROL - @ref PAL_ADAPTER_FEATURE_TESSELLATION_SHADER feature to be supported
 * - PAL_SHADER_STAGE_TESSELLATION_EVALUATION - @ref PAL_ADAPTER_FEATURE_TESSELLATION_SHADER feature to be supported
 * - PAL_SHADER_STAGE_RAYGEN - @ref PAL_ADAPTER_FEATURE_RAY_TRACING feature to be supported
 * - PAL_SHADER_STAGE_CLOSEST_HIT - @ref PAL_ADAPTER_FEATURE_RAY_TRACING feature to be supported
 * - PAL_SHADER_STAGE_ANY_HIT - @ref PAL_ADAPTER_FEATURE_RAY_TRACING feature to be supported
 * - PAL_SHADER_STAGE_MISS - @ref PAL_ADAPTER_FEATURE_RAY_TRACING feature to be supported
 * - PAL_SHADER_STAGE_INTERSECTION - @ref PAL_ADAPTER_FEATURE_RAY_TRACING feature to be supported
 * - PAL_SHADER_STAGE_CALLABLE - @ref PAL_ADAPTER_FEATURE_RAY_TRACING feature to be supported
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalShaderCreateInfo {
    void* code;                  /**< Shader code or bytecode. */
    PalShaderEntryInfo* entries; /**< Shader entries. */
    uint32_t codeSize;           /**< Size of shader code or bytecode in bytes. */
    uint32_t entryCount;         /**< Number of shader entries. */
} PalShaderCreateInfo;

/**
 * @brief Creates a shader.
 *
 * The created shader must be destroyed using @ref palDestroyShader().
 *
 * This function creates a shader using the specified creation parameters.
 * `info` must remain valid for the duration of this function. PAL does not
 * copy its contents.
 *
 * @param[in] device Device that creates the shader.
 * @param[in] info Shader creation parameters.
 * @param[out] outShader Output handle to receive the created shader.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palDestroyShader
 */
PAL_API PalResult PAL_CALL palCreateShader(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

/**
 * @brief Destroys a shader.
 *
 * @param[in] shader Shader to destroy.
 *
 * @thread-safety The device used to create `shader` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateShader
 */
PAL_API void PAL_CALL palDestroyShader(PalShader* shader);

#endif // PAL_GRAPHICS_SHADER_H