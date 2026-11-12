/**
 * @file acceleration_structure.h
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

#ifndef PAL_GRAPHICS_ACCELERATION_STRUCTURE_H
#define PAL_GRAPHICS_ACCELERATION_STRUCTURE_H

#include "pal2/core/base.h"
#include "pal2/core/result.h"
#include "device.h"
#include "buffer.h"
#include "vertex.h"

/**
 * @typedef PalAccelerationStructureType
 * @brief Acceleration structure type.
 *
 * Identifies the level of an acceleration structure.
 *
 * All values of this type follow the format `PAL_ACCELERATION_STRUCTURE_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalAccelerationStructureType;

/**
 * @typedef PalAccelerationStructureBuildMode
 * @brief Acceleration structure build mode.
 *
 * Identifies whether an acceleration structure is built or updated.
 *
 * All values of this type follow the format `PAL_ACCELERATION_STRUCTURE_BUILD_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalAccelerationStructureBuildMode;

/**
 * @typedef PalAccelerationStructureBuildHints
 * @brief Acceleration structure build hints.
 *
 * Identifies the optimization goals of an acceleration structure build.
 *
 * Multiple hints can be OR'ed together using bitwise OR operator (`|`).
 * Hints can be ignored by the driver.
 *
 * All values of this type follow the format `PAL_ACCELERATION_STRUCTURE_BUILD_HINT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalAccelerationStructureBuildHints;

/**
 * @typedef PalAccelerationStructureInstanceFlags
 * @brief Acceleration structure instance flags.
 *
 * Identifies the behavior of an acceleration structure instance.
 *
 * Multiple flags can be OR'ed together using bitwise OR operator (`|`).
 *
 * All values of this type follow the format `PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalAccelerationStructureInstanceFlags;

/**
 * @typedef PalGeometryType
 * @brief Acceleration structure geometry type.
 *
 * Identifies the kind of acceleration structure geometry.
 *
 * All values of this type follow the format `PAL_GEOMETRY_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalGeometryType;

/**
 * @typedef PalGeometryFlags
 * @brief Acceleration structure geometry flags.
 *
 * Identifies the behavior of acceleration structure geometry.
 *
 * Multiple flags can be OR'ed together using bitwise OR operator (`|`).
 * Not all combinations are valid.
 *
 * All values of this type follow the format `PAL_GEOMETRY_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalGeometryFlags;

/**
 * @struct PalAccelerationStructure
 * @brief Opaque handle to an acceleration structure.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAccelerationStructure PalAccelerationStructure;

/**
 * @defgroup as_types Acceleration Structure Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL 0 /**< Top-level acceleration structure (TLAS). */
#define PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL 1 /**< Bottom-level acceleration structure (BLAS). */
#define PAL_ACCELERATION_STRUCTURE_TYPE_COUNT 2 /**< Number of acceleration structure types. */
/** @} */

/**
 * @defgroup as_build_modes Acceleration Structure Build Modes
 * @ingroup pal_graphics
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD 0 /**< Build the acceleration structure from scratch. */
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_UPDATE 1 /**< Update an existing acceleration structure. */
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_COUNT 2 /**< Number of acceleration structure build modes. */
/** @} */

/**
 * @defgroup as_build_hints Acceleration Structure Build Hints
 * @ingroup pal_graphics
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD (1U << 0) /**< Prefer faster build times. */
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_TRACE (1U << 1) /**< Prefer faster ray traversal. */
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_LOW_MEMORY (1U << 2) /**< Prefer a smaller memory footprint. */
/** @} */

/**
 * @defgroup as_instance_flags Acceleration Structure Instance Flags
 * @ingroup pal_graphics
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE (1U << 0) /**< Force opaque geometry. */
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NO_OPAQUE (1U << 1) /**< Force non-opaque geometry. */
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FACING_CULL_DISABLE (1U << 2) /**< Disable facing cull. */
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE (1U << 3) /**< CCW front face. */
/** @} */

/**
 * @defgroup as_geometry_types Acceleration Structure Geometry Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_GEOMETRY_TYPE_TRIANGLE 0 /**< Triangle geometry. */
#define PAL_GEOMETRY_TYPE_AABBS 1 /**< Axis-aligned bounding box geometry. */
#define PAL_GEOMETRY_TYPE_COUNT 2 /**< Number of acceleration structure geometry types. */
/** @} */

/**
 * @defgroup as_geometry_flags Acceleration Structure Geometry Flags
 * @ingroup pal_graphics
 * @{
 */
#define PAL_GEOMETRY_FLAG_OPAQUE (1U << 0) /**< The geometry is opaque. */
#define PAL_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT (1U << 1) /**< One any-hit call per primitive. */
/** @} */

/**
 * @struct PalAccelerationStructureInstance
 * @brief Acceleration structure instance data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAccelerationStructureInstance {
    PalAccelerationStructure* blas;              /**< Bottom level acceleration structure. */
    PalAccelerationStructureInstanceFlags flags; /**< Instance flags. */
    uint32_t mask;           /**< Visibility mask (only the lower 8 bits are used). */
    uint32_t instanceId;     /**< User-defined identifier. */
    uint32_t hitGroupOffset; /**< Offset added to hitgroup index in the Shader Binding Table. */
    float transform[12];     /**< Transform (row major 3x4). */
} PalAccelerationStructureInstance;

/**
 * @struct PalAccelerationStructureBuildSize
 * @brief Acceleration structure build size.
 *
 * Filled in by @ref palGetAccelerationStructureBuildSize().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAccelerationStructureBuildSize {
    uint64_t accelerationStructureSize; /**< Required acceleration structure size in bytes. */
    uint64_t scratchBufferSize;         /**< Required scratch buffer size in bytes. */
    uint64_t updateScratchBufferSize;   /**< Required scratch buffer update size in bytes. */
} PalAccelerationStructureBuildSize;

