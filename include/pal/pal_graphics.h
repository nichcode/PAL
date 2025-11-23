
/**

Copyright (C) 2025 Nicholas Agbo <agbonicholas04@gmail.com>

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
 * @defgroup pal_graphics Graphics
 * Graphics PAL functionality such as GPUs, GPUDevices, swapchains and more.
 *
 * @{
 */

#ifndef _PAL_GRAPHICS_H
#define _PAL_GRAPHICS_H

#include "pal_core.h"

typedef struct PalGPUAdapter PalGPUAdapter;
typedef struct PalGPUDevice PalGPUDevice;

typedef struct {
    PalResult PAL_CALL (*enumerateAdapters)(
        Int32* count, 
        PalGPUAdapter** outAdapters);
} PalGPUBackend;

PAL_API PalResult PAL_CALL palInitGraphics(const PalAllocator* allocator);

PAL_API void PAL_CALL palShutdownGraphics();

/** @} */ // end of pal_graphics group

#endif // _PAL_GRAPHICS_H