
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal/core/result.h"

const char* PAL_CALL palFormatResult(PalResult result)
{
    switch (result) {
        case PAL_RESULT_SUCCESS:
            return "Success";

        case PAL_RESULT_NULL_POINTER:
            return "Null pointer";

        case PAL_RESULT_INVALID_ARGUMENT:
            return "Invalid argument";

        case PAL_RESULT_OUT_OF_MEMORY:
            return "Out of memory";

        case PAL_RESULT_PLATFORM_FAILURE: {
            return "Platform error";
        }

        case PAL_RESULT_INVALID_ALLOCATOR:
            return "Invalif allocator";

        case PAL_RESULT_ACCESS_DENIED:
            return "Access denied";

        case PAL_RESULT_TIMEOUT:
            return "Timeout expired";

        case PAL_RESULT_INSUFFICIENT_BUFFER:
            return "Insufficient buffer";

        // thread
        case PAL_RESULT_INVALID_THREAD:
            return "Invalid thread";

        case PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED:
            return "Unsupported thread feature";

        // video
        case PAL_RESULT_VIDEO_NOT_INITIALIZED:
            return "Video system not initialized";

        case PAL_RESULT_INVALID_MONITOR:
            return "Invalid monitor";

        case PAL_RESULT_INVALID_MONITOR_MODE:
            return "Invalid monitor display mode";

        case PAL_RESULT_INVALID_WINDOW:
            return "Invalid window";

        case PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED:
            return "Unsupported video feature";

        case PAL_RESULT_INVALID_KEYCODE:
            return "Invalid keycode";

        case PAL_RESULT_INVALID_SCANCODE:
            return "Invalid scancode";

        case PAL_RESULT_INVALID_MOUSE_BUTTON:
            return "Invalid mouse button";

        case PAL_RESULT_INVALID_ORIENTATION:
            return "Invalid orientation";

        // opengl
        case PAL_RESULT_GL_NOT_INITIALIZED:
            return "Opengl system not initialized";

        case PAL_RESULT_INVALID_GL_WINDOW:
            return "Invalid opengl window";

        case PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED:
            return "Unsupported opengl extension";

        case PAL_RESULT_INVALID_GL_FBCONFIG:
            return "Invalid opengl framebuffer";

        case PAL_RESULT_INVALID_GL_VERSION:
            return "Unsupported opengl version";

        case PAL_RESULT_INVALID_GL_PROFILE:
            return "Unsupported opengl profile";

        case PAL_RESULT_INVALID_GL_CONTEXT:
            return "Invalid opengl context";

        case PAL_RESULT_INVALID_FBCONFIG_BACKEND:
            return "Invalid FBConfg backend";

        // graphics
        case PAL_RESULT_GRAPHICS_NOT_INITIALIZED:
            return "Graphics system not initialized";

        case PAL_RESULT_INVALID_ADAPTER:
            return "Invalid adapter";

        case PAL_RESULT_INVALID_BACKEND:
            return "Invalid backend";

        case PAL_RESULT_QUEUE_NOT_SUPPORTED:
            return "Queue not supported";

        case PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED:
            return "Unsupported adapter feature";

        case PAL_RESULT_INVALID_DRIVER:
            return "Incompatible driver";

        case PAL_RESULT_INVALID_DEVICE:
            return "Invalid device";

        case PAL_RESULT_INVALID_QUEUE:
            return "Invalid queue";

        case PAL_RESULT_OUT_OF_QUEUE:
            return "Out of queues";

        case PAL_RESULT_INVALID_GRAPHICS_WINDOW:
            return "Invalid graphics window";

        case PAL_RESULT_INVALID_SWAPCHAIN:
            return "Invalid swapchain";

        case PAL_RESULT_INVALID_IMAGE:
            return "Invalid image";

        case PAL_RESULT_INVALID_IMAGE_VIEW:
            return "Invalid image view";

        case PAL_RESULT_MEMORY_TYPE_NOT_SUPPORTED:
            return "memory type not supported";

        case PAL_RESULT_INVALID_OPERATION:
            return "Invalid operation";

        case PAL_RESULT_INVALID_SHADER:
            return "Invalid shader";

        case PAL_RESULT_INVALID_SHADER_TYPE:
            return "Invalid shader type";

        case PAL_RESULT_INVALID_COMMAND_POOL:
            return "Invalid command pool";

        case PAL_RESULT_INVALID_COMMAND_BUFFER:
            return "Invalid command buffer";

        case PAL_RESULT_INVALID_FENCE:
            return "Invalid fence";

        case PAL_RESULT_INVALID_SEMAPHORE:
            return "Invalid semaphore";

        case PAL_RESULT_INVALID_BUFFER:
            return "Invalid buffer";

        case PAL_RESULT_INVALID_PIPELINE:
            return "Invalid pipeline";

        case PAL_RESULT_INVALID_ACCELERATION_STRUCTURE:
            return "Invalid acceleration structure";

        case PAL_RESULT_MEMORY_MAP_FAILED:
            return "Memory map failed";

        case PAL_RESULT_DEVICE_LOST:
            return "Device lost";

        case PAL_RESULT_SURFACE_LOST:
            return "Surface lost";

        case PAL_RESULT_SWAPCHAIN_OUT_OF_DATE:
            return "Swapchain out of date";
    }
    return "Unknown";
}