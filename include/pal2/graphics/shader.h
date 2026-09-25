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

#ifndef PAL_GRAPHICS_SHADER_H
#define PAL_GRAPHICS_SHADER_H

#include "device.h"

#define PAL_SHADER_ENTRY_NAME_SIZE 32

/**
 * @defgroup shader_stages Shader Stages
 * @brief Shader stages
 * 
 * @{
 */
#define PAL_SHADER_STAGE_UNDEFINED 0
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
#define PAL_SHADER_STAGE_COUNT 15
/** @} */

/**
 * @typedef PalShaderStage
 * @brief shader stage types.
 * 
 * All values of this type follow the format `PAL_SHADER_STAGE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalShaderStage;

/**
 * @struct PalShader
 * @brief Opaque handle to a shader.
 *
 * @since Added in version 2.0
 */
typedef struct PalShader PalShader;

/**
 * @struct PalShaderEntryInfo
 * @brief Contains information about a shader entry.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalShaderEntryInfo::entryName
 * The shader entry name.
 * 
 * @var PalShaderEntryInfo::stage
 * The shader stage of the entry (eg. `PAL_SHADER_STAGE_VERTEX`).
 * 
 * @var PalShaderEntryInfo::patchControlPoints
 * The tessellation patch point. This will be used for tessellation shaders.
 */
typedef struct PalShaderEntryInfo
{
    const char* entryName;
    PalShaderStage stage;
    uint32_t patchControlPoints;
} PalShaderEntryInfo;

/**
 * @struct PalShaderCreateInfo
 * @brief Contains creation parameters of a swapchain.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalShaderCreateInfo::code
 * The shader source or bytecode.
 * 
 * @var PalShaderCreateInfo::entries
 * The shader entries.
 * 
 * @var PalShaderCreateInfo::codeSize
 * The size of the shader source or bytecode.
 * 
 * @var PalShaderCreateInfo::entryCount
 * The number of shader entries.
 */
typedef struct PalShaderCreateInfo
{
    void* code;
    PalShaderEntryInfo* entries;
    uint32_t codeSize;
    uint32_t entryCount;
} PalShaderCreateInfo;

/**
 * @brief Create a shader.
 *
 * The created shader must be destroyed using `palDestroyShader()`.
 *
 * `PAL_ADAPTER_FEATURE_GEOMETRY_SHADER` must be supported and enabled by the device if
 * `PAL_SHADER_STAGE_GEOMETRY` will be used.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` must be supported and enabled by the device if
 * `PAL_SHADER_STAGE_MESH` or `PAL_SHADER_STAGE_TASK` will be used.
 *
 * `PAL_ADAPTER_FEATURE_TESSELLATION_SHADER` must be supported and enabled by the device if
 * `PAL_SHADER_STAGE_TESSELLATION_CONTROL` or `PAL_SHADER_STAGE_TESSELLATION_EVALUATION` will
 * be used.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if
 * `PAL_SHADER_STAGE_RAYGEN` or `PAL_SHADER_STAGE_CLOSEST_HIT` or `PAL_SHADER_STAGE_ANY_HIT` or
 * `PAL_SHADER_STAGE_MISS` or `PAL_SHADER_STAGE_INTERSECTION` or `PAL_SHADER_STAGE_CALLABLE` will
 * be used.
 *
 * @param[in] device Device that creates the shader.
 * @param[in] info Pointer to a PalShaderCreateInfo struct that specifies parameters.
 * @param[out] outShader Pointer to a PalShader to recieve the created shader.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @note The shader entry name must not be greater than `PAL_SHADER_ENTRY_NAME_SIZE (32)`.
 *
 * @since Added in version 2.0
 * @sa palDestroyShader
 */
PAL_API PalResult PAL_CALL palCreateShader(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

/**
 * @brief Destroy a shader.
 *
 * @param[in] shader Shader to destroy.
 *
 * Thread safety: Thread safe if the device used to create the shader is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateShader
 */
PAL_API void PAL_CALL palDestroyShader(PalShader* shader);

#endif // PAL_GRAPHICS_SHADER_H