/**
 * @struct PalGeometryDataTriangle
 * @brief Acceleration structure triangle geometry data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalGeometryDataTriangle {
    PalDeviceAddress vertexBufferAddress;    /**< Address of the vertex buffer. */
    PalDeviceAddress indexBufferAddress;     /**< Address of the index buffer. */
    PalDeviceAddress transformBufferAddress; /**< Address of the transform buffer. */
    PalVertexType vertexType;                /**< Vertex attribute type. */
    PalIndexType indexType;                  /**< Index type. */
    uint32_t vertexCount;                    /**< Number of vertices. */
    uint32_t vertexStride;                   /**< Size of each vertex in bytes. */
} PalGeometryDataTriangle;

/**
 * @struct PalGeometryDataAABBS
 * @brief Acceleration structure AABBS geometry data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalGeometryDataAABBS {
    PalDeviceAddress bufferAddress; /**< Address of the AABBS buffer. */
    uint64_t stride;                /**< Size of each AABBS in bytes. */
} PalGeometryDataAABBS;

/**
 * @struct PalGeometry
 * @brief Acceleration structure geometry.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalGeometry {
    const void* data;        /**< Geometry data. */
    uint64_t primitiveCount; /**< Number of primitives in `data`. */
    PalGeometryFlags flags;  /**< Geometry flags. */
    PalGeometryType type;    /**< Geometry type. Must match `data`. */
} PalGeometry;

/**
 * @struct PalAccelerationStructureBuildInfo
 * @brief Acceleration structure build information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAccelerationStructureBuildInfo {
    PalAccelerationStructure* dst;          /**< Destination acceleration structure. */
    PalAccelerationStructure* src;          /**< Source acceleration structure for updates, or `nullptr`. */
    PalGeometry* geometries;                /**< BLAS geometries, or `nullptr` for a TLAS. */
    PalDeviceAddress scratchBufferAddress;  /**< Address of scratch buffer. */
    PalDeviceAddress instanceBufferAddress; /**< Address of the instance buffer, or `0` for a BLAS. */
    PalAccelerationStructureBuildHints buildHints; /**< Build hints. */
    PalAccelerationStructureType type; /**< Acceleration structure type. */
    PalAccelerationStructureBuildMode buildMode; /**< Acceleration structure build mode. */
    uint32_t count; /**< Number of elements in `geometries` or in the instance buffer. */
} PalAccelerationStructureBuildInfo;

/**
 * @struct PalAccelerationStructureCreateInfo
 * @brief Acceleration structure creation parameters.
 *
 * This struct is used only during @ref palCreateAccelerationstructure() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAccelerationStructureCreateInfo {
    PalBuffer* buffer;                 /**< Buffer that stores the acceleration structure. */
    uint64_t offset;                   /**< Offset in bytes into `buffer`. */
    uint64_t size;                     /**< Size in bytes. */
    PalAccelerationStructureType type; /**< Acceleration structure type. */
    uint32_t reserved;                 /**< Must be set to `0`. */
} PalAccelerationStructureCreateInfo;

/**
 * @brief Creates an acceleration structure.
 *
 * The created acceleration structure must be destroyed using @ref palDestroyAccelerationStructure().
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device that creates the acceleration structure.
 * @param[in] info Pointer to a PalAccelerationStructureCreateInfo struct that specifies parameters.
 * @param[out] outAs Pointer to a PalAccelerationStructure to receive the created acceleration
 * structure.
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
 * @sa palDestroyAccelerationStructure
 */
PAL_API PalResult PAL_CALL palCreateAccelerationstructure(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs);

/**
 * @brief Destroys an acceleration structure.
 *
 * @param[in] as Acceleration structure to destroy.
 *
 * @thread-safety The device used to create `as` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateAccelerationstructure
 */
PAL_API void PAL_CALL palDestroyAccelerationStructure(PalAccelerationStructure* as);

/**
 * @brief Gets the build size of an acceleration structure.
 *
 * PalAccelerationStructureBuildInfo::dst, PalAccelerationStructureBuildInfo::scratchBufferAddress
 * and PalAccelerationStructureBuildInfo::src must be set to `nullptr`.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device to query.
 * @param[in] info Pointer to a PalAccelerationStructureBuildInfo struct that specifies parameters.
 * @param[out] size Pointer to a PalAccelerationStructureBuildSize to receive the build size.
 *
 * @thread-safety `size` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palGetAccelerationStructureBuildSize(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size);

/**
 * @brief Computes size for an acceleration structure instance buffer.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * This does not allocate memory for the buffer. This function is required for all
 * acceleration structure instance buffers.
 *
 * @param[in] device The device to use.
 * @param[in] instanceCount Number of instances the instance buffer will hold.
 * @param[out] outSize Pointer to a uint64_t to receive the required size.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palWriteInstanceStaging
 */
PAL_API void PAL_CALL palComputeInstanceStagingSize(
    PalDevice* device,
    uint32_t instanceCount,
    uint64_t* outSize);

/**
 * @brief Writes data to an instance staging buffer.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device The device to use.
 * @param[in] instanceCount Number of instances.
 * @param[in] instances Array of PalAccelerationStructureInstance struct to write.
 * @param[out] ptr Pointer to the CPU visible memory. Must be mapped.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palComputeInstanceStagingSize
 */
PAL_API void PAL_CALL palWriteInstanceStaging(
    PalDevice* device,
    uint32_t instanceCount,
    PalAccelerationStructureInstance* instances,
    void* ptr);

#endif // PAL_GRAPHICS_ACCELERATION_STRUCTURE_H