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

#ifndef PAL_GRAPHICS_ACCELERATION_STRUCTURE_H
#define PAL_GRAPHICS_ACCELERATION_STRUCTURE_H

#include "buffer.h"
#include "types.h"

/**
 * @defgroup acceleration_structure_types Acceleration Structure Types
 * @brief Acceleration structure types
 * 
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL 0
#define PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL 1
#define PAL_ACCELERATION_STRUCTURE_TYPE_COUNT 2
/** @} */

/**
 * @defgroup as_build_modes Acceleration Structure Build Modes
 * @brief Acceleration structure build modes
 * 
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD 0
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_UPDATE 1
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_COUNT 2
/** @} */

/**
 * @defgroup as_build_hints Acceleration Structure Build Hints
 * @brief Acceleration structure build hints
 * 
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD (1U << 0)
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_TRACE (1U << 1)
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_LOW_MEMORY (1U << 2)
/** @} */

/**
 * @defgroup as_instance_flags Acceleration Structure Instance Flags
 * @brief Acceleration structure instance flags
 * 
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE (1U << 0)
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NO_OPAQUE (1U << 1)
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FACING_CULL_DISABLE (1U << 2)
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE (1U << 3)
/** @} */

/**
 * @defgroup geometry_types Geometry Types
 * @brief Geometry types
 * 
 * @{
 */
#define PAL_GEOMETRY_TYPE_TRIANGLE 0
#define PAL_GEOMETRY_TYPE_AABBS 1
#define PAL_GEOMETRY_TYPE_COUNT 2
/** @} */

/**
 * @defgroup geometry_flags Geometry Flags
 * @brief Geometry flags
 * 
 * @{
 */
#define PAL_GEOMETRY_FLAG_OPAQUE (1U << 0)
#define PAL_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT (1U << 1)
/** @} */

/**
 * @typedef PalAccelerationStructureType
 * @brief Acceleration structure types.
 * 
 * All values of this type follow the format 
 * `PAL_ACCELERATION_STRUCTURE_TYPE_*` for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAccelerationStructureType;

/**
 * @typedef PalAccelerationStructureBuildMode
 * @brief Acceleration structure build modes.
 * 
 * All values of this type follow the format 
 * `PAL_ACCELERATION_STRUCTURE_BUILD_MODE_*` for API consistency and 
 * ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAccelerationStructureBuildMode;

/**
 * @typedef PalAccelerationStructureBuildHints
 * @brief Acceleration structure build hints. 
 * 
 * Multiple hints can be OR'ed together using bitwise
 * OR operator (`|`). Hints can be ignored by the driver.
 * 
 * All values of this type follow the format 
 * `PAL_ACCELERATION_STRUCTURE_BUILD_HINT_*` for API consistency and 
 * ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAccelerationStructureBuildHints;

/**
 * @typedef PalAccelerationStructureInstanceFlags
 * @brief Acceleration structure instance flags. 
 * 
 * Multiple flags can be OR'ed together using bitwise OR operator (`|`).
 * 
 * All values of this type follow the format 
 * `PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_*` for API consistency and 
 * ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAccelerationStructureInstanceFlags;

/**
 * @typedef PalGeometryType
 * @brief Geometry types.
 * 
 * All values of this type follow the format `PAL_GEOMETRY_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGeometryType;

/**
 * @typedef PalGeometryFlags
 * @brief Geometry flags. 
 * 
 * Multiple flags can be OR'ed together using bitwise OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_GEOMETRY_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGeometryFlags;

/**
 * @struct PalAccelerationStructure
 * @brief Opaque handle to an acceleration structure.
 *
 * @since Added in version 2.0
 */
typedef struct PalAccelerationStructure PalAccelerationStructure;

/**
 * @struct PalAccelerationStructureInstance
 * @brief Contains acceleration structure instance base data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalAccelerationStructureInstance::blas
 * The bottom level acceleration structure.
 * 
 * @var PalAccelerationStructureInstance::flags
 * The instance flags.
 * 
 * @var PalAccelerationStructureInstance::mask
 * The mask. Only the lower `8-bits` are used (0x00 - 0xFF).
 * 
 * @var PalAccelerationStructureInstance::instanceId
 * The id used to identify the instance.
 * 
 * @var PalAccelerationStructureInstance::hitGroupOffset
 * The hitgroup offset.
 * 
 * @var PalAccelerationStructureInstance::transform
 * The instance trasnform. Must be row major.
 */
