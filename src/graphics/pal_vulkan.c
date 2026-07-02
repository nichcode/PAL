
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

// ==================================================
// Includes
// ==================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pal/pal_graphics.h"

#if PAL_HAS_VULKAN_BACKEND
#include <vulkan/vulkan_core.h>

// HACK: Needed to determine display type if on linux
#ifdef _WIN32
#include <libloaderapi.h>
#define VK_LIB_NAME "vulkan-1.dll"
#elif defined(__linux__)
#include <dlfcn.h>
#define VK_LIB_NAME "libvulkan.so"
#else
// Android
#define VK_LIB_NAME ""
#endif // _WIN32

// ==================================================
// Typedefs, enums and structs
// ==================================================

#pragma region Video

struct wl_display;
struct wl_surface;
typedef struct _XDisplay Display;
typedef unsigned long Window;
typedef unsigned long VisualID;
typedef unsigned long Colormap;
typedef char *XPointer;
typedef struct _XGC *GC;

typedef struct _XExtData {
	int number;
	struct _XExtData *next;
	int (*free_private)(
	struct _XExtData *extension
	);
	XPointer private_data;
} XExtData;

typedef struct {
	XExtData *ext_data;
	VisualID visualid;
	int class;
	unsigned long red_mask, green_mask, blue_mask;
	int bits_per_rgb;
	int map_entries;
} Visual;

typedef struct {
	int depth;
	int nvisuals;
	Visual *visuals;
} Depth;

typedef struct {
	XExtData *ext_data;
	struct _XDisplay *display;
	Window root;
	int width, height;
	int mwidth, mheight;
	int ndepths;
	Depth *depths;
	int root_depth;
	Visual *root_visual;
	GC default_gc;
	Colormap cmap;
	unsigned long white_pixel;
	unsigned long black_pixel;
	int max_maps, min_maps;
	int backing_store;
	int save_unders;
	long root_input_mask;
} Screen;

typedef struct {
    int x, y;
    int width, height;
    int border_width;
    int depth;
    Visual *visual;
    Window root;
    int class;
    int bit_gravity;
    int win_gravity;
    int backing_store;
    unsigned long backing_planes;
    unsigned long backing_pixel;
    int save_under;
    Colormap colormap;
    int map_installed;
    int map_state;
    long all_event_masks;
    long your_event_mask;
    long do_not_propagate_mask;
    int override_redirect;
    Screen *screen;
} XWindowAttributes;

typedef int (*XGetWindowAttributesFn)(
    Display*,
    Window,
    XWindowAttributes*);

typedef VisualID (*XVisualIDFromVisualFn)(Visual*);

typedef struct xcb_connection_t xcb_connection_t;
typedef uint32_t xcb_window_t;
typedef uint32_t xcb_visualid_t;
typedef uint32_t xcb_colormap_t;

typedef struct xcb_get_window_attributes_cookie_t {
    unsigned int sequence;
} xcb_get_window_attributes_cookie_t;

typedef struct xcb_get_window_attributes_reply_t {
    uint8_t        response_type;
    uint8_t        backing_store;
    uint16_t       sequence;
    uint32_t       length;
    xcb_visualid_t visual;
    uint16_t       _class;
    uint8_t        bit_gravity;
    uint8_t        win_gravity;
    uint32_t       backing_planes;
    uint32_t       backing_pixel;
    uint8_t        save_under;
    uint8_t        map_is_installed;
    uint8_t        map_state;
    uint8_t        override_redirect;
    xcb_colormap_t colormap;
    uint32_t       all_event_masks;
    uint32_t       your_event_mask;
    uint16_t       do_not_propagate_mask;
    uint8_t        pad0[2];
} xcb_get_window_attributes_reply_t;

typedef struct {
    uint8_t   response_type;
    uint8_t   error_code;
    uint16_t sequence;
    uint32_t resource_id;
    uint16_t minor_code;
    uint8_t major_code;
    uint8_t pad0;
    uint32_t pad[5];
    uint32_t full_sequence;
} xcb_generic_error_t;

typedef xcb_get_window_attributes_cookie_t (*xcb_get_window_attributes_fn)(
    xcb_connection_t*,
    xcb_window_t);

typedef xcb_get_window_attributes_reply_t* (*xcb_get_window_attributes_reply_fn)(
    xcb_connection_t*,
    xcb_get_window_attributes_cookie_t,
    xcb_generic_error_t**);

typedef unsigned long DWORD;
typedef int WINBOOL;
typedef void *LPVOID;
typedef const wchar_t *LPCWSTR,*PCWSTR;
typedef void *HANDLE;
typedef struct HINSTANCE__ *HINSTANCE;
typedef struct HWND__ *HWND;
typedef struct HMONITOR__ *HMONITOR;

// only define the struct if not on windows
#ifndef _MINWINBASE_
typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID ipSecurityDescriptor;
    WINBOOL bInheritHandle;
} SECURITY_ATTRIBUTES;
#endif // _MINWINBASE_

#pragma endregion

static Vulkan s_Vk = {0};

// ==================================================
// Helper Functions
// ==================================================

static void* loadLibrary(const char* name)
{
#ifdef _WIN32
    return LoadLibraryA(name);
#elif defined (__linux__)
    return dlopen(name, RTLD_LAZY);
#endif
}

static void freeLibrary(void* lib)
{
#ifdef _WIN32
    FreeLibrary((HMODULE)lib);
#elif defined (__linux__)
    dlclose(lib);
#endif
}

static void* loadProc(void* lib, const char* name)
{
#ifdef _WIN32
    return GetProcAddress((HMODULE)lib, name);
#elif defined (__linux__)
    return dlsym(lib, name);
#endif
}

VkRenderingFlags renderingFlagToVk(PalRenderingFlags flags)
{
    VkRenderingFlags renderingFlags = 0;
    if (flags == PAL_RENDERING_FLAG_NONE) {
        renderingFlags = 0;
    }

    if (flags & PAL_RENDERING_FLAG_RESUMING) {
        renderingFlags |= VK_RENDERING_RESUMING_BIT;
    }

    if (flags & PAL_RENDERING_FLAG_SUSPENDING) {
        renderingFlags |= VK_RENDERING_SUSPENDING_BIT;
    }

    return renderingFlags;
}

