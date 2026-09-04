
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
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

#include <stdbool.h>

#ifndef SHARED_H_
#define SHARED_H_

#ifdef _MSC_VER
#define ALIGNOF_(type) __alignof(type)
#else
#define ALIGNOF_(type) __alignof__(type)
#endif // _MSC_VER

#ifndef __cplusplus
#define bool _Bool
#endif // __cplusplus

#define ARRAY_SIZE_(array) (sizeof(array) / sizeof((array)[0]))
#define ALIGN_UP_(value, alignment) (((value) + (alignment) - 1) & ~((alignment) - 1))

#endif // SHARED_H_