typedef struct PalAccelerationStructureInstance
{
    PalAccelerationStructure* blas; 
    PalAccelerationStructureInstanceFlags flags; 
    uint32_t mask;
    uint32_t instanceId;
    uint32_t hitGroupOffset;
    float transform[12];
} PalAccelerationStructureInstance;

/**
 * @struct PalAccelerationStructureBuildSize
 * @brief Contains acceleration structure build size.
 *
 * @since Added in version 2.0
 * 
 * @var PalAccelerationStructureBuildSize::accelerationStructureSize
 * The required acceleration structure size in bytes.
 * 
 * @var PalAccelerationStructureBuildSize::scratchBufferSize
 * The required scratch buffer size in bytes.
 * 
 * @var PalAccelerationStructureBuildSize::updateScratchBufferSize
 * The required scratch buffer update size in bytes.
 */
typedef struct PalAccelerationStructureBuildSize
{
    uint64_t accelerationStructureSize;
    uint64_t scratchBufferSize;        
    uint64_t updateScratchBufferSize;  
} PalAccelerationStructureBuildSize;

/**
 * @struct PalGeometryDataTriangle
 * @brief Contains acceleration structure triangle geometry data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalGeometryDataTriangle::vertexBufferAddress
 * The address of the vertex buffer.
 * 
 * @var PalGeometryDataTriangle::indexBufferAddress
 * The address of the index buffer.
 * 
 * @var PalGeometryDataTriangle::transformBufferAddress
 * The address of the transform buffer.
 * 
 * @var PalGeometryDataTriangle::vertexType
 * The vertex type of ::vertexBufferAddress.
 * 
 * @var PalGeometryDataTriangle::indexType
 * The index type of ::indexBufferAddress.
 * 
 * @var PalGeometryDataTriangle::vertexCount
 * The number of vertices in ::vertexBufferAddress.
 * 
 * @var PalGeometryDataTriangle::vertexStride
 * The size of each vertex in bytes in ::vertexBufferAddress.
 */
typedef struct PalGeometryDataTriangle
{
    PalDeviceAddress vertexBufferAddress;
    PalDeviceAddress indexBufferAddress;
    PalDeviceAddress transformBufferAddress;
    PalVertexType vertexType;
    PalIndexType indexType;
    uint32_t vertexCount;
    uint32_t vertexStride;
} PalGeometryDataTriangle;

/**
 * @struct PalGeometryDataAABBS
 * @brief Contains acceleration structure AABBS geometry data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalGeometryDataAABBS::bufferAddress
 * The address of the `AABBS` buffer.
 * 
 * @var PalGeometryDataAABBS::stride
 * The size of each `AABBS` in bytes in ::bufferAddress.
 */
typedef struct PalGeometryDataAABBS
{
    PalDeviceAddress bufferAddress;
    uint64_t stride;               
} PalGeometryDataAABBS;

/**
 * @struct PalGeometry
 * @brief Contains acceleration structure geometry.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalGeometry::data
 * The geometry data. This is based on ::type.
 * 
 * @var PalGeometry::primitiveCount
 * The number of primitives in ::data.
 * 
 * @var PalGeometry::flags
 * The geometry flags.
 * 
 * @var PalGeometry::type
 * The geometry type (eg. `PAL_GEOMETRY_TYPE_TRIANGLE`).
 */
typedef struct PalGeometry
{
    const void* data;
    uint64_t primitiveCount;
    PalGeometryFlags flags;
    PalGeometryType type;  
} PalGeometry;

/**
 * @struct PalAccelerationStructureBuildInfo
 * @brief Contains build information of an acceleration structure.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalAccelerationStructureBuildInfo::dst
 * The destination acceleration structure.
 * 
 * @var PalAccelerationStructureBuildInfo::src
 * The Source acceleration structure. This is used update builds.
 * 
 * @var PalAccelerationStructureBuildInfo::geometries
 * The Bottom level acceleration structure geometries.
 * Set to `nullptr` if ::type is `PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL`.
 * 
 * @var PalAccelerationStructureBuildInfo::scratchBufferAddress
 * The address of the scratch buffer.
 * 
 * @var PalAccelerationStructureBuildInfo::instanceBufferAddress
 * The address of the instance buffer.
 * Set to `nullptr` if ::type is `PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL`
 * 
 * @var PalAccelerationStructureBuildInfo::buildHints
 * Driver build hints. These are hints and might be ignored.
 * 
 * @var PalAccelerationStructureBuildInfo::type
 * The type of the acceleration structure.
 * 
 * @var PalAccelerationStructureBuildInfo::buildMode
 * The build mode of the acceleration structure.
 * (eg. `PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD`).
 * 
 * @var PalAccelerationStructureBuildInfo::count
 * If ::type is `PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL`, this is
 * the number of instances in ::instanceBufferAddress. @nl
 * If ::type is `PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL`, this is @nl
 * the number of geometries in ::geometries.
 */