static PalResult resultFromVknnn(VkResult result)
{
    switch (result) {
        case VK_ERROR_FEATURE_NOT_PRESENT:
        case VK_ERROR_EXTENSION_NOT_PRESENT: {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

        case VK_ERROR_OUT_OF_HOST_MEMORY:
        case VK_ERROR_TOO_MANY_OBJECTS:
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        case VK_ERROR_INITIALIZATION_FAILED:
            return PAL_RESULT_PLATFORM_FAILURE;

        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return PAL_RESULT_INVALID_DRIVER;

        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: {
            return PAL_RESULT_INVALID_WINDOW;
        }

        case VK_TIMEOUT:
            return PAL_RESULT_TIMEOUT;

        case VK_ERROR_MEMORY_MAP_FAILED:
            return PAL_RESULT_MEMORY_MAP_FAILED;

        case VK_ERROR_DEVICE_LOST:
            return PAL_RESULT_DEVICE_LOST;

        case VK_ERROR_SURFACE_LOST_KHR:
            return PAL_RESULT_SURFACE_LOST;

        case VK_ERROR_OUT_OF_DATE_KHR:
            return PAL_RESULT_SWAPCHAIN_OUT_OF_DATE;

        default:
            return PAL_RESULT_PLATFORM_FAILURE;
    }
    return PAL_RESULT_PLATFORM_FAILURE;
}

static VkFormat formatToVk(PalFormat format)
{
    switch (format) {
        case PAL_FORMAT_R8_UNORM:
            return VK_FORMAT_R8_UNORM;

        case PAL_FORMAT_R8_SNORM:
            return VK_FORMAT_R8_SNORM;

        case PAL_FORMAT_R8_UINT:
            return VK_FORMAT_R8_UINT;

        case PAL_FORMAT_R8_SINT:
            return VK_FORMAT_R8_SINT;

        case PAL_FORMAT_R8_SRGB:
            return VK_FORMAT_R8_SRGB;

        case PAL_FORMAT_R16_UNORM:
            return VK_FORMAT_R16_UNORM;

        case PAL_FORMAT_R16_SNORM:
            return VK_FORMAT_R16_SNORM;

        case PAL_FORMAT_R16_UINT:
            return VK_FORMAT_R16_UINT;

        case PAL_FORMAT_R16_SINT:
            return VK_FORMAT_R16_SINT;

        case PAL_FORMAT_R16_SFLOAT:
            return VK_FORMAT_R16_SFLOAT;

        case PAL_FORMAT_R32_UINT:
            return VK_FORMAT_R32_UINT;

        case PAL_FORMAT_R32_SINT:
            return VK_FORMAT_R32_SINT;

        case PAL_FORMAT_R32_SFLOAT:
            return VK_FORMAT_R32_SFLOAT;

        case PAL_FORMAT_R64_UINT:
            return VK_FORMAT_R64_UINT;

        case PAL_FORMAT_R64_SINT:
            return VK_FORMAT_R64_SINT;

        case PAL_FORMAT_R64_SFLOAT:
            return VK_FORMAT_R64_SFLOAT;

        case PAL_FORMAT_R8G8_UNORM:
            return VK_FORMAT_R8G8_UNORM;

        case PAL_FORMAT_R8G8_SNORM:
            return VK_FORMAT_R8G8_SNORM;

        case PAL_FORMAT_R8G8_UINT:
            return VK_FORMAT_R8G8_UINT;

        case PAL_FORMAT_R8G8_SINT:
            return VK_FORMAT_R8G8_SINT;

        case PAL_FORMAT_R8G8_SRGB:
            return VK_FORMAT_R8G8_SRGB;

        case PAL_FORMAT_R16G16_UNORM:
            return VK_FORMAT_R16G16_UNORM;

        case PAL_FORMAT_R16G16_SNORM:
            return VK_FORMAT_R16G16_SNORM;

        case PAL_FORMAT_R16G16_UINT:
            return VK_FORMAT_R16G16_UINT;

        case PAL_FORMAT_R16G16_SINT:
            return VK_FORMAT_R16G16_SINT;

        case PAL_FORMAT_R16G16_SFLOAT:
            return VK_FORMAT_R16G16_SFLOAT;

        case PAL_FORMAT_R32G32_UINT:
            return VK_FORMAT_R32G32_UINT;

        case PAL_FORMAT_R32G32_SINT:
            return VK_FORMAT_R32G32_SINT;

        case PAL_FORMAT_R32G32_SFLOAT:
            return VK_FORMAT_R32G32_SFLOAT;

        case PAL_FORMAT_R64G64_UINT:
            return VK_FORMAT_R64G64_UINT;

        case PAL_FORMAT_R64G64_SINT:
            return VK_FORMAT_R64G64_SINT;

        case PAL_FORMAT_R64G64_SFLOAT:
            return VK_FORMAT_R64G64_SFLOAT;

        case PAL_FORMAT_R8G8B8_UNORM:
            return VK_FORMAT_R8G8B8_UNORM;

        case PAL_FORMAT_R8G8B8_SNORM:
            return VK_FORMAT_R8G8B8_SNORM;

        case PAL_FORMAT_R8G8B8_UINT:
            return VK_FORMAT_R8G8B8_UINT;

        case PAL_FORMAT_R8G8B8_SINT:
            return VK_FORMAT_R8G8B8_SINT;

        case PAL_FORMAT_R8G8B8_SRGB:
            return VK_FORMAT_R8G8B8_SRGB;

        case PAL_FORMAT_R16G16B16_UNORM:
            return VK_FORMAT_R16G16B16_UNORM;

        case PAL_FORMAT_R16G16B16_SNORM:
            return VK_FORMAT_R16G16B16_SNORM;

        case PAL_FORMAT_R16G16B16_UINT:
            return VK_FORMAT_R16G16B16_UINT;

        case PAL_FORMAT_R16G16B16_SINT:
            return VK_FORMAT_R16G16B16_SINT;

        case PAL_FORMAT_R16G16B16_SFLOAT:
            return VK_FORMAT_R16G16B16_SFLOAT;

        case PAL_FORMAT_R32G32B32_UINT:
            return VK_FORMAT_R32G32B32_UINT;

        case PAL_FORMAT_R32G32B32_SINT:
            return VK_FORMAT_R32G32B32_SINT;

        case PAL_FORMAT_R32G32B32_SFLOAT:
            return VK_FORMAT_R32G32B32_SFLOAT;

        case PAL_FORMAT_R64G64B64_UINT:
            return VK_FORMAT_R64G64B64_UINT;

        case PAL_FORMAT_R64G64B64_SINT:
            return VK_FORMAT_R64G64B64_SINT;

        case PAL_FORMAT_R64G64B64_SFLOAT:
            return VK_FORMAT_R64G64B64_SFLOAT;

        case PAL_FORMAT_B8G8R8_UNORM:
            return VK_FORMAT_B8G8R8_UNORM;

        case PAL_FORMAT_B8G8R8_SNORM:
            return VK_FORMAT_B8G8R8_SNORM;

        case PAL_FORMAT_B8G8R8_UINT:
            return VK_FORMAT_B8G8R8_UINT;

        case PAL_FORMAT_B8G8R8_SINT:
            return VK_FORMAT_B8G8R8_SINT;

        case PAL_FORMAT_B8G8R8_SRGB:
            return VK_FORMAT_B8G8R8_SRGB;

        case PAL_FORMAT_R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;

        case PAL_FORMAT_R8G8B8A8_SNORM:
            return VK_FORMAT_R8G8B8A8_SNORM;

        case PAL_FORMAT_R8G8B8A8_UINT:
            return VK_FORMAT_R8G8B8A8_UINT;

        case PAL_FORMAT_R8G8B8A8_SINT:
            return VK_FORMAT_R8G8B8A8_SINT;

        case PAL_FORMAT_R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;

        case PAL_FORMAT_R16G16B16A16_UNORM:
            return VK_FORMAT_R16G16B16A16_UNORM;

        case PAL_FORMAT_R16G16B16A16_SNORM:
            return VK_FORMAT_R16G16B16A16_SNORM;

        case PAL_FORMAT_R16G16B16A16_UINT:
            return VK_FORMAT_R16G16B16A16_UINT;

        case PAL_FORMAT_R16G16B16A16_SINT:
            return VK_FORMAT_R16G16B16A16_SINT;

        case PAL_FORMAT_R16G16B16A16_SFLOAT:
            return VK_FORMAT_R16G16B16A16_SFLOAT;

        case PAL_FORMAT_R32G32B32A32_UINT:
            return VK_FORMAT_R32G32B32A32_UINT;

        case PAL_FORMAT_R32G32B32A32_SINT:
            return VK_FORMAT_R32G32B32A32_SINT;

        case PAL_FORMAT_R32G32B32A32_SFLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;

        case PAL_FORMAT_R64G64B64A64_UINT:
            return VK_FORMAT_R64G64B64A64_UINT;

        case PAL_FORMAT_R64G64B64A64_SINT:
            return VK_FORMAT_R64G64B64A64_SINT;

        case PAL_FORMAT_R64G64B64A64_SFLOAT:
            return VK_FORMAT_R64G64B64A64_SFLOAT;

        case PAL_FORMAT_B8G8R8A8_UNORM:
            return VK_FORMAT_B8G8R8A8_UNORM;

        case PAL_FORMAT_B8G8R8A8_SNORM:
            return VK_FORMAT_B8G8R8A8_SNORM;

        case PAL_FORMAT_B8G8R8A8_UINT:
            return VK_FORMAT_B8G8R8A8_UINT;

        case PAL_FORMAT_B8G8R8A8_SINT:
            return VK_FORMAT_B8G8R8A8_SINT;

        case PAL_FORMAT_B8G8R8A8_SRGB:
            return VK_FORMAT_B8G8R8A8_SRGB;

        case PAL_FORMAT_S8_UINT:
            return VK_FORMAT_S8_UINT;

        case PAL_FORMAT_D16_UNORM:
            return VK_FORMAT_D16_UNORM;

        case PAL_FORMAT_D32_SFLOAT:
            return VK_FORMAT_D32_SFLOAT;

        case PAL_FORMAT_D32_SFLOAT_S8_UINT:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;

        case PAL_FORMAT_D16_UNORM_S8_UINT:
            return VK_FORMAT_D16_UNORM_S8_UINT;

        case PAL_FORMAT_D24_UNORM_S8_UINT:
            return VK_FORMAT_D24_UNORM_S8_UINT;
    }

    return VK_FORMAT_UNDEFINED;
}

static VkSampleCountFlags samplesToVk(PalSampleCount count)
{
    switch (count) {
        case PAL_SAMPLE_COUNT_2:
            return VK_SAMPLE_COUNT_2_BIT;

        case PAL_SAMPLE_COUNT_4:
            return VK_SAMPLE_COUNT_4_BIT;

        case PAL_SAMPLE_COUNT_8:
            return VK_SAMPLE_COUNT_8_BIT;

        case PAL_SAMPLE_COUNT_16:
            return VK_SAMPLE_COUNT_16_BIT;

        case PAL_SAMPLE_COUNT_32:
            return VK_SAMPLE_COUNT_32_BIT;

        case PAL_SAMPLE_COUNT_64:
            return VK_SAMPLE_COUNT_64_BIT;
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

static PalSampleCount samplesFromVk(VkSampleCountFlags count)
{
    if (count & VK_SAMPLE_COUNT_2_BIT) {
        return PAL_SAMPLE_COUNT_2;

    } else if (count & VK_SAMPLE_COUNT_4_BIT) {
        return PAL_SAMPLE_COUNT_4;

    } else if (count & VK_SAMPLE_COUNT_8_BIT) {
        return PAL_SAMPLE_COUNT_8;

    } else if (count & VK_SAMPLE_COUNT_16_BIT) {
        return PAL_SAMPLE_COUNT_16;

    } else if (count & VK_SAMPLE_COUNT_32_BIT) {
        return PAL_SAMPLE_COUNT_32;

    } else if (count & VK_SAMPLE_COUNT_64_BIT) {
        return PAL_SAMPLE_COUNT_64;
    }

    return PAL_SAMPLE_COUNT_1;
}

static PalFormat formatFromVk(VkFormat format)
{
    switch (format) {
        case VK_FORMAT_R8_UNORM:
            return PAL_FORMAT_R8_UNORM;

        case VK_FORMAT_R8_SNORM:
            return PAL_FORMAT_R8_SNORM;

        case VK_FORMAT_R8_UINT:
            return PAL_FORMAT_R8_UINT;

        case VK_FORMAT_R8_SINT:
            return PAL_FORMAT_R8_SINT;

        case VK_FORMAT_R8_SRGB:
            return PAL_FORMAT_R8_SRGB;

        case VK_FORMAT_R16_UNORM:
            return PAL_FORMAT_R16_UNORM;

        case VK_FORMAT_R16_SNORM:
            return PAL_FORMAT_R16_SNORM;

        case VK_FORMAT_R16_UINT:
            return PAL_FORMAT_R16_UINT;

        case VK_FORMAT_R16_SINT:
            return PAL_FORMAT_R16_SINT;

        case VK_FORMAT_R16_SFLOAT:
            return PAL_FORMAT_R16_SFLOAT;

        case VK_FORMAT_R32_UINT:
            return PAL_FORMAT_R32_UINT;

        case VK_FORMAT_R32_SINT:
            return PAL_FORMAT_R32_SINT;

        case VK_FORMAT_R32_SFLOAT:
            return PAL_FORMAT_R32_SFLOAT;

        case VK_FORMAT_R64_UINT:
            return PAL_FORMAT_R64_UINT;

        case VK_FORMAT_R64_SINT:
            return PAL_FORMAT_R64_SINT;

        case VK_FORMAT_R64_SFLOAT:
            return PAL_FORMAT_R64_SFLOAT;

        case VK_FORMAT_R8G8_UNORM:
            return PAL_FORMAT_R8G8_UNORM;

        case VK_FORMAT_R8G8_SNORM:
            return PAL_FORMAT_R8G8_SNORM;

        case VK_FORMAT_R8G8_UINT:
            return PAL_FORMAT_R8G8_UINT;

        case VK_FORMAT_R8G8_SINT:
            return PAL_FORMAT_R8G8_SINT;

        case VK_FORMAT_R8G8_SRGB:
            return PAL_FORMAT_R8G8_SRGB;

        case VK_FORMAT_R16G16_UNORM:
            return PAL_FORMAT_R16G16_UNORM;

        case VK_FORMAT_R16G16_SNORM:
            return PAL_FORMAT_R16G16_SNORM;

        case VK_FORMAT_R16G16_UINT:
            return PAL_FORMAT_R16G16_UINT;

        case VK_FORMAT_R16G16_SINT:
            return PAL_FORMAT_R16G16_SINT;

        case VK_FORMAT_R16G16_SFLOAT:
            return PAL_FORMAT_R16G16_SFLOAT;

        case VK_FORMAT_R32G32_UINT:
            return PAL_FORMAT_R32G32_UINT;

        case VK_FORMAT_R32G32_SINT:
            return PAL_FORMAT_R32G32_SINT;

        case VK_FORMAT_R32G32_SFLOAT:
            return PAL_FORMAT_R32G32_SFLOAT;

        case VK_FORMAT_R64G64_UINT:
            return PAL_FORMAT_R64G64_UINT;

        case VK_FORMAT_R64G64_SINT:
            return PAL_FORMAT_R64G64_SINT;

        case VK_FORMAT_R64G64_SFLOAT:
            return PAL_FORMAT_R64G64_SFLOAT;

        case VK_FORMAT_R8G8B8_UNORM:
            return PAL_FORMAT_R8G8B8_UNORM;

        case VK_FORMAT_R8G8B8_SNORM:
            return PAL_FORMAT_R8G8B8_SNORM;

        case VK_FORMAT_R8G8B8_UINT:
            return PAL_FORMAT_R8G8B8_UINT;

        case VK_FORMAT_R8G8B8_SINT:
            return PAL_FORMAT_R8G8B8_SINT;

        case VK_FORMAT_R8G8B8_SRGB:
            return PAL_FORMAT_R8G8B8_SRGB;

        case VK_FORMAT_R16G16B16_UNORM:
            return PAL_FORMAT_R16G16B16_UNORM;

        case VK_FORMAT_R16G16B16_SNORM:
            return PAL_FORMAT_R16G16B16_SNORM;

        case VK_FORMAT_R16G16B16_UINT:
            return PAL_FORMAT_R16G16B16_UINT;

        case VK_FORMAT_R16G16B16_SINT:
            return PAL_FORMAT_R16G16B16_SINT;

        case VK_FORMAT_R16G16B16_SFLOAT:
            return PAL_FORMAT_R16G16B16_SFLOAT;

        case VK_FORMAT_R32G32B32_UINT:
            return PAL_FORMAT_R32G32B32_UINT;

        case VK_FORMAT_R32G32B32_SINT:
            return PAL_FORMAT_R32G32B32_SINT;

        case VK_FORMAT_R32G32B32_SFLOAT:
            return PAL_FORMAT_R32G32B32_SFLOAT;

        case VK_FORMAT_R64G64B64_UINT:
            return PAL_FORMAT_R64G64B64_UINT;

        case VK_FORMAT_R64G64B64_SINT:
            return PAL_FORMAT_R64G64B64_SINT;

        case VK_FORMAT_R64G64B64_SFLOAT:
            return PAL_FORMAT_R64G64B64_SFLOAT;

        case VK_FORMAT_B8G8R8_UNORM:
            return PAL_FORMAT_B8G8R8_UNORM;

        case VK_FORMAT_B8G8R8_SNORM:
            return PAL_FORMAT_B8G8R8_SNORM;

        case VK_FORMAT_B8G8R8_UINT:
            return PAL_FORMAT_B8G8R8_UINT;

        case VK_FORMAT_B8G8R8_SINT:
            return PAL_FORMAT_B8G8R8_SINT;

        case VK_FORMAT_B8G8R8_SRGB:
            return PAL_FORMAT_B8G8R8_SRGB;

        case VK_FORMAT_R8G8B8A8_UNORM:
            return PAL_FORMAT_R8G8B8A8_UNORM;

        case VK_FORMAT_R8G8B8A8_SNORM:
            return PAL_FORMAT_R8G8B8A8_SNORM;

        case VK_FORMAT_R8G8B8A8_UINT:
            return PAL_FORMAT_R8G8B8A8_UINT;

        case VK_FORMAT_R8G8B8A8_SINT:
            return PAL_FORMAT_R8G8B8A8_SINT;

        case VK_FORMAT_R8G8B8A8_SRGB:
            return PAL_FORMAT_R8G8B8A8_SRGB;

        case VK_FORMAT_R16G16B16A16_UNORM:
            return PAL_FORMAT_R16G16B16A16_UNORM;

        case VK_FORMAT_R16G16B16A16_SNORM:
            return PAL_FORMAT_R16G16B16A16_SNORM;

        case VK_FORMAT_R16G16B16A16_UINT:
            return PAL_FORMAT_R16G16B16A16_UINT;

        case VK_FORMAT_R16G16B16A16_SINT:
            return PAL_FORMAT_R16G16B16A16_SINT;

        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return PAL_FORMAT_R16G16B16A16_SFLOAT;

        case VK_FORMAT_R32G32B32A32_UINT:
            return PAL_FORMAT_R32G32B32A32_UINT;

        case VK_FORMAT_R32G32B32A32_SINT:
            return PAL_FORMAT_R32G32B32A32_SINT;

        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return PAL_FORMAT_R32G32B32A32_SFLOAT;

        case VK_FORMAT_R64G64B64A64_UINT:
            return PAL_FORMAT_R64G64B64A64_UINT;

        case VK_FORMAT_R64G64B64A64_SINT:
            return PAL_FORMAT_R64G64B64A64_SINT;

        case VK_FORMAT_R64G64B64A64_SFLOAT:
            return PAL_FORMAT_R64G64B64A64_SFLOAT;

        case VK_FORMAT_B8G8R8A8_UNORM:
            return PAL_FORMAT_B8G8R8A8_UNORM;

        case VK_FORMAT_B8G8R8A8_SNORM:
            return PAL_FORMAT_B8G8R8A8_SNORM;

        case VK_FORMAT_B8G8R8A8_UINT:
            return PAL_FORMAT_B8G8R8A8_UINT;

        case VK_FORMAT_B8G8R8A8_SINT:
            return PAL_FORMAT_B8G8R8A8_SINT;

        case VK_FORMAT_B8G8R8A8_SRGB:
            return PAL_FORMAT_B8G8R8A8_SRGB;

        case VK_FORMAT_S8_UINT:
            return PAL_FORMAT_S8_UINT;

        case VK_FORMAT_D16_UNORM:
            return PAL_FORMAT_D16_UNORM;

        case VK_FORMAT_D32_SFLOAT:
            return PAL_FORMAT_D32_SFLOAT;

        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return PAL_FORMAT_D32_SFLOAT_S8_UINT;

        case VK_FORMAT_D16_UNORM_S8_UINT:
            return PAL_FORMAT_D16_UNORM_S8_UINT;

        case VK_FORMAT_D24_UNORM_S8_UINT:
            return PAL_FORMAT_D24_UNORM_S8_UINT;
    }

    return PAL_FORMAT_UNDEFINED;
}

static PalImageUsages ImageUsageFromVk(VkFormatFeatureFlags flags)
{
    PalImageUsages usages = 0;
    if (flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) {
        usages |= PAL_IMAGE_USAGE_COLOR_ATTACHEMENT;
    }

    if (flags & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        usages |= PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT;
    }

    if (flags & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT) {
        usages |= PAL_IMAGE_USAGE_TRANSFER_SRC;
    }

    if (flags & VK_FORMAT_FEATURE_TRANSFER_DST_BIT) {
        usages |= PAL_IMAGE_USAGE_TRANSFER_DST;
    }

    if (flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) {
        usages |= VK_IMAGE_USAGE_STORAGE_BIT;
    }

    if (flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
        usages |= PAL_IMAGE_USAGE_SAMPLED;
    }

    return usages;
}

static VkImageViewType imageViewTypeToVk(PalImageViewType type)
{
    switch (type) {
        case PAL_IMAGE_VIEW_TYPE_1D:
            return VK_IMAGE_VIEW_TYPE_1D;

        case PAL_IMAGE_VIEW_TYPE_1D_ARRAY:
            return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

        case PAL_IMAGE_VIEW_TYPE_2D:
            return VK_IMAGE_VIEW_TYPE_2D;

        case PAL_IMAGE_VIEW_TYPE_2D_ARRAY:
            return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

        case PAL_IMAGE_VIEW_TYPE_3D:
            return VK_IMAGE_VIEW_TYPE_3D;

        case PAL_IMAGE_VIEW_TYPE_CUBE:
            return VK_IMAGE_VIEW_TYPE_CUBE;

        case PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY:
            return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    }

    return VK_IMAGE_VIEW_TYPE_2D;
}

static VkExtent2D getShadingRateSizeVk(PalFragmentShadingRate rate)
{
    switch (rate) {
        case PAL_FRAGMENT_SHADING_RATE_1X1:
            return (VkExtent2D){1, 1};

        case PAL_FRAGMENT_SHADING_RATE_1X2:
            return (VkExtent2D){1, 2};

        case PAL_FRAGMENT_SHADING_RATE_2X1:
            return (VkExtent2D){2, 1};

        case PAL_FRAGMENT_SHADING_RATE_2X2:
            return (VkExtent2D){2, 2};

        case PAL_FRAGMENT_SHADING_RATE_2X4:
            return (VkExtent2D){2, 4};

        case PAL_FRAGMENT_SHADING_RATE_4X2:
            return (VkExtent2D){4, 2};

        case PAL_FRAGMENT_SHADING_RATE_4X4:
            return (VkExtent2D){4, 4};
    }

    return (VkExtent2D){0, 0};
}

static VkFormat vertexTypeToVk(PalVertexType type)
{
    switch (type) {
        case PAL_VERTEX_TYPE_INT32:
            return VK_FORMAT_R32_SINT;

        case PAL_VERTEX_TYPE_INT32_2:
            return VK_FORMAT_R32G32_SINT;

        case PAL_VERTEX_TYPE_INT32_3:
            return VK_FORMAT_R32G32B32_SINT;

        case PAL_VERTEX_TYPE_INT32_4:
            return VK_FORMAT_R32G32B32A32_SINT;

        case PAL_VERTEX_TYPE_UINT32:
            return VK_FORMAT_R32_UINT;

        case PAL_VERTEX_TYPE_UINT32_2:
            return VK_FORMAT_R32G32_UINT;

        case PAL_VERTEX_TYPE_UINT32_3:
            return VK_FORMAT_R32G32B32_UINT;

        case PAL_VERTEX_TYPE_UINT32_4:
            return VK_FORMAT_R32G32B32A32_UINT;

        case PAL_VERTEX_TYPE_INT8_2:
            return VK_FORMAT_R8G8_SINT;

        case PAL_VERTEX_TYPE_INT8_4:
            return VK_FORMAT_R8G8B8A8_SINT;

        case PAL_VERTEX_TYPE_UINT8_2:
            return VK_FORMAT_R8G8_UINT;

        case PAL_VERTEX_TYPE_UINT8_4:
            return VK_FORMAT_R8G8B8A8_UINT;

        case PAL_VERTEX_TYPE_INT8_2NORM:
            return VK_FORMAT_R8G8_SNORM;

        case PAL_VERTEX_TYPE_INT8_4NORM:
            return VK_FORMAT_R8G8B8A8_SNORM;

        case PAL_VERTEX_TYPE_UINT8_2NORM:
            return VK_FORMAT_R8G8_UNORM;

        case PAL_VERTEX_TYPE_UINT8_4NORM:
            return VK_FORMAT_R8G8B8A8_UNORM;

        case PAL_VERTEX_TYPE_INT16_2:
            return VK_FORMAT_R16G16_SINT;

        case PAL_VERTEX_TYPE_INT16_4:
            return VK_FORMAT_R16G16B16A16_SINT;

        case PAL_VERTEX_TYPE_UINT16_2:
            return VK_FORMAT_R16G16_UINT;

        case PAL_VERTEX_TYPE_UINT16_4:
            return VK_FORMAT_R16G16B16A16_UINT;

        case PAL_VERTEX_TYPE_INT16_2NORM:
            return VK_FORMAT_R16G16_SNORM;

        case PAL_VERTEX_TYPE_INT16_4NORM:
            return VK_FORMAT_R16G16B16A16_SNORM;

        case PAL_VERTEX_TYPE_UINT16_2NORM:
            return VK_FORMAT_R16G16_UNORM;

        case PAL_VERTEX_TYPE_UINT16_4NORM:
            return VK_FORMAT_R16G16B16A16_UNORM;

        case PAL_VERTEX_TYPE_FLOAT:
            return VK_FORMAT_R32_SFLOAT;

        case PAL_VERTEX_TYPE_FLOAT2:
            return VK_FORMAT_R32G32_SFLOAT;

        case PAL_VERTEX_TYPE_FLOAT3:
            return VK_FORMAT_R32G32B32_SFLOAT;

        case PAL_VERTEX_TYPE_FLOAT4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;

        case PAL_VERTEX_TYPE_HALF_FLOAT16_2:
            return VK_FORMAT_R16G16_SFLOAT;

        case PAL_VERTEX_TYPE_HALF_FLOAT16_4:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
    }

    return VK_FORMAT_UNDEFINED;
}




static VkStencilOp stencilOpToVk(PalStencilOp op)
{
    switch (op) {
        case PAL_STENCIL_OP_KEEP:
            return VK_STENCIL_OP_KEEP;

        case PAL_STENCIL_OP_ZERO:
            return VK_STENCIL_OP_ZERO;

        case PAL_STENCIL_OP_REPLACE:
            return VK_STENCIL_OP_REPLACE;

        case PAL_STENCIL_OP_INCREMENT_AND_CLAMP:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;

        case PAL_STENCIL_OP_DECREMENT_AND_CLAMP:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;

        case PAL_STENCIL_OP_INVERT:
            return VK_STENCIL_OP_INVERT;

        case PAL_STENCIL_OP_INCREMENT_AND_WRAP:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;

        case PAL_STENCIL_OP_DECREMENT_AND_WRAP:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    }

    return VK_STENCIL_OP_KEEP;
}

static VkCompareOp compareOpToVk(PalCompareOp op)
{
    switch (op) {
        case PAL_COMPARE_OP_NEVER:
            return VK_COMPARE_OP_NEVER;

        case PAL_COMPARE_OP_LESS:
            return VK_COMPARE_OP_LESS;

        case PAL_COMPARE_OP_EQUAL:
            return VK_COMPARE_OP_EQUAL;

        case PAL_COMPARE_OP_LESS_OR_EQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;

        case PAL_COMPARE_OP_GREATER:
            return VK_COMPARE_OP_GREATER;

        case PAL_COMPARE_OP_NOT_EQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;

        case PAL_COMPARE_OP_GREATER_OR_EQUAL:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;

        case PAL_COMPARE_OP_ALWAYS:
            return VK_COMPARE_OP_ALWAYS;
    }

    return VK_COMPARE_OP_NEVER;
}





static VkResolveModeFlags resolveModeToVk(PalResolveMode mode)
{
    switch (mode) {
        case PAL_RESOLVE_MODE_SAMPLE_ZERO:
            return VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR;

        case PAL_RESOLVE_MODE_AVERAGE:
            return VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR;

        case PAL_RESOLVE_MODE_MIN:
            return VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR;

        case PAL_RESOLVE_MODE_MAX:
            return VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR;
    }

    return VK_RESOLVE_MODE_NONE_KHR;
}
     
static VkFilter filterToVk(PalFilterMode mode)
{
    switch (mode) {
        case PAL_FILTER_MODE_NEAREST: {
            return VK_FILTER_NEAREST;
        }

        case PAL_FILTER_MODE_LINEAR: {
            return VK_FILTER_LINEAR;
        }
    }
    return VK_FILTER_NEAREST;
}

static VkSamplerMipmapMode mipmapModeToVk(PalSamplerMipmapMode mode)
{
    switch (mode) {
        case PAL_SAMPLER_MIPMAP_MODE_NEAREST: {
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        }

        case PAL_SAMPLER_MIPMAP_MODE_LINEAR: {
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }
    }
    return VK_SAMPLER_MIPMAP_MODE_NEAREST;
}

static VkSamplerAddressMode addressModeToVk(PalSamplerAddressMode mode)
{
    switch (mode) {
        case PAL_SAMPLER_ADDRESS_MODE_REPEAT: {
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }

        case PAL_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT: {
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

        }
        case PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE: {
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

        }
        case PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER: {
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        }
    }
    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

static VkBorderColor borderColorToVk(PalBorderColor color)
{
    switch (color) {
        case PAL_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK: {
            return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        }

        case PAL_BORDER_COLOR_INT_TRANSPARENT_BLACK: {
            return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        }

        case PAL_BORDER_COLOR_FLOAT_OPAQUE_BLACK: {
            return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        }

        case PAL_BORDER_COLOR_INT_OPAQUE_BLACK: {
            return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        }

        case PAL_BORDER_COLOR_FLOAT_OPAQUE_WHITE: {
            return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        }

        case PAL_BORDER_COLOR_INT_OPAQUE_WHITE: {
            return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
        }
    }
    return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
}

static Barrier barrierToVk(
    uint32_t stageCount,
    PalUsageState state,
    PalShaderStage* shaderStages)
{
    Barrier barrier = {0};
    switch (state) {
        case PAL_USAGE_STATE_UNDEFINED: {
            barrier.stages = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT_KHR;
            barrier.access = 0;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_PRESENT: {
            barrier.stages = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT_KHR;
            barrier.access = 0;
            barrier.layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            return barrier;
        }

        case PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE: {
            barrier.stages = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;
            barrier.access = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_DEPTH_ATTACHMENT_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR;
            barrier.stages |= VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR;
            barrier.access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_DEPTH_ATTACHMENT_WRITE: {
            barrier.stages = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR;
            barrier.stages |= VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR;
            barrier.access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_STENCIL_ATTACHMENT_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR;
            barrier.stages |= VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR;
            barrier.access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_STENCIL_ATTACHMENT_WRITE: {
            barrier.stages = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR;
            barrier.stages |= VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR;
            barrier.access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_FRAGMENT_SHADING_RATE_ATTACHMENT_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
            barrier.access = VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
            return barrier;
        }

        case PAL_USAGE_STATE_TRANSFER_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR;
            barrier.access = VK_ACCESS_2_TRANSFER_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_TRANSFER_WRITE: {
            barrier.stages = VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR;
            barrier.access = VK_ACCESS_2_TRANSFER_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_VERTEX_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT_KHR;
            barrier.access = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_INDEX_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT_KHR;
            barrier.access = VK_ACCESS_2_INDEX_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_INDIRECT_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT_KHR;
            barrier.access = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_UNIFORM_READ: {
            for (int i = 0; i < stageCount; i++) {
                barrier.stages |= pipelineStageToVk(shaderStages[i]);
            }

            if (barrier.stages == 0) {
                barrier.stages = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
            }
            
            barrier.access = VK_ACCESS_2_UNIFORM_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_SHADER_READ: {
            for (int i = 0; i < stageCount; i++) {
                barrier.stages |= pipelineStageToVk(shaderStages[i]);
            }

            if (barrier.stages == 0) {
                barrier.stages = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
            }

            barrier.access = VK_ACCESS_2_SHADER_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_SHADER_WRITE: {
            for (int i = 0; i < stageCount; i++) {
                barrier.stages |= pipelineStageToVk(shaderStages[i]);
            }

            if (barrier.stages == 0) {
                barrier.stages = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
            }

            barrier.access = VK_ACCESS_2_SHADER_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_GENERAL;
            return barrier;
        }

        case PAL_USAGE_STATE_STORAGE_READ: {
            for (int i = 0; i < stageCount; i++) {
                barrier.stages |= pipelineStageToVk(shaderStages[i]);
            }

            if (barrier.stages == 0) {
                barrier.stages = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
            }

            barrier.access = VK_ACCESS_2_SHADER_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_GENERAL;
            return barrier;
        }

        case PAL_USAGE_STATE_STORAGE_WRITE: {
            for (int i = 0; i < stageCount; i++) {
                barrier.stages |= pipelineStageToVk(shaderStages[i]);
            }

            if (barrier.stages == 0) {
                barrier.stages = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
            }

            barrier.access = VK_ACCESS_2_SHADER_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_GENERAL;
            return barrier;
        }

        case PAL_USAGE_STATE_HOST_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_HOST_BIT_KHR;
            barrier.access = VK_ACCESS_2_HOST_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_HOST_WRITE: {
            barrier.stages = VK_PIPELINE_STAGE_2_HOST_BIT_KHR;
            barrier.access = VK_ACCESS_2_HOST_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
            barrier.access = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;

            // HACK: small performance lost for the case of a single scratch buffer used for
            // BLAS and TLAS builds.
            barrier.access |= VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;

            for (int i = 0; i < stageCount; i++) {
                if (i == 0) {
                    barrier.stages = 0;
                }

                barrier.stages |= pipelineStageToVk(shaderStages[i]);
                barrier.access = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
            }

            if (barrier.stages == 0) {
                barrier.stages = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
            }

            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE: {
            barrier.stages = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
            barrier.access = VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }
    }

    barrier.stages = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT_KHR;
    barrier.access = 0;
    barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
    return barrier;
}

static VkDescriptorType descriptortypeToVk(PalDescriptorType type)
{
    switch (type) {
        case PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        case PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        case PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

        case PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

        case PAL_DESCRIPTOR_TYPE_SAMPLER:
            return VK_DESCRIPTOR_TYPE_SAMPLER;

        case PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE:
            return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    }

    return 0;
}

static VkShaderStageFlags shaderStageToVK(PalShaderStage stage)
{
    switch (stage) {
        case PAL_SHADER_STAGE_VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;

        case PAL_SHADER_STAGE_FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;

        case PAL_SHADER_STAGE_COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;

        case PAL_SHADER_STAGE_GEOMETRY:
            return VK_SHADER_STAGE_GEOMETRY_BIT;

        case PAL_SHADER_STAGE_MESH:
            return VK_SHADER_STAGE_MESH_BIT_EXT;

        case PAL_SHADER_STAGE_TASK:
            return VK_SHADER_STAGE_TASK_BIT_EXT;

        case PAL_SHADER_STAGE_TESSELLATION_CONTROL:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

        case PAL_SHADER_STAGE_TESSELLATION_EVALUATION:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

        case PAL_SHADER_STAGE_RAYGEN:
            return VK_SHADER_STAGE_RAYGEN_BIT_KHR;

        case PAL_SHADER_STAGE_CLOSEST_HIT:
            return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

        case PAL_SHADER_STAGE_ANY_HIT:
            return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

        case PAL_SHADER_STAGE_MISS:
            return VK_SHADER_STAGE_MISS_BIT_KHR;

        case PAL_SHADER_STAGE_INTERSECTION:
            return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;

        case PAL_SHADER_STAGE_CALLABLE:
            return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
    }

    return 0;
}

static inline void* alignedRealloc(
    void* memory,
    uint64_t size,
    uint64_t alignment)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    return _aligned_realloc(memory, size, alignment);
#else
    return realloc(memory, size);
#endif // _MSC_VER
}

static inline void alignedFree(void* ptr)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    _aligned_free(ptr);
#else
    free(ptr);
#endif // _MSC_VER
}

static void* VKAPI_CALL allocateVk(
    void* pUserData,
    size_t size,
    size_t alignment,
    VkSystemAllocationScope allocationScope)
{
    return palAllocate(s_Vk.allocator, size, alignment);
}

static void VKAPI_CALL freeVk(
    void* pUserData,
    void* ptr)
{
    palFree(s_Vk.allocator, ptr);
}

static void* VKAPI_CALL reallocVk(
    void* pUserData,
    void* pOriginal,
    size_t size,
    size_t alignment,
    VkSystemAllocationScope allocationScope)
{
    // Note: This is a hack which could cost performance but
    // realloc is not really called that much so it should be fine
    // this is because we dont know the old size
    void* block = alignedRealloc(pOriginal, size, alignment);
    if (block) {
        void* memory = palAllocate(s_Vk.allocator, size, alignment);
        if (!memory) {
            alignedFree(block);
            return nullptr;
        }

        memcpy(memory, block, size);
        alignedFree(block);
        return memory;
    }
    return nullptr;
}

VkBool32 VKAPI_CALL debugCallbackVk(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagBitsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void* userData)
{
    if (!s_Vk.callback) {
        return VK_FALSE;
    }

    PalDebugMessageSeverity debugSeverity = 0;
    PalDebugMessageType debugType = 0;
    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        debugType = PAL_DEBUG_MESSAGE_TYPE_GENERAL;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        debugType = PAL_DEBUG_MESSAGE_TYPE_PERFORMANCE;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        debugType = PAL_DEBUG_MESSAGE_TYPE_VALIDATION;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        debugSeverity = PAL_DEBUG_MESSAGE_SEVERITY_INFO;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        debugSeverity = PAL_DEBUG_MESSAGE_SEVERITY_WARNING;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        debugSeverity = PAL_DEBUG_MESSAGE_SEVERITY_ERROR;
    }

    s_Vk.callback(userData, debugSeverity, debugType, data->pMessage);
    return VK_FALSE;
}

static uint32_t findBestMemoryIndexVk(
    VkPhysicalDevice phyDevice,
    uint32_t memoryMask)
{
    int bestScore = -1;
    uint32_t bestIndex = UINT32_MAX;
    VkPhysicalDeviceMemoryProperties memProps = {0};
    s_Vk.getPhysicalDeviceMemoryProperties(phyDevice, &memProps);

    for (int i = 0; i < memProps.memoryTypeCount; i++) {
        if (!(memoryMask & (1u << i))) {
            continue;
        }

        int score = 0;
        VkMemoryPropertyFlags flags = memProps.memoryTypes[i].propertyFlags;

        // GPU only memory
        if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            score += 100;
        }

        // CPU memory
        if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            score += 50;
        }

        if (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
            score += 25;
        }

        if (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
            score += 10;
        }

        if (score > bestScore) {
            bestIndex = i;
        }
    }

    return bestIndex;
}

static inline uint32_t alignVk(
    uint32_t value,
    uint32_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

static inline uint32_t maxVk(
    uint32_t a,
    uint32_t b)
{
    return (a > b) ? a : b;
}

static inline uint32_t minVk(
    uint32_t a,
    uint32_t b)
{
    return (a < b) ? a : b;
}

static void fillBuildInfoVk(
    uint32_t count,
    PalAccelerationStructureBuildInfo* info,
    uint32_t* maxPrimities,
    VkAccelerationStructureGeometryKHR* geometries,
    VkAccelerationStructureKHR srcAs,
    VkAccelerationStructureKHR dstAs,
    VkAccelerationStructureBuildRangeInfoKHR* rangeInfos,
    VkAccelerationStructureBuildGeometryInfoKHR* buildInfo)
{
    for (int i = 0; i < count; i++) {
        // fill vulkan geometry struct
        VkAccelerationStructureGeometryKHR* tmp = &geometries[i];
        tmp->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        tmp->flags = 0;

        if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL) {
            tmp->geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
            VkAccelerationStructureGeometryInstancesDataKHR* data = nullptr;
            data = &tmp->geometry.instances;
            data->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
            data->arrayOfPointers = PAL_FALSE;

            VkDeviceOrHostAddressConstKHR address = {0};
            address.deviceAddress = info->instanceBufferAddress;
            data->data = address;

            if (maxPrimities) {
                maxPrimities[i] = info->instanceCount;
            }

            // range info
            if (rangeInfos) {
                VkAccelerationStructureBuildRangeInfoKHR* rangeInfo = &rangeInfos[i];
                rangeInfo->primitiveCount = info->instanceCount;
                rangeInfo->firstVertex = 0;     // PAL does not allow setting this
                rangeInfo->primitiveOffset = 0; // PAL does not allow setting this
                rangeInfo->transformOffset = 0; // PAL does not allow setting this
            }
            break;

        } else {
            if (info->geometries[i].flags & PAL_GEOMETRY_FLAG_OPAQUE) {
                tmp->flags |= VK_GEOMETRY_OPAQUE_BIT_KHR;
            }

            if (info->geometries[i].flags & PAL_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT) {
                tmp->flags |= VK_GEOMETRY_OPAQUE_BIT_KHR;
            }

            if (maxPrimities) {
                maxPrimities[i] = info->geometries[i].primitiveCount;
            }

            // range info
            if (rangeInfos) {
                VkAccelerationStructureBuildRangeInfoKHR* rangeInfo = &rangeInfos[i];
                rangeInfo->primitiveCount = info->geometries[i].primitiveCount;
                rangeInfo->firstVertex = 0;     // PAL does not allow setting this
                rangeInfo->primitiveOffset = 0; // PAL does not allow setting this
                rangeInfo->transformOffset = 0; // PAL does not allow setting this
            }
        }

        if (info->geometries[i].type == PAL_GEOMETRY_TYPE_TRIANGLE) {
            tmp->geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
            VkAccelerationStructureGeometryTrianglesDataKHR* data = &tmp->geometry.triangles;
            data->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;

            VkDeviceOrHostAddressConstKHR vertexAddress = {0};
            VkDeviceOrHostAddressConstKHR indexAddress = {0};
            PalGeometryDataTriangle* tmpData = info->geometries[i].data;

            vertexAddress.deviceAddress = tmpData->vertexBufferAddress;
            data->vertexData = vertexAddress;
            data->maxVertex = tmpData->vertexCount - 1;
            data->vertexFormat = vertexTypeToVk(tmpData->vertexType);
            data->vertexStride = tmpData->vertexStride;

            indexAddress.deviceAddress = tmpData->indexBufferAddress;
            data->indexData = indexAddress;
            if (tmpData->indexType == PAL_INDEX_TYPE_UINT32) {
                data->indexType = VK_INDEX_TYPE_UINT32;
            } else {
                data->indexType = VK_INDEX_TYPE_UINT16;
            }

            // set to none if there is no index buffer address
            if (!tmpData->indexBufferAddress) {
                data->indexType = VK_INDEX_TYPE_NONE_KHR;
            }

        } else if (info->geometries[i].type == PAL_GEOMETRY_TYPE_AABBS) {
            tmp->geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
            VkAccelerationStructureGeometryAabbsDataKHR* data = &tmp->geometry.aabbs;
            data->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;

            VkDeviceOrHostAddressConstKHR address = {0};
            PalGeometryDataAABBS* tmpData = info->geometries[i].data;
            address.deviceAddress = tmpData->bufferAddress;
            data->data = address;
            data->stride = tmpData->stride;
        }
    }

    buildInfo->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        buildInfo->type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    } else {
        buildInfo->type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    }

    // build mode
    if (info->buildMode == PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD) {
        buildInfo->mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    } else {
        buildInfo->mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
    }

    // build hints
    buildInfo->flags = 0;
    if (info->buildHints & PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD) {
        buildInfo->flags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;
    }

    if (info->buildHints & PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_TRACE) {
        buildInfo->flags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    }

    if (info->buildHints & PAL_ACCELERATION_STRUCTURE_BUILD_HINT_LOW_MEMORY) {
        buildInfo->flags |= VK_BUILD_ACCELERATION_STRUCTURE_LOW_MEMORY_BIT_KHR;
    }

    buildInfo->geometryCount = count;
    buildInfo->srcAccelerationStructure = srcAs;
    buildInfo->dstAccelerationStructure = dstAs;
    buildInfo->pGeometries = geometries;

    VkDeviceOrHostAddressKHR scratchData = {0};
    scratchData.deviceAddress = info->scratchBufferAddress;
    buildInfo->scratchData = scratchData;
}



static VkImageAspectFlags imageAspectToVk(PalImageAspect aspect)
{
    switch (aspect) {
        case PAL_IMAGE_ASPECT_COLOR:
            return VK_IMAGE_ASPECT_COLOR_BIT;

        case PAL_IMAGE_ASPECT_DEPTH:
            return VK_IMAGE_ASPECT_DEPTH_BIT;

        case PAL_IMAGE_ASPECT_STENCIL:
            return VK_IMAGE_ASPECT_STENCIL_BIT;

        case PAL_IMAGE_ASPECT_DEPTH_STENCIL:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    return VK_IMAGE_ASPECT_COLOR_BIT;
}

// ==================================================
// Adapter
// ==================================================

PalResult PAL_CALL initGraphicsVk(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator)
{
    // load vulkan
    s_Vk.handle = loadLibrary(VK_LIB_NAME);
    if (!s_Vk.handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // clang-format off
    s_Vk.enumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)loadProc(
        s_Vk.handle,
        "vkEnumerateInstanceVersion");

    s_Vk.enumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)loadProc(
        s_Vk.handle,
        "vkEnumerateInstanceExtensionProperties");

    s_Vk.createInstance = (PFN_vkCreateInstance)loadProc(
        s_Vk.handle,
        "vkCreateInstance");

    s_Vk.destroyInstance = (PFN_vkDestroyInstance)loadProc(
        s_Vk.handle,
        "vkDestroyInstance");

    s_Vk.enumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)loadProc(
        s_Vk.handle,
        "vkEnumeratePhysicalDevices");

    s_Vk.getPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceProperties");

    s_Vk.getPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceMemoryProperties");

    s_Vk.enumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)loadProc(
        s_Vk.handle,
        "vkEnumerateInstanceLayerProperties");

    s_Vk.getPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceQueueFamilyProperties");

    s_Vk.enumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)loadProc(
        s_Vk.handle,
        "vkEnumerateDeviceExtensionProperties");

    s_Vk.getPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceFeatures");

    s_Vk.getPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceFeatures2");

    s_Vk.getInstanceProcAddr = (PFN_vkGetInstanceProcAddr)loadProc(
        s_Vk.handle,
        "vkGetInstanceProcAddr");

    s_Vk.createImage = (PFN_vkCreateImage)loadProc(
        s_Vk.handle,
        "vkCreateImage");

    s_Vk.destroyImage = (PFN_vkDestroyImage)loadProc(
        s_Vk.handle,
        "vkDestroyImage");

    s_Vk.createImageView = (PFN_vkCreateImageView)loadProc(
        s_Vk.handle,
        "vkCreateImageView");

    s_Vk.destroyImageView = (PFN_vkDestroyImageView)loadProc(
        s_Vk.handle,
        "vkDestroyImageView");

    s_Vk.createShader = (PFN_vkCreateShaderModule)loadProc(
        s_Vk.handle,
        "vkCreateShaderModule");

    s_Vk.destroyShader = (PFN_vkDestroyShaderModule)loadProc(
        s_Vk.handle,
        "vkDestroyShaderModule");

    s_Vk.createSampler = (PFN_vkCreateSampler)loadProc(
        s_Vk.handle,
        "vkCreateSampler");

    s_Vk.destroySampler = (PFN_vkDestroySampler)loadProc(
        s_Vk.handle,
        "vkDestroySampler");

    s_Vk.getPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceProperties2");

    s_Vk.getPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceFormatProperties");

    s_Vk.getPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceImageFormatProperties");

    s_Vk.createDevice = (PFN_vkCreateDevice)loadProc(
        s_Vk.handle,
        "vkCreateDevice");

    s_Vk.destroyDevice = (PFN_vkDestroyDevice)loadProc(
        s_Vk.handle,
        "vkDestroyDevice");

    s_Vk.getDeviceQueue = (PFN_vkGetDeviceQueue)loadProc(
        s_Vk.handle,
        "vkGetDeviceQueue");

    s_Vk.queueSubmit = (PFN_vkQueueSubmit)loadProc(
        s_Vk.handle,
        "vkQueueSubmit");

    s_Vk.getDeviceProcAddr = (PFN_vkGetDeviceProcAddr)loadProc(
        s_Vk.handle,
        "vkGetDeviceProcAddr");

    s_Vk.getImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)loadProc(
        s_Vk.handle,
        "vkGetImageMemoryRequirements");

    s_Vk.allocateMemory = (PFN_vkAllocateMemory)loadProc(
        s_Vk.handle,
        "vkAllocateMemory");

    s_Vk.freeMemory = (PFN_vkFreeMemory)loadProc(
        s_Vk.handle,
        "vkFreeMemory");

    s_Vk.bindImageMemory = (PFN_vkBindImageMemory)loadProc(
        s_Vk.handle,
        "vkBindImageMemory");

    s_Vk.createCommandPool = (PFN_vkCreateCommandPool)loadProc(
        s_Vk.handle,
        "vkCreateCommandPool");

    s_Vk.destroyCommandPool = (PFN_vkDestroyCommandPool)loadProc(
        s_Vk.handle,
        "vkDestroyCommandPool");

    s_Vk.allocateCommandBuffer = (PFN_vkAllocateCommandBuffers)loadProc(
        s_Vk.handle,
        "vkAllocateCommandBuffers");

    s_Vk.freeCommandBuffer = (PFN_vkFreeCommandBuffers)loadProc(
        s_Vk.handle,
        "vkFreeCommandBuffers");

    s_Vk.createFence = (PFN_vkCreateFence)loadProc(
        s_Vk.handle,
        "vkCreateFence");

    s_Vk.destroyFence = (PFN_vkDestroyFence)loadProc(
        s_Vk.handle,
        "vkDestroyFence");

    s_Vk.resetFence = (PFN_vkResetFences)loadProc(
        s_Vk.handle,
        "vkResetFences");

    s_Vk.waitFence = (PFN_vkWaitForFences)loadProc(
        s_Vk.handle,
        "vkWaitForFences");

    s_Vk.isFenceSignaled = (PFN_vkGetFenceStatus)loadProc(
        s_Vk.handle,
        "vkGetFenceStatus");

    s_Vk.createSemaphore = (PFN_vkCreateSemaphore)loadProc(
        s_Vk.handle,
        "vkCreateSemaphore");

    s_Vk.destroySemaphore = (PFN_vkDestroySemaphore)loadProc(
        s_Vk.handle,
        "vkDestroySemaphore");

    s_Vk.cmdBegin = (PFN_vkBeginCommandBuffer)loadProc(
        s_Vk.handle,
        "vkBeginCommandBuffer");

    s_Vk.cmdEnd = (PFN_vkEndCommandBuffer)loadProc(
        s_Vk.handle,
        "vkEndCommandBuffer");

    s_Vk.resetCommandPool = (PFN_vkResetCommandPool)loadProc(
        s_Vk.handle,
        "vkResetCommandPool");

    s_Vk.resetCommandBuffer = (PFN_vkResetCommandBuffer)loadProc(
        s_Vk.handle,
        "vkResetCommandBuffer");

    s_Vk.cmdExecuteCommandBuffer = (PFN_vkCmdExecuteCommands)loadProc(
        s_Vk.handle,
        "vkCmdExecuteCommands");

    s_Vk.cmdCopyBuffer = (PFN_vkCmdCopyBuffer)loadProc(
        s_Vk.handle,
        "vkCmdCopyBuffer");

    s_Vk.cmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)loadProc(
        s_Vk.handle,
        "vkCmdCopyBufferToImage");

    s_Vk.cmdCopyImage = (PFN_vkCmdCopyImage)loadProc(
        s_Vk.handle,
        "vkCmdCopyImage");

    s_Vk.cmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)loadProc(
        s_Vk.handle,
        "vkCmdCopyImageToBuffer");

    s_Vk.cmdBindPipeline = (PFN_vkCmdBindPipeline)loadProc(
        s_Vk.handle,
        "vkCmdBindPipeline");

    s_Vk.cmdSetViewports = (PFN_vkCmdSetViewport)loadProc(
        s_Vk.handle,
        "vkCmdSetViewport");

    s_Vk.cmdSetScissors = (PFN_vkCmdSetScissor)loadProc(
        s_Vk.handle,
        "vkCmdSetScissor");

    s_Vk.cmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)loadProc(
        s_Vk.handle,
        "vkCmdBindVertexBuffers");

    s_Vk.cmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)loadProc(
        s_Vk.handle,
        "vkCmdBindIndexBuffer");

    s_Vk.cmdDraw = (PFN_vkCmdDraw)loadProc(
        s_Vk.handle,
        "vkCmdDraw");

    s_Vk.cmdDrawIndirect = (PFN_vkCmdDrawIndirect)loadProc(
        s_Vk.handle,
        "vkCmdDrawIndirect");

    s_Vk.cmdDrawIndexed = (PFN_vkCmdDrawIndexed)loadProc(
        s_Vk.handle,
        "vkCmdDrawIndexed");

    s_Vk.cmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect)loadProc(
        s_Vk.handle,
        "vkCmdDrawIndexedIndirect");

    s_Vk.cmdDispatch = (PFN_vkCmdDispatch)loadProc(
        s_Vk.handle,
        "vkCmdDispatch");

    s_Vk.cmdDispatchIndirect = (PFN_vkCmdDispatchIndirect)loadProc(
        s_Vk.handle,
        "vkCmdDispatchIndirect");

    s_Vk.cmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)loadProc(
        s_Vk.handle,
        "vkCmdBindDescriptorSets");

    s_Vk.cmdPushConstants = (PFN_vkCmdPushConstants)loadProc(
        s_Vk.handle,
        "vkCmdPushConstants");

    s_Vk.createBuffer = (PFN_vkCreateBuffer)loadProc(
        s_Vk.handle,
        "vkCreateBuffer");

    s_Vk.destroyBuffer = (PFN_vkDestroyBuffer)loadProc(
        s_Vk.handle,
        "vkDestroyBuffer");

    s_Vk.mapMemory = (PFN_vkMapMemory)loadProc(
        s_Vk.handle,
        "vkMapMemory");

    s_Vk.unmapMemory = (PFN_vkUnmapMemory)loadProc(
        s_Vk.handle,
        "vkUnmapMemory");

    s_Vk.getBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress)loadProc(
        s_Vk.handle,
        "vkGetBufferDeviceAddress");

    s_Vk.getBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)loadProc(
        s_Vk.handle,
        "vkGetBufferMemoryRequirements");

    s_Vk.bindBufferMemory = (PFN_vkBindBufferMemory)loadProc(
        s_Vk.handle,
        "vkBindBufferMemory");

    s_Vk.createDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)loadProc(
        s_Vk.handle,
        "vkCreateDescriptorSetLayout");

    s_Vk.destroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)loadProc(
        s_Vk.handle,
        "vkDestroyDescriptorSetLayout");

    s_Vk.createDescriptorPool = (PFN_vkCreateDescriptorPool)loadProc(
        s_Vk.handle,
        "vkCreateDescriptorPool");

    s_Vk.destroyDescriptorPool = (PFN_vkDestroyDescriptorPool)loadProc(
        s_Vk.handle,
        "vkDestroyDescriptorPool");

    s_Vk.resetDescriptorPool = (PFN_vkResetDescriptorPool)loadProc(
        s_Vk.handle,
        "vkResetDescriptorPool");

    s_Vk.allocateDescriptorSet = (PFN_vkAllocateDescriptorSets)loadProc(
        s_Vk.handle,
        "vkAllocateDescriptorSets");

    s_Vk.updateDescriptorSet = (PFN_vkUpdateDescriptorSets)loadProc(
        s_Vk.handle,
        "vkUpdateDescriptorSets");

    s_Vk.createPipelineLayout = (PFN_vkCreatePipelineLayout)loadProc(
        s_Vk.handle,
        "vkCreatePipelineLayout");

    s_Vk.destroyPipelineLayout = (PFN_vkDestroyPipelineLayout)loadProc(
        s_Vk.handle,
        "vkDestroyPipelineLayout");

    s_Vk.createGraphicsPipeline = (PFN_vkCreateGraphicsPipelines)loadProc(
        s_Vk.handle,
        "vkCreateGraphicsPipelines");

    s_Vk.createComputePipeline = (PFN_vkCreateComputePipelines)loadProc(
        s_Vk.handle,
        "vkCreateComputePipelines");

    s_Vk.destroyPipeline = (PFN_vkDestroyPipeline)loadProc(
        s_Vk.handle,
        "vkDestroyPipeline");

    s_Vk.waitQueue = (PFN_vkQueueWaitIdle)loadProc(
        s_Vk.handle,
        "vkQueueWaitIdle");
    // clang-format on

    // get version
    PalBool versionFallback = PAL_FALSE;
    uint32_t version = 0;
    if (s_Vk.enumerateInstanceVersion) {
        s_Vk.enumerateInstanceVersion(&version);
        if (version <= VK_API_VERSION_1_0) {
            versionFallback = PAL_TRUE;
        }
    }

    VkResult result;
    uint32_t layerCount = 0;
    PalBool hasValidationLayer = PAL_FALSE;
    s_Vk.messenger = nullptr;
    s_Vk.allocator = allocator;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    if (debugger && debugger->callback) {
        // layers
        result = s_Vk.enumerateInstanceLayerProperties(&layerCount, nullptr);
        if (result == VK_SUCCESS) {
            VkLayerProperties* props = nullptr;
            props = palAllocate(s_Vk.allocator, sizeof(VkLayerProperties) * layerCount, 0);
            if (!props) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }

            s_Vk.enumerateInstanceLayerProperties(&layerCount, props);
            for (int i = 0; i < layerCount; i++) {
                const char* name = props[i].layerName;
                if (strcmp(name, "VK_LAYER_KHRONOS_validation") == 0) {
                    hasValidationLayer = PAL_TRUE;
                    break;
                }
            }
            palFree(s_Vk.allocator, props);

            // message types
            if (!debugger->denyGeneral) {
                debugCreateInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
            }

            if (!debugger->denyPerformance) {
                debugCreateInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            }

            if (!debugger->denyValidation) {
                debugCreateInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
            }

            // message severities
            if (!debugger->denyInfoSeverity) {
                debugCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
            }

            if (!debugger->denyWarningSeverity) {
                debugCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
            }

            if (!debugger->denyErrorSeverity) {
                debugCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            }

            debugCreateInfo.pUserData = debugger->userData;
            debugCreateInfo.pfnUserCallback = debugCallbackVk;
            s_Vk.callback = debugger->callback;
        }
    }

    // extensions
    uint32_t extCount = 0;
    const char* extensions[8];
    result = s_Vk.enumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
    if (result != VK_SUCCESS) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    VkExtensionProperties* extensionProps = nullptr;
    extensionProps = palAllocate(s_Vk.allocator, sizeof(VkExtensionProperties) * extCount, 0);
    if (!extensionProps) {
        return PAL_RESULT_SUCCESS;
    }

    PalBool hasXlib = PAL_FALSE;
    PalBool hasXcb = PAL_FALSE;
    PalBool hasWayland = PAL_FALSE;
    PalBool hasWin32 = PAL_FALSE;
    PalBool hasSurface = PAL_FALSE;
    PalBool hasExtDebug = PAL_FALSE;
    s_Vk.enumerateInstanceExtensionProperties(nullptr, &extCount, extensionProps);

    for (int i = 0; i < extCount; i++) {
        VkExtensionProperties* prop = &extensionProps[i];
        if (strcmp(prop->extensionName, "VK_KHR_xlib_surface") == 0) {
            hasXlib = PAL_TRUE;

        } else if (strcmp(prop->extensionName, "VK_KHR_xcb_surface") == 0) {
            hasXcb = PAL_TRUE;

        } else if (strcmp(prop->extensionName, "VK_KHR_wayland_surface") == 0) {
            hasWayland = PAL_TRUE;

        } else if (strcmp(prop->extensionName, "VK_KHR_win32_surface") == 0) {
            hasWin32 = PAL_TRUE;

        } else if (strcmp(prop->extensionName, "VK_KHR_surface") == 0) {
            hasSurface = PAL_TRUE;

        } else if (strcmp(prop->extensionName, "VK_EXT_debug_utils") == 0) {
            hasExtDebug = PAL_TRUE;
        }
    }
    palFree(s_Vk.allocator, extensionProps);

    int extensionCount = 0;
    if (hasSurface) {
        extensions[extensionCount++] = "VK_KHR_surface";
        if (hasWayland) {
            extensions[extensionCount++] = "VK_KHR_wayland_surface";
        }

        if (hasXlib) {
            extensions[extensionCount++] = "VK_KHR_xlib_surface";
        }

        if (hasXcb) {
            extensions[extensionCount++] = "VK_KHR_xcb_surface";
        }

        if (hasWin32) {
            extensions[extensionCount++] = "VK_KHR_win32_surface";
        }
    }

    const char* layers[2];
    layerCount = 0;
    if (hasValidationLayer && hasExtDebug) {
        extensions[extensionCount++] = "VK_EXT_debug_utils";
        layers[layerCount++] = "VK_LAYER_KHRONOS_validation";
    }

    if (versionFallback) {
        const char* name = "VK_KHR_get_physical_device_properties2";
        extensions[extensionCount++] = name;
    }

    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = version;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Engine";
    appInfo.pApplicationName = "App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instanceCreateInfo = {0};
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.enabledExtensionCount = extensionCount;
    instanceCreateInfo.enabledLayerCount = layerCount;
    instanceCreateInfo.ppEnabledExtensionNames = extensions;
    instanceCreateInfo.ppEnabledLayerNames = layers;

    if (debugger && debugger->callback) {
        instanceCreateInfo.pNext = &debugCreateInfo;
    }

    // vk allocator
    s_Vk.vkAllocator.pfnAllocation = allocateVk;
    s_Vk.vkAllocator.pfnFree = freeVk;
    s_Vk.vkAllocator.pfnReallocation = reallocVk;

    VkInstance instance = nullptr;
    result = s_Vk.createInstance(&instanceCreateInfo, &s_Vk.vkAllocator, &instance);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    // clang-format off
    if (versionFallback) {
        // load get physical device properties2 proc if we are on version 1.0
        s_Vk.getPhysicalDeviceFeatures2 =
            (PFN_vkGetPhysicalDeviceFeatures2KHR)s_Vk.getInstanceProcAddr(
                s_Vk.handle,
                "vkGetPhysicalDeviceFeatures2KHR");
    }

    // load surface creation function pointers
    s_Vk.createWaylandSurface = nullptr;
    s_Vk.createXlibSurface = nullptr;
    s_Vk.createXcbSurface = nullptr;
    s_Vk.createWin32Surface = nullptr;

    if (hasWayland) {
        s_Vk.createWaylandSurface = (PFN_vkCreateWaylandSurfaceKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkCreateWaylandSurfaceKHR");
    }

    if (hasXlib) {
        s_Vk.libX = loadLibrary("libX11.so");
        if (s_Vk.libX) {
            s_Vk.XGetWindowAttributes = (XGetWindowAttributesFn)loadProc(
                s_Vk.libX, 
                "XGetWindowAttributes");

            s_Vk.XVisualIDFromVisual = (XVisualIDFromVisualFn)loadProc(
                s_Vk.libX, 
                "XVisualIDFromVisual");
        }

        s_Vk.createXlibSurface = (PFN_vkCreateXlibSurfaceKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkCreateXlibSurfaceKHR");
    }

    if (hasXcb) {
        s_Vk.libXcb = loadLibrary("libxcb.so.1");
        if (s_Vk.libXcb) {
            s_Vk.xcbGetWindowAttributes = (xcb_get_window_attributes_fn)loadProc(
                s_Vk.libXcb, 
                "xcb_get_window_attributes");

            s_Vk.xcbGetWindowAttributesReply = (xcb_get_window_attributes_reply_fn)loadProc(
                s_Vk.libXcb, 
                "xcb_get_window_attributes_reply");
        }

        s_Vk.createXcbSurface = (PFN_vkCreateXcbSurfaceKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkCreateXcbSurfaceKHR");
    }

    if (hasWin32) {
        s_Vk.createWin32Surface = (PFN_vkCreateWin32SurfaceKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkCreateWin32SurfaceKHR");
    }

    // remaining function procs
    s_Vk.destroySurface = (PFN_vkDestroySurfaceKHR)s_Vk.getInstanceProcAddr(
        instance,
        "vkDestroySurfaceKHR");

    s_Vk.getSurfaceCapabilities =
        (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

    s_Vk.getSurfacePresentModes =
        (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)s_Vk.getInstanceProcAddr(
            instance,
           "vkGetPhysicalDeviceSurfacePresentModesKHR");

    s_Vk.getSurfaceFormats = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)s_Vk.getInstanceProcAddr(
        instance,
        "vkGetPhysicalDeviceSurfaceFormatsKHR");

    s_Vk.checkSurfaceSupport = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)s_Vk.getInstanceProcAddr(
        instance,
        "vkGetPhysicalDeviceSurfaceSupportKHR");

    if (debugger) {
        s_Vk.createMessenger =
            (PFN_vkCreateDebugUtilsMessengerEXT)s_Vk.getInstanceProcAddr(
                instance,
                "vkCreateDebugUtilsMessengerEXT");

        s_Vk.destroyMessenger =
            (PFN_vkDestroyDebugUtilsMessengerEXT)s_Vk.getInstanceProcAddr(
                instance,
                "vkDestroyDebugUtilsMessengerEXT");

        s_Vk.createMessenger(instance, &debugCreateInfo, &s_Vk.vkAllocator, &s_Vk.messenger);
    }
    // clang-format on

    s_Vk.adapters = nullptr;
    s_Vk.instance = instance;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL shutdownGraphicsVk()
{
    if (s_Vk.messenger) {
        s_Vk.destroyMessenger(s_Vk.instance, s_Vk.messenger, &s_Vk.vkAllocator);
    }

    s_Vk.destroyInstance(s_Vk.instance, &s_Vk.vkAllocator);
    freeLibrary(s_Vk.handle);

    if (s_Vk.libX) {
        freeLibrary(s_Vk.libX);
    }

    if (s_Vk.libXcb) {
        freeLibrary(s_Vk.libXcb);
    }

    if (s_Vk.adapters) {
        palFree(s_Vk.allocator, s_Vk.adapters);
    }
    memset(&s_Vk, 0, sizeof(s_Vk));
}

PalResult PAL_CALL enumerateAdaptersVk(
    int32_t* count,
    PalAdapter** outAdapters)
{
    int deviceCount = 0;
    int adapterCount = 0;
    int extCount = 0;
    VkResult ret;
    VkExtensionProperties* exts = nullptr;
    VkPhysicalDeviceProperties props = {0};

    ret = s_Vk.enumeratePhysicalDevices(s_Vk.instance, &deviceCount, nullptr);
    if (ret != VK_SUCCESS) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    if (s_Vk.adapters) {
        palFree(s_Vk.allocator, s_Vk.adapters);
    }

    VkPhysicalDevice* devices = nullptr;
    devices = palAllocate(s_Vk.allocator, sizeof(VkPhysicalDevice) * deviceCount, 0);
    s_Vk.adapters = palAllocate(s_Vk.allocator, sizeof(Adapter) * deviceCount, 0);
    if (!devices || !s_Vk.adapters) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_Vk.enumeratePhysicalDevices(s_Vk.instance, &deviceCount, devices);
    for (int i = 0; i < deviceCount; i++) {
        VkPhysicalDevice phyDevice = devices[i];
        s_Vk.getPhysicalDeviceProperties(phyDevice, &props);
        if (props.apiVersion < VK_API_VERSION_1_3) {
            // check extension
            ret = s_Vk.enumerateDeviceExtensionProperties(phyDevice, nullptr, &extCount, nullptr);
            exts = palAllocate(s_Vk.allocator, sizeof(VkExtensionProperties) * extCount, 0);
            if (!exts) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }

            PalBool found = PAL_FALSE;
            s_Vk.enumerateDeviceExtensionProperties(phyDevice, nullptr, &extCount, exts);

            for (int i = 0; i < extCount; i++) {
                const char* ext = exts[i].extensionName;
                if (strcmp(ext, "VK_KHR_dynamic_rendering") == 0) {
                    found = PAL_TRUE;
                    break;
                }
            }

            palFree(s_Vk.allocator, exts);
            if (!found) {
                continue;
            }
        }

        VkPhysicalDeviceDynamicRenderingFeaturesKHR required = {0};
        required.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;

        VkPhysicalDeviceFeatures2KHR features = {0};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
        features.pNext = &required;
        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);

        if (!required.dynamicRendering) {
            continue;
        }

        if (outAdapters) {
            if (adapterCount < *count) {
                Adapter* tmp = &s_Vk.adapters[adapterCount];
                tmp->handle = devices[i];
                outAdapters[adapterCount] = (PalAdapter*)tmp;
            }
        }
        adapterCount++;
    }

    if (!outAdapters) {
        *count = adapterCount;
    }

    palFree(s_Vk.allocator, devices);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getAdapterInfoVk(
    PalAdapter* adapter,
    PalAdapterInfo* info)
{
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
    VkPhysicalDeviceProperties props = {0};
    VkPhysicalDeviceMemoryProperties memProps = {0};

    s_Vk.getPhysicalDeviceMemoryProperties(phyDevice, &memProps);
    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);

    info->apiType = PAL_ADAPTER_API_TYPE_VULKAN;
    info->shaderFormats = PAL_SHADER_FORMAT_SPIRV;
    info->deviceId = props.deviceID;
    info->vendorId = props.vendorID;
    strcpy(info->name, props.deviceName);
    strcpy(info->backendName, "PAL");

    info->vram = 0;
    info->sharedMemory = 0;
    for (int i = 0; i < memProps.memoryHeapCount; i++) {
        if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            info->vram += memProps.memoryHeaps[i].size;
        } else {
            info->sharedMemory += memProps.memoryHeaps[i].size;
        }
    }

    // get device type
    switch (props.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: {
            info->type = PAL_ADAPTER_TYPE_INTEGRATED;
            break;
        }

        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: {
            info->type = PAL_ADAPTER_TYPE_DISCRETE;
            break;
        }

        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: {
            info->type = PAL_ADAPTER_TYPE_VIRTUAL;
            break;
        }

        case VK_PHYSICAL_DEVICE_TYPE_CPU: {
            info->type = PAL_ADAPTER_TYPE_CPU;
            break;
        }

        default: {
            info->type = PAL_ADAPTER_TYPE_UNKNOWN;
            break;
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getAdapterCapabilitiesVk(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    VkResult result = VK_SUCCESS;
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;

    VkPhysicalDeviceProperties props = {0};
    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);
    VkPhysicalDeviceLimits* limits = &props.limits;

    VkPhysicalDeviceFeatures fts = {0};
    s_Vk.getPhysicalDeviceFeatures(phyDevice, &fts);

    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceAccelerationStructurePropertiesKHR accProps = {0};
    accProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
    properties2.pNext = &accProps;
    s_Vk.getPhysicalDeviceProperties2(phyDevice, &properties2);

    PalViewportCapabilities* viewportCaps = &caps->viewportCaps;
    PalImageCapabilities* imageCaps = &caps->imageCaps;
    PalResourceCapabilities* resourceCaps = &caps->resourceCaps;
    PalComputeCapabilities* computeCaps = &caps->computeCaps;

    // get supported queue commands
    uint32_t count = 0;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(phyDevice, &count, nullptr);

    VkQueueFamilyProperties* queueProps = nullptr;
    queueProps = palAllocate(s_Vk.allocator, sizeof(VkQueueFamilyProperties) * count, 0);
    s_Vk.getPhysicalDeviceQueueFamilyProperties(phyDevice, &count, queueProps);

    caps->maxComputeQueues = 0;
    caps->maxGraphicsQueues = 0;
    caps->maxCopyQueues = 0;

    for (int i = 0; i < count; i++) {
        if (queueProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            caps->maxComputeQueues += queueProps->queueCount;
        }

        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            caps->maxGraphicsQueues += queueProps->queueCount;
        }

        if (queueProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            caps->maxCopyQueues += queueProps->queueCount;
        }
    }

    caps->maxColorAttachments = limits->maxColorAttachments;
    caps->maxUniformBufferSize = limits->maxUniformBufferRange;
    caps->maxStorageBufferSize = limits->maxStorageBufferRange;
    caps->maxPushConstantSize = limits->maxPushConstantsSize;

    caps->maxVertexLayouts = limits->maxVertexInputBindings;
    caps->maxVertexAttributes = limits->maxVertexInputAttributes;
    caps->maxTessellationPatchPoint = limits->maxTessellationPatchSize;

    // viewport limits
    viewportCaps->maxWidth = limits->maxViewportDimensions[0];
    viewportCaps->maxHeight = limits->maxViewportDimensions[1];
    viewportCaps->minBoundsRange = limits->viewportBoundsRange[0];
    viewportCaps->maxBoundsRange = limits->viewportBoundsRange[1];

    // image limits
    imageCaps->maxWidth = limits->maxImageDimension2D;
    imageCaps->maxHeight = limits->maxImageDimension2D;
    imageCaps->maxDepth = limits->maxImageDimension3D;
    imageCaps->maxArrayLayers = limits->maxImageArrayLayers;

    // vulkan does not give this but we calculate from the max width and width
    uint32_t a = imageCaps->maxWidth;
    uint32_t b = imageCaps->maxHeight;
    uint32_t c = imageCaps->maxDepth;

    uint32_t tmp = a > b ? a : b;
    uint32_t size = tmp > c ? tmp : c;
    uint32_t levels = 0;
    while (size > 0) {
        // divide by two
        size = size / 2;
        levels++;
    }
    imageCaps->maxMipLevels = levels;

    // resource limits
    resourceCaps->sampledImageDynamicArrayIndexing = fts.shaderSampledImageArrayDynamicIndexing;
    resourceCaps->storageImageDynamicArrayIndexing = fts.shaderStorageImageArrayDynamicIndexing;
    resourceCaps->storageBufferDynamicArrayIndexing = fts.shaderStorageBufferArrayDynamicIndexing;
    resourceCaps->uniformBufferDynamicArrayIndexing = fts.shaderUniformBufferArrayDynamicIndexing;

    resourceCaps->maxPerStageSampledImages = limits->maxPerStageDescriptorSampledImages;
    resourceCaps->maxPerSetSampledImages = limits->maxDescriptorSetSampledImages;
    resourceCaps->maxPerStageStorageImages = limits->maxPerStageDescriptorStorageImages;
    resourceCaps->maxPerSetStorageImages = limits->maxDescriptorSetStorageImages;

    resourceCaps->maxPerStageSamplers = limits->maxPerStageDescriptorSamplers;
    resourceCaps->maxPerSetSamplers = limits->maxDescriptorSetSamplers;
    resourceCaps->maxPerStageStorageBuffers = limits->maxPerStageDescriptorStorageBuffers;
    resourceCaps->maxPerSetStorageBuffers = limits->maxDescriptorSetStorageBuffers;

    resourceCaps->maxPerStageUniformBuffers = limits->maxPerStageDescriptorUniformBuffers;
    resourceCaps->maxPerSetUniformBuffers = limits->maxDescriptorSetUniformBuffers;

    tmp = accProps.maxPerStageDescriptorAccelerationStructures;
    resourceCaps->maxPerStageAccelerationStructure = tmp;
    resourceCaps->maxPerSetAccelerationStructure = accProps.maxDescriptorSetAccelerationStructures;
    resourceCaps->maxBoundSets = limits->maxBoundDescriptorSets;

    // compute limits
    computeCaps->maxWorkGroupInvocations = limits->maxComputeWorkGroupInvocations;
    computeCaps->maxWorkGroupCount[0] = limits->maxComputeWorkGroupCount[0];
    computeCaps->maxWorkGroupCount[1] = limits->maxComputeWorkGroupCount[1];
    computeCaps->maxWorkGroupCount[2] = limits->maxComputeWorkGroupCount[2];
    computeCaps->maxWorkGroupSize[0] = limits->maxComputeWorkGroupSize[0];
    computeCaps->maxWorkGroupSize[1] = limits->maxComputeWorkGroupSize[1];
    computeCaps->maxWorkGroupSize[2] = limits->maxComputeWorkGroupSize[2];

    palFree(s_Vk.allocator, queueProps);
    return PAL_RESULT_SUCCESS;
}

PalAdapterFeatures PAL_CALL getAdapterFeaturesVk(PalAdapter* adapter)
{
    VkResult result;
    PalAdapterFeatures adapterFeatures = 0;
    uint32_t extensionCount = 0;
    VkPhysicalDeviceProperties props = {0};

    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;

    // get supported extensions
    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);
    result = s_Vk.enumerateDeviceExtensionProperties(phyDevice, nullptr, &extensionCount, nullptr);
    if (result != VK_SUCCESS) {
        // we just return without any modern features which is rare
        return 0;
    }

    VkExtensionProperties* extensionProps = nullptr;
    extensionProps = palAllocate(s_Vk.allocator, sizeof(VkExtensionProperties) * extensionCount, 0);
    if (!extensionProps) {
        return 0;
    }

    // check extensions
    PalBool rayTracing = PAL_FALSE;
    PalBool accelerationStructure = PAL_FALSE;
    PalBool meshShader = PAL_FALSE;
    PalBool fragmentRateShading = PAL_FALSE;
    PalBool timelineSemaphore = PAL_FALSE;
    PalBool descriptorIndexing = PAL_FALSE;
    PalBool shaderFloat16 = PAL_FALSE;
    PalBool multiiView = PAL_FALSE;
    PalBool dynamicstate = PAL_FALSE;
    PalBool bufferDeviceAddress = PAL_FALSE;
    PalBool shaderParameters = PAL_FALSE;
    PalBool nullDescriptors = PAL_FALSE;
    PalBool rayQuery = PAL_FALSE;
    s_Vk.enumerateDeviceExtensionProperties(phyDevice, nullptr, &extensionCount, extensionProps);

    // clang-format off
    // check if the extensions are present
    for (int i = 0; i < extensionCount; i++) {
        VkExtensionProperties* props = &extensionProps[i];
        if (strcmp(props->extensionName, "VK_KHR_ray_tracing_pipeline") == 0) {
            rayTracing = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_KHR_acceleration_structure") == 0) {
            accelerationStructure = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_EXT_mesh_shader") == 0) {
            meshShader = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_KHR_fragment_shading_rate") == 0) {
            fragmentRateShading = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_EXT_descriptor_indexing") == 0) {
            descriptorIndexing = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_KHR_swapchain") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_SWAPCHAIN;

        } else if (strcmp(props->extensionName, "VK_KHR_shader_float16_int8") == 0) {
            shaderFloat16 = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_KHR_timeline_semaphore") == 0) {
            timelineSemaphore = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_KHR_multiview") == 0) {
            multiiView = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_EXT_extended_dynamic_state") == 0) {
            dynamicstate = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_EXT_extended_dynamic_state2") == 0) {
            VkPhysicalDeviceExtendedDynamicState2FeaturesEXT dynState2 = {0};
            dynState2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &dynState2;

            s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
            if (dynState2.extendedDynamicState2) {
                adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE;
                adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE;
                adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP;
            }

        } else if (strcmp(props->extensionName, "VK_KHR_depth_stencil_resolve") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE;

        } else if (strcmp(props->extensionName, "VK_KHR_draw_indirect_count") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT;

        } else if (strcmp(props->extensionName, "VK_KHR_draw_mesh_tasks_indirect_count") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT;

        } else if (strcmp(props->extensionName, "VK_KHR_buffer_device_address") == 0) {
            bufferDeviceAddress = PAL_TRUE;

        }  else if (strcmp(props->extensionName, "VK_KHR_shader_draw_parameters") == 0) {
            shaderParameters = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_EXT_robustness2") == 0) {
            nullDescriptors = PAL_TRUE;
        }
    }

    // features that require additional checks
    if (rayTracing && accelerationStructure) {
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray = {0};
        VkPhysicalDeviceAccelerationStructureFeaturesKHR acc = {0};

        ray.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        acc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

        ray.pNext = &acc;
        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &ray;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (ray.rayTracingPipeline && acc.accelerationStructure) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_RAY_TRACING;
        }

        if (ray.rayTracingPipelineTraceRaysIndirect) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_RAY_TRACING;
        }
    }

    if (meshShader) {
        VkPhysicalDeviceMeshShaderFeaturesEXT mesh = {0};
        mesh.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &mesh;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (mesh.meshShader && mesh.taskShader) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_MESH_SHADER;
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH;
        }
    }

    if (fragmentRateShading) {
        VkPhysicalDeviceFragmentShadingRateFeaturesKHR frag = {0};
        frag.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &frag;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (frag.pipelineFragmentShadingRate) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE;
        }

        if (frag.attachmentFragmentShadingRate) {
            adapterFeatures |=
                PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_2 || descriptorIndexing) {
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT desc = {0};
        desc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &desc;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        // core features we need
        if (desc.runtimeDescriptorArray || 
            desc.descriptorBindingUpdateUnusedWhilePending) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING;
        }

        if (desc.descriptorBindingPartiallyBound) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_PARTIALLY_BOUND_DESCRIPTORS;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_2 || timelineSemaphore) {
        VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timeline = {0};
        timeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &timeline;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (timeline.timelineSemaphore) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_2 || shaderFloat16) {
        VkPhysicalDeviceShaderFloat16Int8FeaturesKHR shader16 = {0};
        shader16.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &shader16;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (shader16.shaderFloat16) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_SHADER_FLOAT16;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_1 || multiiView) {
        VkPhysicalDeviceMultiviewFeaturesKHR multiView = {0};
        multiView.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &multiView;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (multiView.multiview) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_MULTI_VIEW;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_3 || dynamicstate) {
        VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynState = {0};
        dynState.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &dynState;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (dynState.extendedDynamicState) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE;
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE;
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_2 || bufferDeviceAddress) {
        VkPhysicalDeviceBufferDeviceAddressFeaturesKHR bufferAddress = {0};
        bufferAddress.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &bufferAddress;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (bufferAddress.bufferDeviceAddress) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_2) {
        VkPhysicalDeviceVulkan12Features features12 = {0};
        features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &features12;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (features12.drawIndirectCount) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_2 || shaderParameters) {
        VkPhysicalDeviceShaderDrawParametersFeatures drawParameters = {0};
        drawParameters.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &drawParameters;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (drawParameters.shaderDrawParameters) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DISPATCH_BASE;
        }
    }

    if (nullDescriptors) {
        VkPhysicalDeviceRobustness2FeaturesEXT nullDescriptors = {0};
        nullDescriptors.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &nullDescriptors;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (nullDescriptors.nullDescriptor) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS;
        }
    }

    if (rayQuery) {
        VkPhysicalDeviceRayQueryFeaturesKHR query = {0};
        query.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &query;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (query.rayQuery) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_RAY_QUERY;
        }
    }
    // clang-format on

    VkPhysicalDeviceFeatures features;
    s_Vk.getPhysicalDeviceFeatures(phyDevice, &features);

    // check for additional features
    if (features.multiViewport) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_MULTI_VIEWPORT;
    }

    if (features.samplerAnisotropy) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY;
    }

    if (features.sampleRateShading) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING;
    }

    if (features.shaderFloat64) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SHADER_FLOAT64;
    }

    if (features.shaderInt64) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SHADER_INT64;
    }

    if (features.shaderInt16) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SHADER_INT16;
    }

    if (features.geometryShader) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_GEOMETRY_SHADER;
    }

    if (features.tessellationShader) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_TESSELLATION_SHADER;
    }

    if (features.fillModeNonSolid) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_POLYGON_MODE_LINE;
    }

    // this features are supported on vulkan
    adapterFeatures |= PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY;
    adapterFeatures |= PAL_ADAPTER_FEATURE_FENCE_RESET;
    adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW;
    adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH;

    palFree(s_Vk.allocator, extensionProps);
    return adapterFeatures;
}

