
/**
 * @file vertex.h
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

#ifndef PAL_GRAPHICS_VERTEX_H
#define PAL_GRAPHICS_VERTEX_H

#include "pal2/core/base.h"

/**
 * @typedef PalVertexType
 * @brief Vertex attribute type.
 *
 * Identifies the data type of a vertex attribute.
 *
 * All values of this type follow the format `PAL_VERTEX_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalVertexType;

/**
 * @typedef PalVertexSemanticID
 * @brief Vertex semantic identifier.
 *
 * Identifies the meaning of a vertex attribute.
 *
 * All values of this type follow the format `PAL_VERTEX_SEMANTIC_ID_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalVertexSemanticID;

/**
 * @typedef PalVertexLayoutType
 * @brief Vertex layout type.
 *
 * Identifies the rate at which vertex attributes advance.
 *
 * All values of this type follow the format `PAL_VERTEX_LAYOUT_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalVertexLayoutType;

/**
 * @typedef PalIndexType
 * @brief Index type.
 *
 * Identifies the size of the indices in an index buffer.
 *
 * All values of this type follow the format `PAL_INDEX_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalIndexType;

/**
 * @defgroup vertex_types Vertex Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_VERTEX_TYPE_UNDEFINED 0 /**< Invalid vertex type. */
#define PAL_VERTEX_TYPE_INT32 1
#define PAL_VERTEX_TYPE_INT32_2 2
#define PAL_VERTEX_TYPE_INT32_3 3
#define PAL_VERTEX_TYPE_INT32_4 4
#define PAL_VERTEX_TYPE_UINT32 5
#define PAL_VERTEX_TYPE_UINT32_2 6
#define PAL_VERTEX_TYPE_UINT32_3 7
#define PAL_VERTEX_TYPE_UINT32_4 8
#define PAL_VERTEX_TYPE_INT8_2 9
#define PAL_VERTEX_TYPE_INT8_4 10
#define PAL_VERTEX_TYPE_UINT8_2 11
#define PAL_VERTEX_TYPE_UINT8_4 12
#define PAL_VERTEX_TYPE_INT8_2NORM 13
#define PAL_VERTEX_TYPE_INT8_4NORM 14
#define PAL_VERTEX_TYPE_UINT8_2NORM 15
#define PAL_VERTEX_TYPE_UINT8_4NORM 16
#define PAL_VERTEX_TYPE_INT16_2 17
#define PAL_VERTEX_TYPE_INT16_4 18
#define PAL_VERTEX_TYPE_UINT16_2 19
#define PAL_VERTEX_TYPE_UINT16_4 20
#define PAL_VERTEX_TYPE_INT16_2NORM 21
#define PAL_VERTEX_TYPE_INT16_4NORM 22
#define PAL_VERTEX_TYPE_UINT16_2NORM 23
#define PAL_VERTEX_TYPE_UINT16_4NORM 24
#define PAL_VERTEX_TYPE_FLOAT 25
#define PAL_VERTEX_TYPE_FLOAT2 26
#define PAL_VERTEX_TYPE_FLOAT3 27
#define PAL_VERTEX_TYPE_FLOAT4 28
#define PAL_VERTEX_TYPE_HALF_FLOAT16_2 29
#define PAL_VERTEX_TYPE_HALF_FLOAT16_4 30
#define PAL_VERTEX_TYPE_COUNT 31 /**< Number of vertex types. */
/** @} */

/**
 * @defgroup vertex_semantic_ids Vertex Semantic IDs
 * @ingroup pal_graphics
 * @{
 */
#define PAL_VERTEX_SEMANTIC_ID_POSITION 0 /**< Vertex position. */
#define PAL_VERTEX_SEMANTIC_ID_COLOR 1 /**< Vertex color. */
#define PAL_VERTEX_SEMANTIC_ID_TEXCOORD 2 /**< Texture coordinates. */
#define PAL_VERTEX_SEMANTIC_ID_NORMAL 3 /**< Vertex normal. */
#define PAL_VERTEX_SEMANTIC_ID_TANGENT 4 /**< Vertex tangent. */
#define PAL_VERTEX_SEMANTIC_ID_COUNT 5 /**< Number of vertex semantic identifiers. */
/** @} */

/**
 * @defgroup vertex_layout_types Vertex Layout Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_VERTEX_LAYOUT_TYPE_PER_VERTEX 0 /**< Attributes advance once per vertex. */
#define PAL_VERTEX_LAYOUT_TYPE_PER_INSTANCE 1 /**< Attributes advance once per instance. */
#define PAL_VERTEX_LAYOUT_TYPE_COUNT 2 /**< Number of vertex layout types. */
/** @} */

/**
 * @defgroup index_types Index Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_INDEX_TYPE_UINT16 0 /**< 16-bit indices. */
#define PAL_INDEX_TYPE_UINT32 1 /**< 32-bit indices. */
#define PAL_INDEX_TYPE_COUNT 2 /**< Number of index types. */
/** @} */

/**
 * @struct PalVertexAttribute
 * @brief Vertex attribute.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalVertexAttribute {
    PalVertexSemanticID semanticID; /**< Vertex attribute semantic identifier. */
    PalVertexType type;             /**< Vertex attribute type. */
} PalVertexAttribute;

/**
 * @struct PalVertexLayout
 * @brief Vertex layout.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalVertexLayout {
    PalVertexAttribute* attributes; /**< Vertex attributes. */
    uint32_t attributeCount;        /**< Number of vertex attributes. */
    PalVertexLayoutType type;       /**< Vertex layout type. */
    uint32_t binding;               /**< Vertex buffer binding slot. */
    uint32_t reserved;              /**< Must be set to `0`. */
} PalVertexLayout;

#endif // PAL_GRAPHICS_VERTEX_H