typedef struct PalAccelerationStructureBuildInfo
{
    PalAccelerationStructure* dst;
    PalAccelerationStructure* src;
    PalGeometry* geometries;
    PalDeviceAddress scratchBufferAddress;
    PalDeviceAddress instanceBufferAddress;
    PalAccelerationStructureBuildHints buildHints;
    PalAccelerationStructureType type;
    PalAccelerationStructureBuildMode buildMode;
    uint32_t count;
} PalAccelerationStructureBuildInfo;

/**
 * @struct PalAccelerationStructureCreateInfo
 * @brief Contains creation parameters of an acceleration structure.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalAccelerationStructureCreateInfo::buffer
 * The acceleration structure buffer. It must be valid.
 * 
 * @var PalAccelerationStructureCreateInfo::offset
 * The offset in bytes into the buffer.
 * 
 * @var PalAccelerationStructureCreateInfo::size
 * The size in bytes of the buffer.
 * 
 * @var PalAccelerationStructureCreateInfo::type
 * The type of the acceleration structure.
 * (eg. `PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL`).
 * 
 * @var PalAccelerationStructureCreateInfo::reserved
 * Not used. Set to `0`.
 */
typedef struct PalAccelerationStructureCreateInfo
{
    PalBuffer* buffer;
    uint64_t offset;
    uint64_t size;   
    PalAccelerationStructureType type;
    uint32_t reserved;
} PalAccelerationStructureCreateInfo;

/**
 * @brief Create an acceleration structure.
 *
 * The created acceleration structure must be destroyed using `palDestroyAccelerationStructure()`.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device that creates the acceleration structure.
 * @param[in] info Pointer to a PalAccelerationStructureCreateInfo struct that specifies parameters
 * @param[out] outAs Pointer to a PalAccelerationStructure to recieve the created acceleration
 * structure.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroyAccelerationStructure
 */
PAL_API PalResult PAL_CALL palCreateAccelerationstructure(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs);

/**
 * @brief Destroy an acceleration structure.
 *
 * @param[in] as Acceleration structure to destroy.
 *
 * Thread safety: Thread safe if the device used to create the acceleration structure is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateAccelerationstructure
 */
PAL_API void PAL_CALL palDestroyAccelerationStructure(PalAccelerationStructure* as);

/**
 * @brief Get the build size of an acceleration structure.
 *
 * PalAccelerationStructureBuildInfo::dst, PalAccelerationStructureBuildInfo::scratchBufferAddress
 * and PalAccelerationStructureBuildInfo::src must be set to `nullptr`.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device to query.
 * @param[in] info Pointer to a PalAccelerationStructureBuildInfo struct that specifies parameters.
 * @param[out] size Pointer to a PalAccelerationStructureBuildSize to recieve the build size.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetAccelerationStructureBuildSize(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size);

/**
 * @brief Compute size for an acceleration structure instance buffer.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * This does not allocate memory for the buffer. This function must is required for all
 * acceleration structure instance buffers.
 *
 * @param[in] device The device to use.
 * @param[in] instanceCount Number of instances the instance buffer will hold.
 * @param[out] outSize Pointer to a uint64_t to recieve the required size.
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 * @sa palWriteInstanceStaging
 */
PAL_API void PAL_CALL palComputeInstanceStagingSize(
    PalDevice* device,
    uint32_t instanceCount,
    uint64_t* outSize);

/**
 * @brief Write data to an instance staging buffer.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device The device to use.
 * @param[in] instanceCount Number of instances.
 * @param[in] instances Array of PalAccelerationStructureInstance struct to write.
 * @param[out] ptr Pointer to the CPU visible memory. Must be mapped.
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 * @sa palComputeInstanceStagingSize
 */
PAL_API void PAL_CALL palWriteInstanceStaging(
    PalDevice* device,
    uint32_t instanceCount,
    PalAccelerationStructureInstance* instances,
    void* ptr);

#endif // PAL_GRAPHICS_ACCELERATION_STRUCTURE_H