uint32_t PAL_CALL getHighestSupportedShaderTargetVk(
    PalAdapter* adapter, 
    PalShaderFormats shaderFormat)
{
    if (shaderFormat != PAL_SHADER_FORMAT_SPIRV) {
        return 0;
    }

    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDeviceProperties props = {0};
    s_Vk.getPhysicalDeviceProperties(vkAdapter->handle, &props);

     if (props.apiVersion >= VK_API_VERSION_1_3) {
        return PAL_MAKE_SHADER_TARGET(1, 6);

    } else if (props.apiVersion >= VK_API_VERSION_1_2) {
        return PAL_MAKE_SHADER_TARGET(1, 5);

    } else if (props.apiVersion >= VK_API_VERSION_1_1) {
        return PAL_MAKE_SHADER_TARGET(1, 4);
        
    } else if (props.apiVersion >= VK_API_VERSION_1_0) {
        return PAL_MAKE_SHADER_TARGET(1, 2);
    }

    return 0;
}

// ==================================================
// Device
// ==================================================

PalResult PAL_CALL createDeviceVk(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice)
{
    float priority = 1.0f;
    uint32_t phyQueueCount = 0;
    uint32_t queueFamilyCount = 0;
    VkResult result = VK_SUCCESS;
    Device* device = nullptr;
    VkPhysicalDeviceProperties props = {0};

    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);

    VkQueueFamilyProperties* queueFamilyProps = nullptr;
    VkDeviceQueueCreateInfo* queueCreateInfos = nullptr;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamilyCount, nullptr);

    uint32_t queueFamilySize = sizeof(VkQueueFamilyProperties) * queueFamilyCount;
    uint32_t queueCreateInfosSize = sizeof(VkDeviceQueueCreateInfo) * queueFamilyCount;

    queueFamilyProps = palAllocate(s_Vk.allocator, queueFamilySize, 0);
    queueCreateInfos = palAllocate(s_Vk.allocator, queueCreateInfosSize, 0);
    device = palAllocate(s_Vk.allocator, sizeof(Device), 0);
    if (!queueFamilyProps || !queueCreateInfos || !device) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(device, 0, sizeof(Device));
    device->phyDevice = phyDevice;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamilyCount, queueFamilyProps);
    for (int i = 0; i < queueFamilyCount; i++) {
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].pNext = nullptr;
        queueCreateInfos[i].pQueuePriorities = &priority;
        queueCreateInfos[i].queueFamilyIndex = i;
        queueCreateInfos[i].queueCount = queueFamilyProps[i].queueCount;
        queueCreateInfos[i].flags = 0;

        // we need the total number of physical queues
        phyQueueCount += queueFamilyProps[i].queueCount;
    }

    device->phyQueues = palAllocate(s_Vk.allocator, sizeof(PhysicalQueue) * phyQueueCount, 0);
    if (!device->phyQueues) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // build features and extensions capabilities
    VkPhysicalDeviceFeatures phyDeviceFeatures = {0};
    s_Vk.getPhysicalDeviceFeatures(phyDevice, &phyDeviceFeatures);

    VkPhysicalDeviceFeatures coreFeatures = {0};
    if (features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY) {
        coreFeatures.samplerAnisotropy = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING) {
        coreFeatures.sampleRateShading = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_MULTI_VIEWPORT) {
        coreFeatures.multiViewport = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER) {
        coreFeatures.tessellationShader = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_GEOMETRY_SHADER) {
        coreFeatures.geometryShader = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_INT16) {
        coreFeatures.shaderInt16 = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_INT64) {
        coreFeatures.shaderInt64 = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_FLOAT64) {
        coreFeatures.shaderFloat64 = PAL_TRUE;
    }

    // clang-format off
    coreFeatures.shaderSampledImageArrayDynamicIndexing = phyDeviceFeatures.shaderSampledImageArrayDynamicIndexing;
    coreFeatures.shaderStorageImageArrayDynamicIndexing = phyDeviceFeatures.shaderStorageImageArrayDynamicIndexing;
    coreFeatures.shaderStorageBufferArrayDynamicIndexing = phyDeviceFeatures.shaderStorageBufferArrayDynamicIndexing;
    coreFeatures.shaderUniformBufferArrayDynamicIndexing = phyDeviceFeatures.shaderUniformBufferArrayDynamicIndexing;
    // clang-format on

    // extensions and features2
    void* next = nullptr;
    int extCount = 0;
    const char* extensions[32] = {0};

    VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timeline = {0};
    timeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR;

    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR shader16 = {0};
    shader16.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

    VkPhysicalDeviceMeshShaderFeaturesEXT mesh = {0};
    mesh.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray = {0};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR acc = {0};
    ray.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    acc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

    VkPhysicalDeviceFragmentShadingRateFeaturesKHR fsr = {0};
    fsr.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;

    VkPhysicalDeviceDescriptorIndexingFeatures descIndex = {0};
    descIndex.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;

    VkPhysicalDeviceMultiviewFeatures multiView = {0};
    multiView.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;

    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;

    VkPhysicalDeviceExtendedDynamicState2FeaturesEXT dynamicState2 = {0};
    dynamicState2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT;

    VkPhysicalDeviceBufferDeviceAddressFeaturesKHR bufferAddress = {0};
    bufferAddress.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRendering = {0};
    dynamicRendering.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;

    VkPhysicalDeviceSynchronization2FeaturesKHR sync2 = {0};
    sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;

    VkPhysicalDeviceShaderDrawParametersFeatures drawParameters = {0};
    drawParameters.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;

    VkPhysicalDeviceRobustness2FeaturesEXT nullDescriptors = {0};
    nullDescriptors.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;

    if (props.apiVersion < VK_API_VERSION_1_3) {
        extensions[extCount++] = "VK_KHR_dynamic_rendering";
        extensions[extCount++] = "VK_KHR_synchronization2";
    }

    dynamicRendering.dynamicRendering = PAL_TRUE;
    sync2.synchronization2 = PAL_TRUE;

    dynamicRendering.pNext = next;
    sync2.pNext = &dynamicRendering;
    next = &sync2;

    if (features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
        extensions[extCount++] = "VK_KHR_swapchain";
    }

    if (features & PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE) {
        extensions[extCount++] = "VK_KHR_depth_stencil_resolve";
    }

    if (features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_timeline_semaphore";
        }
        timeline.timelineSemaphore = PAL_TRUE;

        timeline.pNext = next;
        next = &timeline;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_FLOAT16) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_shader_float16_int8";
        }
        shader16.shaderFloat16 = PAL_TRUE;

        shader16.pNext = next;
        next = &shader16;
    }

    if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        extensions[extCount++] = "VK_KHR_ray_tracing_pipeline";
        extensions[extCount++] = "VK_KHR_acceleration_structure";
        extensions[extCount++] = "VK_KHR_deferred_host_operations";
        ray.rayTracingPipeline = PAL_TRUE;
        acc.accelerationStructure = PAL_TRUE;

        ray.pNext = next;
        acc.pNext = &ray;
        next = &acc;
    }

    if (features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
        extensions[extCount++] = "VK_EXT_mesh_shader";
        mesh.meshShader = PAL_TRUE;
        mesh.taskShader = PAL_TRUE;

        // mesh shader needs geometry feature for primitives
        coreFeatures.geometryShader = PAL_TRUE;

        // msh draw indirect count
        if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT) {
            extensions[extCount++] = "VK_KHR_draw_mesh_tasks_indirect_count";
        }

        mesh.pNext = next;
        next = &mesh;
    }

    if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_draw_indirect_count";
        }
    }

    if ((features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) ||
        (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT)) {
        extensions[extCount++] = "VK_KHR_fragment_shading_rate";
        fsr.pipelineFragmentShadingRate = PAL_TRUE;

        // fragment shading rate attachment needs this
        if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT) {
            fsr.attachmentFragmentShadingRate = PAL_TRUE;
        }

        fsr.pNext = next;
        next = &fsr;
    }

    if (features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_EXT_descriptor_indexing";
        }

        // check support for sub features
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT desc = {0};
        desc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &desc;
        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);

        // clang-format off
        descIndex.runtimeDescriptorArray = desc.runtimeDescriptorArray;
        descIndex.descriptorBindingUpdateUnusedWhilePending = desc.descriptorBindingUpdateUnusedWhilePending;

        descIndex.shaderSampledImageArrayNonUniformIndexing = desc.shaderSampledImageArrayNonUniformIndexing;
        descIndex.descriptorBindingSampledImageUpdateAfterBind = desc.descriptorBindingSampledImageUpdateAfterBind;

        descIndex.shaderStorageImageArrayNonUniformIndexing = desc.shaderStorageImageArrayNonUniformIndexing;
        descIndex.descriptorBindingStorageImageUpdateAfterBind = desc.descriptorBindingStorageImageUpdateAfterBind;

        descIndex.shaderStorageBufferArrayNonUniformIndexing = desc.shaderStorageBufferArrayNonUniformIndexing;
        descIndex.descriptorBindingStorageBufferUpdateAfterBind = desc.descriptorBindingStorageBufferUpdateAfterBind;

        descIndex.shaderUniformBufferArrayNonUniformIndexing = desc.shaderUniformBufferArrayNonUniformIndexing;
        descIndex.descriptorBindingUniformBufferUpdateAfterBind = desc.descriptorBindingUniformBufferUpdateAfterBind;
        // clang-format on

        descIndex.pNext = next;
        next = &descIndex;
    }

    if (features & PAL_ADAPTER_FEATURE_PARTIALLY_BOUND_DESCRIPTORS) {
        if (!(features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING)) {
            if (props.apiVersion < VK_API_VERSION_1_2) {
                extensions[extCount++] = "VK_EXT_descriptor_indexing";
            }

            descIndex.pNext = next;
            next = &descIndex;
        }
        
        descIndex.descriptorBindingPartiallyBound = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_MULTI_VIEW) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_multiview";
        }
        multiView.multiview = PAL_TRUE;

        multiView.pNext = next;
        next = &multiView;
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE ||
        features & PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE ||
        features & PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY) {
        if (props.apiVersion < VK_API_VERSION_1_3) {
            extensions[extCount++] = "VK_EXT_extended_dynamic_state";
        }
        dynamicState.extendedDynamicState = PAL_TRUE;

        dynamicState.pNext = next;
        next = &dynamicState;
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE ||
        features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE ||
        features & PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP) {
        extensions[extCount++] = "VK_EXT_extended_dynamic_state2";
        dynamicState2.extendedDynamicState2 = PAL_TRUE;

        dynamicState2.pNext = next;
        next = &dynamicState2;
    }

    if (features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_buffer_device_address";
        }
        bufferAddress.bufferDeviceAddress = PAL_TRUE;

        bufferAddress.pNext = next;
        next = &bufferAddress;
    }

    if (features & PAL_ADAPTER_FEATURE_DISPATCH_BASE) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_shader_draw_parameters";
        }
        drawParameters.shaderDrawParameters = PAL_TRUE;

        drawParameters.pNext = next;
        next = &drawParameters;
    }

    if (features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS) {
        extensions[extCount++] = "VK_EXT_robustness2";
        nullDescriptors.nullDescriptor = PAL_TRUE;

        nullDescriptors.pNext = next;
        next = &nullDescriptors;
    }

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pEnabledFeatures = &coreFeatures;
    createInfo.enabledExtensionCount = extCount;
    createInfo.ppEnabledExtensionNames = extensions;
    createInfo.pQueueCreateInfos = queueCreateInfos;
    createInfo.queueCreateInfoCount = queueFamilyCount;
    createInfo.pNext = next;

    result = s_Vk.createDevice(phyDevice, &createInfo, &s_Vk.vkAllocator, &device->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, queueFamilyProps);
        palFree(s_Vk.allocator, queueCreateInfos);
        palFree(s_Vk.allocator, device->phyQueues);
        palFree(s_Vk.allocator, device);
        return makeResultVk(result);
    }

    device->phyQueueIndex = 0;
    device->queueFamilyCount = queueFamilyCount;
    device->phyQueueCount = phyQueueCount;
    device->features = features;

    // get queues
    for (int i = 0; i < queueFamilyCount; i++) {
        VkQueueFamilyProperties* data = &queueFamilyProps[i];
        for (int j = 0; j < data->queueCount; j++) {
            PhysicalQueue* queue = &device->phyQueues[i];
            s_Vk.getDeviceQueue(device->handle, i, j, &queue->handle);
            queue->usages = data->queueFlags;
            queue->usedUsages = 0;
            queue->familyIndex = i;
            queue->phyDevice = phyDevice;
        }
    }

    // cache memory type indices
    VkPhysicalDeviceMemoryProperties memProps = {0};
    s_Vk.getPhysicalDeviceMemoryProperties(phyDevice, &memProps);

    memset(device->memoryClassMask, 0, sizeof(uint32_t) * 3);
    for (int i = 0; i < memProps.memoryTypeCount; i++) {
        VkMemoryPropertyFlags flags = memProps.memoryTypes[i].propertyFlags;

        uint32_t bit = (1u << i);
        if ((flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) &&
            !(flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
            device->memoryClassMask[PAL_MEMORY_TYPE_GPU_ONLY] |= bit;
        }

        if ((flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
            (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            device->memoryClassMask[PAL_MEMORY_TYPE_CPU_UPLOAD] |= bit;
        }

        if ((flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
            (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)) {
            device->memoryClassMask[PAL_MEMORY_TYPE_CPU_READBACK] |= bit;
        }
    }

    // HACK: most CPU drivers dont have a vram so we set the vram to system memory
    if (device->memoryClassMask[PAL_MEMORY_TYPE_GPU_ONLY] == 0) {
        device->memoryClassMask[PAL_MEMORY_TYPE_GPU_ONLY] =
            device->memoryClassMask[PAL_MEMORY_TYPE_CPU_UPLOAD];
    }

    // clang-format off
    // swapchain procs
    if (features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
        device->acquireNextImage = (PFN_vkAcquireNextImageKHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkAcquireNextImageKHR");

        device->createSwapchain = (PFN_vkCreateSwapchainKHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCreateSwapchainKHR");

        device->destroySwapchain = (PFN_vkDestroySwapchainKHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkDestroySwapchainKHR");

        device->getSwapchainImages = (PFN_vkGetSwapchainImagesKHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkGetSwapchainImagesKHR");

        device->queuePresent = (PFN_vkQueuePresentKHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkQueuePresentKHR");
    }

    // semaphore procs
    if (features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
        device->waitSemaphore = (PFN_vkWaitSemaphores)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkWaitSemaphores");

        device->signalSemaphore = (PFN_vkSignalSemaphore)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkSignalSemaphore");

        device->getSemaphoreValue = (PFN_vkGetSemaphoreCounterValue)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkGetSemaphoreCounterValue");

        if (!device->waitSemaphore) {
            device->waitSemaphore = (PFN_vkWaitSemaphoresKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkWaitSemaphoresKHR");

            device->signalSemaphore = (PFN_vkSignalSemaphoreKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkSignalSemaphoreKHR");

            device->getSemaphoreValue = (PFN_vkGetSemaphoreCounterValueKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkGetSemaphoreCounterValueKHR");
        }
    }

    // fragment shading rate procs
    if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) {
        device->cmdSetFragmentShadingRate =
            (PFN_vkCmdSetFragmentShadingRateKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdSetFragmentShadingRateKHR");
    }

    // mesh shader procs
    if (features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
        device->cmdDrawMeshTask = (PFN_vkCmdDrawMeshTasksEXT)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdDrawMeshTasksEXT");

        device->cmdDrawMeshTaskIndirect =
            (PFN_vkCmdDrawMeshTasksIndirectEXT)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdDrawMeshTasksIndirectEXT");

        device->cmdDrawMeshTaskIndirectCount =
            (PFN_vkCmdDrawMeshTasksIndirectCountEXT)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdDrawMeshTasksIndirectCountEXT");
    }

    // ray tracing procs
    device->limits.maxPayloadSize = 0;
    if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        device->limits.maxPayloadSize = 64;

        device->createAccelerationStructure =
            (PFN_vkCreateAccelerationStructureKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCreateAccelerationStructureKHR");

        device->destroyAccelerationStructure =
            (PFN_vkDestroyAccelerationStructureKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkDestroyAccelerationStructureKHR");

        device->getAccelerationBuildsize =
            (PFN_vkGetAccelerationStructureBuildSizesKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkGetAccelerationStructureBuildSizesKHR");

        device->cmdBuildAccelerationStructures =
            (PFN_vkCmdBuildAccelerationStructuresKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdBuildAccelerationStructuresKHR");

        device->getAccelerationDeviceAddress =
            (PFN_vkGetAccelerationStructureDeviceAddressKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkGetAccelerationStructureDeviceAddressKHR");

        device->cmdTraceRays =
            (PFN_vkCmdTraceRaysKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdTraceRaysKHR");

        device->createRayTracingPipeline =
            (PFN_vkCreateRayTracingPipelinesKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCreateRayTracingPipelinesKHR");

        device->cmdTraceRaysIndirect =
            (PFN_vkCmdTraceRaysIndirectKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdTraceRaysIndirectKHR");

        device->getRayTracingShaderGroupHandles =
            (PFN_vkGetRayTracingShaderGroupHandlesKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkGetRayTracingShaderGroupHandlesKHR");
    }

    // buffer address procs
    if (features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS) {
        device->getBufferrAddress =
            (PFN_vkGetBufferDeviceAddress)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkGetBufferDeviceAddress");

        if (!device->getBufferrAddress) {
            device->getBufferrAddress =
                (PFN_vkGetBufferDeviceAddressKHR)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkGetBufferDeviceAddressKHR");
        }
    }

    // indirect draw count procs
    if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT) {
        device->cmdDrawIndirectCount =
            (PFN_vkCmdDrawIndirectCount)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdDrawIndirectCount");

        device->cmdDrawIndexedIndirectCount =
                (PFN_vkCmdDrawIndexedIndirectCount)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkCmdDrawIndexedIndirectCount");

        if (!device->cmdDrawIndirectCount) {
            device->cmdDrawIndirectCount =
                (PFN_vkCmdDrawIndirectCountKHR)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkCmdDrawIndirectCountKHR");

            device->cmdDrawIndexedIndirectCount =
                    (PFN_vkCmdDrawIndexedIndirectCountKHR)s_Vk.getDeviceProcAddr(
                        device->handle,
                        "vkCmdDrawIndexedIndirectCountKHR");
        }
    }

    // dispatch base procs
    if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) {
        device->cmdDispatchBase =
            (PFN_vkCmdDispatchBase)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdDispatchBase");

        if (!device->cmdDispatchBase) {
            device->cmdDispatchBase =
                (PFN_vkCmdDispatchBaseKHR)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkCmdDispatchBaseKHR");
        }
    }

    // dynamic rendering procs
    device->cmdBeginRendering =
        (PFN_vkCmdBeginRendering)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdBeginRendering");

    device->cmdEndRendering =
        (PFN_vkCmdEndRendering)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdEndRendering");

    device->cmdPipelineBarrier =
        (PFN_vkCmdPipelineBarrier2)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdPipelineBarrier2");

    device->queueSubmit =
        (PFN_vkQueueSubmit2)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkQueueSubmit2");

    if (!device->cmdBeginRendering) {
        device->cmdBeginRendering =
            (PFN_vkCmdBeginRenderingKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdBeginRenderingKHR");

        device->cmdEndRendering =
            (PFN_vkCmdEndRenderingKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdEndRenderingKHR");

        device->cmdPipelineBarrier =
        (PFN_vkCmdPipelineBarrier2KHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdPipelineBarrier2KHR");

        device->queueSubmit =
        (PFN_vkQueueSubmit2KHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkQueueSubmit2KHR");
    }

    // dynamic states
    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE) {
        device->cmdSetCullMode =
            (PFN_vkCmdSetCullMode)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdSetCullMode");

        if (!device->cmdSetCullMode) {
            device->cmdSetCullMode =
                (PFN_vkCmdSetCullModeEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkCmdSetCullModeEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE) {
        device->cmdSetFrontFace =
            (PFN_vkCmdSetFrontFace)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetFrontFace");

        if (!device->cmdSetFrontFace) {
            device->cmdSetFrontFace =
                (PFN_vkCmdSetFrontFaceEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetFrontFaceEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY) {
        device->cmdSetPrimitiveTopology =
            (PFN_vkCmdSetPrimitiveTopology)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetPrimitiveTopology");

        if (!device->cmdSetPrimitiveTopology) {
            device->cmdSetPrimitiveTopology =
                (PFN_vkCmdSetPrimitiveTopologyEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetPrimitiveTopologyEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE) {
        device->cmdSetDepthTestEnable =
            (PFN_vkCmdSetDepthTestEnable)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetDepthTestEnable");

        if (!device->cmdSetDepthTestEnable) {
            device->cmdSetDepthTestEnable =
                (PFN_vkCmdSetDepthTestEnableEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetDepthTestEnableEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE) {
        device->cmdSetDepthWriteEnable =
            (PFN_vkCmdSetDepthWriteEnable)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetDepthWriteEnable");

        if (!device->cmdSetDepthWriteEnable) {
            device->cmdSetDepthWriteEnable =
                (PFN_vkCmdSetDepthWriteEnableEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetDepthWriteEnableEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP) {
        device->cmdSetStencilOp =
            (PFN_vkCmdSetStencilOp)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetStencilOp");

        if (!device->cmdSetStencilOp) {
            device->cmdSetStencilOp =
                (PFN_vkCmdSetStencilOpEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetStencilOpEXT");
        }
    }

    // clang-format on

    palFree(s_Vk.allocator, queueFamilyProps);
    palFree(s_Vk.allocator, queueCreateInfos);

    *outDevice = (PalDevice*)device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDeviceVk(PalDevice* device)
{
    Device* vkDevice = (Device*)device;
    s_Vk.destroyDevice(vkDevice->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, vkDevice->phyQueues);
    palFree(s_Vk.allocator, vkDevice);
}

// ==================================================
// Memory
// ==================================================

PalResult PAL_CALL allocateMemoryVk(
    PalDevice* device,
    PalMemoryType type,
    uint64_t memoryMask,
    uint64_t size,
    PalMemory** outMemory)
{
    VkResult result;
    Memory* memory = nullptr;
    Device* vkDevice = (Device*)device;
    VkMemoryAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = (VkDeviceSize)size;
    VkMemoryAllocateFlagsInfo allocateFlagsInfo = {0};

    memory = palAllocate(s_Vk.allocator, sizeof(Memory), 0);
    if (!memory) {
        return PAL_RESULT_NULL_POINTER;
    }

    uint32_t memoryTypeMask = 0;
    uint32_t usages = 0;
    palUnpackUint32(memoryMask, &memoryTypeMask, &usages);
    uint32_t memoryClassMask = vkDevice->memoryClassMask[type] & memoryTypeMask;
    if (memoryClassMask == 0) {
        return PAL_RESULT_MEMORY_TYPE_NOT_SUPPORTED;
    }

    // pick an index using the scoring system
    uint32_t memoryIndex = findBestMemoryIndexVk(vkDevice->phyDevice, memoryClassMask);
    if (memoryIndex == UINT32_MAX) {
        return PAL_RESULT_MEMORY_TYPE_NOT_SUPPORTED;
    }

    // check if the memory index is valid
    if (!(memoryClassMask & (1u << memoryIndex))) {
        return PAL_RESULT_MEMORY_TYPE_NOT_SUPPORTED;
    }

    allocateInfo.memoryTypeIndex = memoryIndex;
    if (usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        allocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        allocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
        allocateInfo.pNext = &allocateFlagsInfo;
    }

    result = s_Vk.allocateMemory(
        vkDevice->handle, 
        &allocateInfo, 
        &s_Vk.vkAllocator, 
        &memory->handle);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    memory->type = type;
    *outMemory = (PalMemory*)memory;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL freeMemoryVk(
    PalDevice* device,
    PalMemory* memory)
{
    Device* vkDevice = (Device*)device;
    Memory* vkMemory = (Memory*)memory;
    s_Vk.freeMemory(vkDevice->handle, vkMemory->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, vkMemory);
}

// ==================================================
// Extended Adapter Features
// ==================================================

PalResult PAL_CALL querySamplerAnisotropyCapabilitiesVk(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPhysicalDeviceProperties props = {0};
    s_Vk.getPhysicalDeviceProperties(vkDevice->phyDevice, &props);

    caps->maxAnisotropy = props.limits.maxSamplerAnisotropy;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryMultiViewCapabilitiesVk(
    PalDevice* device,
    PalMultiViewCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPhysicalDeviceMultiviewPropertiesKHR props = {0};
    VkPhysicalDeviceProperties2 properties2 = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR;

    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &properties2);

    caps->maxViewCount = props.maxMultiviewViewCount;
    if (caps->maxViewCount == 0) {
        caps->maxViewCount = 1;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryMultiViewportCapabilitiesVk(
    PalDevice* device,
    PalMultiViewportCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPhysicalDeviceProperties props = {0};
    s_Vk.getPhysicalDeviceProperties(vkDevice->phyDevice, &props);

    caps->maxCount = props.limits.maxViewports;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryDepthStencilCapabilitiesVk(
    PalDevice* device,
    PalDepthStencilCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceDepthStencilResolvePropertiesKHR props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR;

    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &properties2);

    caps->independentResolve = props.independentResolve;
    if (props.supportedDepthResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT_KHR) {
        caps->depthResolves[PAL_RESOLVE_MODE_AVERAGE] = PAL_TRUE;
    }

    if (props.supportedDepthResolveModes & VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR) {
        caps->depthResolves[PAL_RESOLVE_MODE_SAMPLE_ZERO] = PAL_TRUE;
    }

    if (props.supportedDepthResolveModes & VK_RESOLVE_MODE_MIN_BIT_KHR) {
        caps->depthResolves[PAL_RESOLVE_MODE_MIN] = PAL_TRUE;
    }

    if (props.supportedDepthResolveModes & VK_RESOLVE_MODE_MAX_BIT_KHR) {
        caps->depthResolves[PAL_RESOLVE_MODE_MAX] = PAL_TRUE;
    }

    // stencil
    if (props.supportedStencilResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT_KHR) {
        caps->stencilResolves[PAL_RESOLVE_MODE_AVERAGE] = PAL_TRUE;
    }

    if (props.supportedStencilResolveModes & VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR) {
        caps->stencilResolves[PAL_RESOLVE_MODE_SAMPLE_ZERO] = PAL_TRUE;
    }

    if (props.supportedStencilResolveModes & VK_RESOLVE_MODE_MIN_BIT_KHR) {
        caps->stencilResolves[PAL_RESOLVE_MODE_MIN] = PAL_TRUE;
    }

    if (props.supportedStencilResolveModes & VK_RESOLVE_MODE_MAX_BIT_KHR) {
        caps->stencilResolves[PAL_RESOLVE_MODE_MAX] = PAL_TRUE;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryFragmentShadingRateCapabilitiesVk(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceFragmentShadingRatePropertiesKHR props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;

    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &properties2);

    memset(caps, 0, sizeof(PalFragmentShadingRateCapabilities));
    for (int i = 0; i < PAL_FRAGMENT_SHADING_RATE_MAX; i++) {
        VkExtent2D size = getShadingRateSizeVk((PalFragmentShadingRate)i);

        // check against the max size
        if (size.width <= props.maxFragmentSize.width ||
            size.height <= props.maxFragmentSize.height) {
            caps->shadingRates[i] = PAL_TRUE;
        }
    }

    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP] = PAL_TRUE;
    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE] = PAL_TRUE;
    if (props.fragmentShadingRateNonTrivialCombinerOps) {
        caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN] = PAL_TRUE;
        caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX] = PAL_TRUE;
        caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL] = PAL_TRUE;
    }

    VkExtent2D size = props.minFragmentShadingRateAttachmentTexelSize;
    caps->minTexelWidth = size.width;
    caps->minTexelHeight = size.height;

    size = props.maxFragmentShadingRateAttachmentTexelSize;
    caps->maxTexelWidth = size.width;
    caps->maxTexelHeight = size.height;

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryMeshShaderCapabilitiesVk(
    PalDevice* device,
    PalMeshShaderCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceMeshShaderPropertiesEXT props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT;
    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &properties2);

    caps->maxOutputPrimitives = props.maxMeshOutputPrimitives;
    caps->maxOutputVertices = props.maxMeshOutputVertices;
    caps->maxTaskWorkGroupInvocations = props.maxTaskWorkGroupInvocations;
    caps->maxWorkGroupInvocations = props.maxMeshWorkGroupInvocations;

    caps->maxTaskWorkGroupCount[0] = props.maxTaskWorkGroupCount[0];
    caps->maxTaskWorkGroupCount[1] = props.maxTaskWorkGroupCount[1];
    caps->maxTaskWorkGroupCount[2] = props.maxTaskWorkGroupCount[2];

    caps->maxWorkGroupCount[0] = props.maxMeshWorkGroupCount[0];
    caps->maxWorkGroupCount[1] = props.maxMeshWorkGroupCount[1];
    caps->maxWorkGroupCount[2] = props.maxMeshWorkGroupCount[2];

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryRayTracingCapabilitiesVk(
    PalDevice* device,
    PalRayTracingCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceRayTracingPipelinePropertiesKHR props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    VkPhysicalDeviceAccelerationStructurePropertiesKHR accProps = {0};
    accProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;

    props.pNext = &accProps;
    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &properties2);

    caps->maxRecursionDepth = props.maxRayRecursionDepth;
    caps->maxHitAttributeSize = props.maxRayHitAttributeSize;
    caps->maxInstanceCount = accProps.maxInstanceCount;
    caps->maxPrimitiveCount = accProps.maxPrimitiveCount;
    caps->maxGeometryCount = accProps.maxGeometryCount;

    caps->maxPayloadSize = 64; // safe
    caps->maxDispatchInvocations = props.maxRayDispatchInvocationCount;

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryDescriptorIndexingCapabilitiesVk(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPhysicalDeviceDescriptorIndexingFeaturesEXT desc = {0};
    desc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceDescriptorIndexingPropertiesEXT props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

    VkPhysicalDeviceAccelerationStructurePropertiesKHR accProps = {0};
    accProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
    
    features.pNext = &desc;
    props.pNext = &accProps;
    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceFeatures2(vkDevice->phyDevice, &features);
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &properties2);

    // check sub feature for sampled image
    if (desc.shaderSampledImageArrayNonUniformIndexing) {
        caps->sampledImageNonUniformIndexing = PAL_TRUE;
    }

    if (desc.descriptorBindingSampledImageUpdateAfterBind) {
        caps->sampledImageUpdateAfterBind = PAL_TRUE;
    }

    // check sub feature for storage image
    if (desc.shaderStorageImageArrayNonUniformIndexing) {
        caps->storageImageNonUniformIndexing = PAL_TRUE;
    }

    if (desc.descriptorBindingStorageImageUpdateAfterBind) {
        caps->storageImageUpdateAfterBind = PAL_TRUE;
    }

    // check sub feature for storage buffer
    if (desc.shaderStorageBufferArrayNonUniformIndexing) {
        caps->storageBufferNonUniformIndexing = PAL_TRUE;
    }

    if (desc.descriptorBindingStorageBufferUpdateAfterBind) {
        caps->storageBufferUpdateAfterBind = PAL_TRUE;
    }

    // check sub feature for uniform buffer
    if (desc.shaderUniformBufferArrayNonUniformIndexing) {
        caps->uniformBufferNonUniformIndexing = PAL_TRUE;
    }

    if (desc.descriptorBindingUniformBufferUpdateAfterBind) {
        caps->uniformBufferUpdateAfterBind = PAL_TRUE;
    }

    caps->maxPerStageSampledImages = props.maxPerStageDescriptorUpdateAfterBindSampledImages;
    caps->maxPerSetSampledImages = props.maxDescriptorSetUpdateAfterBindSampledImages;
    caps->maxPerStageStorageImages = props.maxPerStageDescriptorUpdateAfterBindStorageImages;
    caps->maxPerSetStorageImages = props.maxDescriptorSetUpdateAfterBindStorageImages;

    caps->maxPerStageSamplers = props.maxPerStageDescriptorUpdateAfterBindSamplers;
    caps->maxPerSetSamplers = props.maxDescriptorSetUpdateAfterBindSamplers;
    caps->maxPerStageStorageBuffers = props.maxPerStageDescriptorUpdateAfterBindStorageBuffers;
    caps->maxPerSetStorageBuffers = props.maxDescriptorSetUpdateAfterBindStorageBuffers;

    caps->maxPerStageUniformBuffers = props.maxPerStageDescriptorUpdateAfterBindUniformBuffers;
    caps->maxPerSetUniformBuffers = props.maxDescriptorSetUpdateAfterBindUniformBuffers;

    uint32_t tmp = accProps.maxPerStageDescriptorUpdateAfterBindAccelerationStructures;
    uint32_t tmp2 = accProps.maxDescriptorSetUpdateAfterBindAccelerationStructures;
    caps->maxPerStageAccelerationStructure = tmp;
    caps->maxPerSetAccelerationStructure = tmp2;

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Queue
// ==================================================

PalResult PAL_CALL createQueueVk(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue)
{
    Device* vkDevice = (Device*)device;
    VkQueueFlags queueFlag = 0;
    Queue* queue = nullptr;

    if (vkDevice->phyQueueCount == 0) {
        return PAL_RESULT_OUT_OF_QUEUE;
    }

    switch (type) {
        case PAL_QUEUE_TYPE_COMPUTE: {
            queueFlag = VK_QUEUE_COMPUTE_BIT;
            break;
        }

        case PAL_QUEUE_TYPE_GRAPHICS: {
            queueFlag = VK_QUEUE_GRAPHICS_BIT;
            break;
        }

        case PAL_QUEUE_TYPE_COPY: {
            queueFlag = VK_QUEUE_TRANSFER_BIT;
            break;
        }
    }

    // we index the for loop so we dont always start at the beginning, this way
    // we cycle through all queue families each time we create a queue
    PhysicalQueue* phyQueue = nullptr;
    for (int i = vkDevice->phyQueueIndex; i < vkDevice->phyQueueCount; i++) {
        PhysicalQueue* pq = &vkDevice->phyQueues[i];
        // check if the physical queue supports the requested operation
        // and if its not already used
        if (pq->usages & queueFlag && pq->usedUsages != queueFlag) {
            pq->usedUsages |= queueFlag;
            phyQueue = pq;
            break;
        }
    }

    if (!phyQueue) {
        // we didnt get any queue, we check if we started the loop at the beginning or mid way
        if (vkDevice->phyQueueIndex == 0) {
            // we searched all queue families
            return PAL_RESULT_OUT_OF_QUEUE;

        } else {
            // we start at the beginning and go through the queue families again
            vkDevice->phyQueueIndex = 0;
            for (int i = vkDevice->phyQueueIndex; i < vkDevice->phyQueueCount; i++) {
                PhysicalQueue* pq = &vkDevice->phyQueues[i];
                if (pq->usages & queueFlag && pq->usedUsages != queueFlag) {
                    pq->usedUsages |= queueFlag;
                    phyQueue = pq;
                    break;
                }
            }

            if (!phyQueue) {
                return PAL_RESULT_OUT_OF_QUEUE;
            }
        }
    }

    queue = palAllocate(s_Vk.allocator, sizeof(Queue), 0);
    if (!queue) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    vkDevice->phyQueueIndex = (vkDevice->phyQueueIndex + 1) % vkDevice->phyQueueCount;
    queue->phyQueue = phyQueue;
    queue->usage = queueFlag;
    queue->device = vkDevice;

    *outQueue = (PalQueue*)queue;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyQueueVk(PalQueue* queue)
{
    Queue* vkQueue = (Queue*)queue;
    PhysicalQueue* phyQueue = vkQueue->phyQueue;
    phyQueue->usedUsages &= ~vkQueue->usage;
    palFree(s_Vk.allocator, vkQueue);
}

PalResult PAL_CALL waitQueueVk(PalQueue* queue)
{
    Queue* vkQueue = (Queue*)queue;
    VkResult result = s_Vk.waitQueue(vkQueue->phyQueue->handle);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalBool PAL_CALL canQueuePresentVk(
    PalQueue* queue,
    PalSurface* surface)
{
    VkResult result;
    Queue* vkQueue = (Queue*)queue;
    PhysicalQueue* phyQueue = vkQueue->phyQueue;
    Surface* vkSurface = (Surface*)surface;

    // check if the queue is a graphics queue before we check its family
    // index for presentation support.
    if (vkQueue->usage != VK_QUEUE_GRAPHICS_BIT) {
        return PAL_FALSE;
    }

    VkBool32 supported = PAL_FALSE;
    result = s_Vk.checkSurfaceSupport(
        phyQueue->phyDevice, 
        phyQueue->familyIndex, 
        vkSurface->handle, 
        &supported);

    if (result == VK_SUCCESS && supported) {
        return PAL_TRUE;
    }

    return PAL_FALSE;
}

// ==================================================
// Formats
// ==================================================

PalResult PAL_CALL enumerateFormatsVk(
    PalAdapter* adapter,
    int32_t* count,
    PalFormatInfo* outFormats)
{
    int32_t fmtCount = 0;
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
    VkFormatProperties props = {0};

    for (int i = 0; i < PAL_FORMAT_MAX; i++) {
        VkFormat fmt = formatToVk((PalFormat)i);
        s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
        if (props.optimalTilingFeatures != 0) {
            // format supported
            if (outFormats) {
                if (fmtCount < *count) {
                    PalFormatInfo* fmtInfo = &outFormats[fmtCount++];
                    fmtInfo->format = (PalFormat)i;
                    fmtInfo->usages = ImageUsageFromVk(props.optimalTilingFeatures);
                }

            } else {
                fmtCount++;
            }
        }
    }
    if (!outFormats) {
        *count = fmtCount;
    }
    return PAL_RESULT_SUCCESS;
}

PalBool PAL_CALL isFormatSupportedVk(
    PalAdapter* adapter,
    PalFormat format)
{
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
    VkFormatProperties props = {0};

    VkFormat fmt = formatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
    if (props.optimalTilingFeatures != 0) {
        return PAL_TRUE;
    }
    return PAL_FALSE;
}

PalImageUsages PAL_CALL queryFormatImageUsagesVk(
    PalAdapter* adapter,
    PalFormat format)
{
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
    VkFormatProperties props = {0};

    VkFormat fmt = formatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
    if (props.optimalTilingFeatures == 0) {
        return PAL_IMAGE_USAGE_UNDEFINED;
    }

    return ImageUsageFromVk(props.optimalTilingFeatures);
}

PalSampleCount PAL_CALL queryFormatSampleCountVk(
    PalAdapter* adapter,
    PalFormat format)
{
    VkResult result;
    Adapter* vkAdapter = (Adapter*)adapter;
    VkFormatProperties props = {0};
    VkImageFormatProperties formatProps = {0};

    VkFormat fmt = formatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(vkAdapter->handle, fmt, &props);
    if (props.optimalTilingFeatures == 0) {
        return PAL_SAMPLE_COUNT_1;
    }

    VkImageUsageFlags vkImageUsage = 0;
    PalImageUsages imageUsages = ImageUsageFromVk(props.optimalTilingFeatures);
    PalBool isDepth = (imageUsages & PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT) != 0;
    if (isDepth) {
        vkImageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    } else {
        vkImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    result = s_Vk.getPhysicalDeviceImageFormatProperties(
        vkAdapter->handle,
        fmt, 
        VK_IMAGE_TYPE_2D, 
        VK_IMAGE_TILING_OPTIMAL,
        vkImageUsage,
        0,
        &formatProps);

    if (result != VK_SUCCESS) {
        return PAL_SAMPLE_COUNT_1;
    }

    return samplesFromVk(formatProps.sampleCounts);
}

// ==================================================
// Image
// ==================================================



// ==================================================
// Surface
// ==================================================

PalResult PAL_CALL createSurfaceVk(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSurface** outSurface)
{
    VkResult result;
    Surface* surface = nullptr;
    Device* vkDevice = (Device*)device;

    surface = palAllocate(s_Vk.allocator, sizeof(Surface), 0);
    if (!surface) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

#ifdef _WIN32
    if (!s_Vk.createWin32Surface) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkWin32SurfaceCreateInfoKHR cInfo = {0};
    cInfo.hinstance = GetModuleHandle(nullptr);
    cInfo.hwnd = window->window;
    cInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;

    VkSurfaceKHR tmp = nullptr;
    result = s_Vk.createWin32Surface(s_Vk.instance, &cInfo, &s_Vk.vkAllocator, &tmp);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    surface->device = vkDevice;
    surface->handle = tmp;
    *outSurface = (PalSurface*)surface;
    return PAL_RESULT_SUCCESS;

#else
    if (window->displayType == PAL_GRAPHICS_WINDOW_DISPLAY_TYPE_WAYLAND) {
        if (!s_Vk.createWaylandSurface) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

        VkWaylandSurfaceCreateInfoKHR cInfo = {0};
        cInfo.display = window->display;
        cInfo.pNext = nullptr;
        cInfo.flags = 0;
        cInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
        cInfo.surface = window->window;

        VkSurfaceKHR tmp = nullptr;
        result = s_Vk.createWaylandSurface(s_Vk.instance, &cInfo, &s_Vk.vkAllocator, &tmp);
        if (result != VK_SUCCESS) {
            return makeResultVk(result);
        }

        surface->device = vkDevice;
        surface->handle = tmp;
        *outSurface = (PalSurface*)surface;
        return PAL_RESULT_SUCCESS;

    } else if (window->displayType == PAL_GRAPHICS_WINDOW_DISPLAY_TYPE_X11) {
        if (!s_Vk.createXlibSurface) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

        VkXlibSurfaceCreateInfoKHR cInfo = {0};
        cInfo.dpy = window->display;
        cInfo.window = (Window)(uintptr_t)(window->window);
        cInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;

        VkSurfaceKHR tmp = nullptr;
        result = s_Vk.createXlibSurface(s_Vk.instance, &cInfo, &s_Vk.vkAllocator, &tmp);
        if (result != VK_SUCCESS) {
            return makeResultVk(result);
        }

        surface->device = vkDevice;
        surface->handle = tmp;
        *outSurface = (PalSurface*)surface;
        return PAL_RESULT_SUCCESS;

    } else if (window->displayType == PAL_GRAPHICS_WINDOW_DISPLAY_TYPE_XCB) {
        if (!s_Vk.createXcbSurface) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

        VkXcbSurfaceCreateInfoKHR cInfo = {0};
        cInfo.connection = window->display;
        cInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;

        VkSurfaceKHR tmp = nullptr;
        result = s_Vk.createXcbSurface(s_Vk.instance, &cInfo, &s_Vk.vkAllocator, &tmp);
        if (result != VK_SUCCESS) {
            return makeResultVk(result);
        }

        surface->device = vkDevice;
        surface->handle = tmp;
        *outSurface = (PalSurface*)surface;
        return PAL_RESULT_SUCCESS;
    }

#endif // _WIN32
}

void PAL_CALL destroySurfaceVk(PalSurface* surface)
{
    Surface* vkSurface = (Surface*)surface;
    s_Vk.destroySurface(s_Vk.instance, vkSurface->handle, &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkSurface);
}

PalResult PAL_CALL getSurfaceCapabilitiesVk(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps)
{
    int32_t formatCount = 0;
    int32_t modeCount = 0;
    Surface* vkSurface = (Surface*)surface;
    VkSurfaceFormatKHR* formats = nullptr;
    VkPresentModeKHR* modes = nullptr;

    Device* vkDevice = (Device*)device;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkDevice->phyDevice;

    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    s_Vk.getSurfacePresentModes(phyDevice, vkSurface->handle, &modeCount, nullptr);
    s_Vk.getSurfaceFormats(phyDevice, vkSurface->handle, &formatCount, nullptr);

    modes = palAllocate(s_Vk.allocator, sizeof(VkPresentModeKHR) * modeCount, 0);
    formats = palAllocate(s_Vk.allocator, sizeof(VkSurfaceFormatKHR) * formatCount, 0);
    if (!modes || !formats) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_Vk.getSurfacePresentModes(phyDevice, vkSurface->handle, &modeCount, modes);
    s_Vk.getSurfaceFormats(phyDevice, vkSurface->handle, &formatCount, formats);

    VkSurfaceCapabilitiesKHR surfaceCaps;
    s_Vk.getSurfaceCapabilities(phyDevice, vkSurface->handle, &surfaceCaps);

    caps->minImageWidth = surfaceCaps.minImageExtent.width;
    caps->minImageHeight = surfaceCaps.minImageExtent.height;
    caps->maxImageWidth = surfaceCaps.maxImageExtent.width;
    caps->maxImageHeight = surfaceCaps.maxImageExtent.height;

    caps->maxImageCount = surfaceCaps.maxImageCount;
    caps->minImageCount = surfaceCaps.minImageCount;
    caps->maxImageArrayLayers = surfaceCaps.maxImageArrayLayers;

    if (caps->maxImageCount == 0) {
        caps->maxImageCount = 8; // safe
    }

    // get supported composite alphas
    VkCompositeAlphaFlagsKHR alpha = surfaceCaps.supportedCompositeAlpha;
    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_OPAQUE] = PAL_TRUE;
    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_POST_MULTIPLIED] = PAL_FALSE;
    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED] = PAL_FALSE;

    if (alpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
        caps->compositeAlphas[PAL_COMPOSITE_ALPHA_POST_MULTIPLIED] = PAL_TRUE;
    }

    if (alpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
        caps->compositeAlphas[PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED] = PAL_TRUE;
    }

    // present modes
    caps->presentModes[PAL_PRESENT_MODE_FIFO] = PAL_TRUE;
    caps->presentModes[PAL_PRESENT_MODE_MAILBOX] = PAL_FALSE;
    caps->presentModes[PAL_PRESENT_MODE_IMMEDIATE] = PAL_FALSE;

    for (int i = 0; i < modeCount; i++) {
        if (modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            caps->presentModes[PAL_PRESENT_MODE_IMMEDIATE] = PAL_TRUE;
        }

        if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            caps->presentModes[PAL_PRESENT_MODE_MAILBOX] = PAL_TRUE;
        }
    }

    // get format and colorspace
    caps->formats[PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10] = PAL_FALSE;
    caps->formats[PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR] = PAL_FALSE;
    caps->formats[PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR] = PAL_FALSE;
    caps->formats[PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR] = PAL_FALSE;

    for (int i = 0; i < formatCount; i++) {
        VkSurfaceFormatKHR* fmt = &formats[i];
        if (fmt->format == VK_FORMAT_B8G8R8A8_UNORM) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                caps->formats[PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR] = PAL_TRUE;
            }

        } else if (fmt->format == VK_FORMAT_B8G8R8A8_SRGB) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                caps->formats[PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR] = PAL_TRUE;
            }

        } else if (fmt->format == VK_FORMAT_R8G8B8A8_UNORM) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                caps->formats[PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR] = PAL_TRUE;
            }

        } else if (fmt->format == VK_FORMAT_R16G16B16A16_SFLOAT) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT) {
                caps->formats[PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10] = PAL_TRUE;
            }
        }
    }

    palFree(s_Vk.allocator, formats);
    palFree(s_Vk.allocator, modes);
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Swapchain
// ==================================================

PalResult PAL_CALL createSwapchainVk(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain)
{
    PalFormat imageFormat = 0;
    Swapchain* swapchain = nullptr;
    VkImage* images = nullptr;

    Device* vkDevice = (Device*)device;
    Queue* vkQueue = (Queue*)queue;
    PhysicalQueue* phyQueue = vkQueue->phyQueue;
    Surface* vkSurface = (Surface*)surface;

    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    // check if the queue is a graphics queue before we check its family
    // index for presentation support.
    if (vkQueue->usage != VK_QUEUE_GRAPHICS_BIT) {
        return PAL_RESULT_INVALID_QUEUE;
    }

    swapchain = palAllocate(s_Vk.allocator, sizeof(Swapchain), 0);
    if (!swapchain) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }
    memset(swapchain, 0, sizeof(Swapchain));

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = vkSurface->handle;
    createInfo.imageArrayLayers = info->imageArrayLayerCount;
    createInfo.imageExtent.width = info->width;
    createInfo.imageExtent.height = info->height;
    createInfo.minImageCount = info->imageCount;
    createInfo.clipped = info->clipped;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

    // present modes
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (info->presentMode == PAL_PRESENT_MODE_IMMEDIATE) {
        createInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

    } else if (info->presentMode == PAL_PRESENT_MODE_MAILBOX) {
        createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    }

    // composite alpha
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    if (info->compositeAlpha == PAL_COMPOSITE_ALPHA_POST_MULTIPLIED) {
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;

    } else if (info->compositeAlpha == PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED) {
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
    }

    // format and colorspace
    createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    imageFormat = PAL_FORMAT_B8G8R8A8_UNORM;

    if (info->format == PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR) {
        createInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
        createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        imageFormat = PAL_FORMAT_B8G8R8A8_SRGB;

    } else if (info->format == PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR) {
        createInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
        createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        imageFormat = PAL_FORMAT_R8G8B8A8_UNORM;

    } else if (info->format == PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10) {
        createInfo.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        createInfo.imageColorSpace = VK_COLOR_SPACE_HDR10_ST2084_EXT;
        imageFormat = PAL_FORMAT_R16G16B16A16_SFLOAT;
    }

    // create swapchain
    VkResult result = vkDevice->createSwapchain(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &swapchain->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, swapchain);
        return makeResultVk(result);
    }

    // get and cache all images
    int32_t count = 0;
    result = vkDevice->getSwapchainImages(vkDevice->handle, swapchain->handle, &count, nullptr);

    swapchain->images = palAllocate(s_Vk.allocator, sizeof(Image) * count, 0);
    images = palAllocate(s_Vk.allocator, sizeof(VkImage) * count, 0);
    if (!swapchain->images || !images) {
        vkDevice->destroySwapchain(vkDevice->handle, swapchain->handle, &s_Vk.vkAllocator);
        palFree(s_Vk.allocator, swapchain);
        return PAL_RESULT_OUT_OF_MEMORY;
    }
    vkDevice->getSwapchainImages(vkDevice->handle, swapchain->handle, &count, images);

    // fill all images with the creatio info
    for (int i = 0; i < count; i++) {
        Image* image = &swapchain->images[i];
        image->belongsToSwapchain = PAL_TRUE;
        image->device = vkDevice;
        image->handle = images[i];

        image->info.depthOrArraySize = createInfo.imageArrayLayers;
        image->info.format = imageFormat;
        image->info.usages = PAL_IMAGE_USAGE_COLOR_ATTACHEMENT;
        image->info.height = createInfo.imageExtent.height;
        image->info.width = createInfo.imageExtent.width;
        image->info.mipLevelCount = 1;
        image->info.sampleCount = PAL_SAMPLE_COUNT_1; // swapchain images are not multisampled
        image->info.type = PAL_IMAGE_TYPE_2D;
    }

    palFree(s_Vk.allocator, images);
    swapchain->device = vkDevice;
    swapchain->queue = vkQueue;
    swapchain->imageCount = count;

    *outSwapchain = (PalSwapchain*)swapchain;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySwapchainVk(PalSwapchain* swapchain)
{
    Swapchain* vkSwapchain = (Swapchain*)swapchain;
    vkSwapchain->device->destroySwapchain(
        vkSwapchain->device->handle,
        vkSwapchain->handle,
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkSwapchain->images);
    palFree(s_Vk.allocator, vkSwapchain);
}

PalImage* PAL_CALL getSwapchainImageVk(
    PalSwapchain* swapchain,
    int32_t index)
{
    Swapchain* vkSwapchain = (Swapchain*)swapchain;
    if (index > vkSwapchain->imageCount) {
        return nullptr;
    }
    return (PalImage*)&vkSwapchain->images[index];
}

PalResult PAL_CALL getNextSwapchainImageVk(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    uint32_t* outIndex)
{
    VkResult result;
    uint32_t index = 0;
    uint64_t timeInNanoseconds = 0;
    VkFence fenceHandle = nullptr;
    VkSemaphore semaphoreHandle = nullptr;
    Swapchain* vkSwapchain = (Swapchain*)swapchain;

    if (info->fence) {
        Fence* vkFence = (Fence*)info->fence;
        fenceHandle = vkFence->handle;
    }

    if (info->signalSemaphore) {
        Semaphore* vkSemaphore = (Semaphore*)info->signalSemaphore;
        semaphoreHandle = vkSemaphore->handle;
    }

    if (info->timeout) {
        if (info->timeout == PAL_INFINITE) {
            timeInNanoseconds = UINT64_MAX;
        } else {
            timeInNanoseconds = info->timeout * 1000000;
        }
    }

    result = vkSwapchain->device->acquireNextImage(
        vkSwapchain->device->handle,
        vkSwapchain->handle,
        timeInNanoseconds,
        semaphoreHandle,
        fenceHandle,
        &index);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    *outIndex = index;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL presentSwapchainVk(
    PalSwapchain* swapchain,
    PalSwapchainPresentInfo* info)
{
    Swapchain* vkSwapchain = (Swapchain*)swapchain;
    int32_t semaphoreCount = 0;
    VkSemaphore semaphoreHandle = nullptr;
    if (info->waitSemaphore) {
        Semaphore* vkSemaphore = (Semaphore*)info->waitSemaphore;
        semaphoreHandle = vkSemaphore->handle;
        semaphoreCount = 1;
    }

    VkResult result;
    VkPresentInfoKHR presentInfo = {0};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vkSwapchain->handle;
    presentInfo.pImageIndices = &info->imageIndex;
    presentInfo.pWaitSemaphores = &semaphoreHandle;
    presentInfo.waitSemaphoreCount = semaphoreCount;

    result = vkSwapchain->device->queuePresent(vkSwapchain->queue->phyQueue->handle, &presentInfo);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resizeSwapchainVk(
    PalSwapchain* swapchain,
    uint32_t newWidth,
    uint32_t newHeight)
{
    VkResult result;
    Swapchain* vkSwapchain = (Swapchain*)swapchain;
    VkSwapchainKHR oldSwapchain = vkSwapchain->handle;
    Device* device = vkSwapchain->device;
    VkImage* images = nullptr;

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.oldSwapchain = oldSwapchain;
    createInfo.imageExtent.width = newWidth;
    createInfo.imageExtent.height = newHeight;

    result = device->createSwapchain(
        device->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &vkSwapchain->handle);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    uint32_t count = vkSwapchain->imageCount;
    images = palAllocate(s_Vk.allocator, sizeof(VkImage) * count, 0);
    if (!images) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    device->destroySwapchain(device->handle, oldSwapchain, &s_Vk.vkAllocator);
    device->getSwapchainImages(device->handle, vkSwapchain->handle, &count, images);

    // fill all images with the creatio info
    for (int i = 0; i < count; i++) {
        Image* image = &vkSwapchain->images[i];
        image->handle = images[i];
        image->info.height = createInfo.imageExtent.height;
        image->info.width = createInfo.imageExtent.width;
    }

    palFree(s_Vk.allocator, images);
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Shader
// ==================================================

PalResult PAL_CALL createShaderVk(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader)
{
    VkResult result;
    Shader* shader = nullptr;
    Device* vkDevice = (Device*)device;

    shader = palAllocate(s_Vk.allocator, sizeof(Shader), 0);
    if (!shader) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // allocate entries array
    shader->entries = palAllocate(s_Vk.allocator, sizeof(ShaderEntry) * info->entryCount, 0);
    if (!shader->entries) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    for (int i = 0; i < info->entryCount; i++) {
        ShaderEntry* entry = &shader->entries[i];

        // clang-format off
        if (info->entries[i].stage == PAL_SHADER_STAGE_MESH || 
            info->entries[i].stage == PAL_SHADER_STAGE_TASK) {
            if (!(vkDevice->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
                return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
            }

        } else if (info->entries[i].stage == PAL_SHADER_STAGE_GEOMETRY) {
            if (!(vkDevice->features & PAL_ADAPTER_FEATURE_GEOMETRY_SHADER)) {
                return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
            }

        } else if (info->entries[i].stage == PAL_SHADER_STAGE_TESSELLATION_CONTROL || 
                info->entries[i].stage == PAL_SHADER_STAGE_TESSELLATION_EVALUATION) {
            if (!(vkDevice->features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER)) {
                return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
            }

        } else if (info->entries[i].stage == PAL_SHADER_STAGE_RAYGEN ||
                info->entries[i].stage == PAL_SHADER_STAGE_CLOSEST_HIT ||
                info->entries[i].stage == PAL_SHADER_STAGE_ANY_HIT ||
                info->entries[i].stage == PAL_SHADER_STAGE_MISS ||
                info->entries[i].stage == PAL_SHADER_STAGE_INTERSECTION ||
                info->entries[i].stage == PAL_SHADER_STAGE_CALLABLE) {
            if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
                return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
            }
        }
        // clang-format on

        strncpy(entry->entryName, info->entries[i].entryName, PAL_SHADER_ENTRY_NAME_SIZE);
        entry->entryName[PAL_SHADER_ENTRY_NAME_SIZE - 1] = '\0';
        entry->patchControlPoints = info->entries[i].patchControlPoints;
        entry->stage = shaderStageToVK(info->entries[i].stage); 
    }

    VkShaderModuleCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = info->bytecodeSize;
    createInfo.pCode = (const uint32_t*)info->bytecode;

    result = s_Vk.createShader(vkDevice->handle, &createInfo, &s_Vk.vkAllocator, &shader->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, shader);
        return makeResultVk(result);
    }

    shader->device = vkDevice;
    shader->entryCount = info->entryCount;
    *outShader = (PalShader*)shader;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyShaderVk(PalShader* shader)
{
    Shader* vkShader = (Shader*)shader;
    s_Vk.destroyShader(vkShader->device->handle, vkShader->handle, &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkShader->entries);
    palFree(s_Vk.allocator, vkShader);
}

// ==================================================
// Fence
// ==================================================

PalResult PAL_CALL createFenceVk(
    PalDevice* device,
    PalBool signaled,
    PalFence** outFence)
{
    VkResult result;
    Fence* fence = nullptr;
    Device* vkDevice = (Device*)device;

    fence = palAllocate(s_Vk.allocator, sizeof(Fence), 0);
    if (!fence) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkFenceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (signaled) {
        createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    result = s_Vk.createFence(vkDevice->handle, &createInfo, &s_Vk.vkAllocator, &fence->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, fence);
        return makeResultVk(result);
    }

    fence->device = vkDevice;
    *outFence = (PalFence*)fence;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyFenceVk(PalFence* fence)
{
    Fence* vkFence = (Fence*)fence;
    s_Vk.destroyFence(vkFence->device->handle, vkFence->handle, &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkFence);
}

PalResult PAL_CALL waitFenceVk(
    PalFence* fence,
    uint64_t timeout)
{
    Fence* vkFence = (Fence*)fence;
    VkResult result;
    uint64_t timeInNanoseconds = 0;

    if (timeout) {
        if (timeout == PAL_INFINITE) {
            timeInNanoseconds = UINT64_MAX;
        } else {
            timeInNanoseconds = timeout * 1000000;
        }
    }

    result = s_Vk.waitFence(vkFence->device->handle, 1, &vkFence->handle, PAL_TRUE, timeInNanoseconds);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resetFenceVk(PalFence* fence)
{
    Fence* vkFence = (Fence*)fence;
    if (!(vkFence->device->features & PAL_ADAPTER_FEATURE_FENCE_RESET)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkResult result = s_Vk.resetFence(vkFence->device->handle, 1, &vkFence->handle);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalBool PAL_CALL isFenceSignaledVk(PalFence* fence)
{
    Fence* vkFence = (Fence*)fence;
    VkResult result = s_Vk.isFenceSignaled(vkFence->device->handle, vkFence->handle);
    if (result == VK_SUCCESS) {
        return PAL_TRUE;
    } else {
        return PAL_FALSE;
    }
}

// ==================================================
// Semaphore
// ==================================================

PalResult PAL_CALL createSemaphoreVk(
    PalDevice* device,
    PalBool enableTimeline,
    PalSemaphore** outSemaphore)
{
    VkResult result;
    Semaphore* semaphore = nullptr;
    Device* vkDevice = (Device*)device;
    PalBool hasTimeline = vkDevice->features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE;

    semaphore = palAllocate(s_Vk.allocator, sizeof(Semaphore), 0);
    if (!semaphore) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkSemaphoreTypeCreateInfo timelineCreateInfo = {0};
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;

    VkSemaphoreCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    const void* next = nullptr;
    semaphore->isTimeline = PAL_FALSE;
    if (enableTimeline && !hasTimeline) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (enableTimeline) {
        next = &timelineCreateInfo;
        semaphore->isTimeline = PAL_TRUE;   
    }

    createInfo.pNext = next;
    result = s_Vk.createSemaphore(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &semaphore->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, semaphore);
        return makeResultVk(result);
    }

    semaphore->device = vkDevice;
    *outSemaphore = (PalSemaphore*)semaphore;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySemaphoreVk(PalSemaphore* semaphore)
{
    Semaphore* vkSemaphore = (Semaphore*)semaphore;
    s_Vk.destroySemaphore(vkSemaphore->device->handle, vkSemaphore->handle, &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkSemaphore);
}

PalResult PAL_CALL waitSemaphoreVk(
    PalSemaphore* semaphore,
    uint64_t value,
    uint64_t timeout)
{
    VkResult result;
    uint64_t timeInNanoseconds = 0;
    Semaphore* vkSemaphore = (Semaphore*)semaphore;
    if (!vkSemaphore->isTimeline) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (timeout) {
        if (timeout == PAL_INFINITE) {
            timeInNanoseconds = UINT64_MAX;
        } else {
            timeInNanoseconds = timeout * 1000000;
        }
    }

    VkSemaphoreWaitInfo waitInfo = {0};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores = &vkSemaphore->handle;
    waitInfo.pValues = &value;

    result = vkSemaphore->device->waitSemaphore(
        vkSemaphore->device->handle, 
        &waitInfo, 
        timeInNanoseconds);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL signalSemaphoreVk(
    PalSemaphore* semaphore,
    PalQueue* queue,
    uint64_t value)
{
    VkResult result;
    Semaphore* vkSemaphore = (Semaphore*)semaphore;
    if (!vkSemaphore->isTimeline) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkSemaphoreSignalInfo signalInfo = {0};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signalInfo.semaphore = vkSemaphore->handle;
    signalInfo.value = value;

    result = vkSemaphore->device->signalSemaphore(vkSemaphore->device->handle, &signalInfo);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getSemaphoreValueVk(
    PalSemaphore* semaphore,
    uint64_t* outValue)
{
    Semaphore* vkSemaphore = (Semaphore*)semaphore;
    if (!vkSemaphore->isTimeline) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkResult result = vkSemaphore->device->getSemaphoreValue(
        vkSemaphore->device->handle,
        vkSemaphore->handle,
        outValue);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_VULKAN_BACKEND
