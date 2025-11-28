
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

// ==================================================
// Includes
// ==================================================

#include "pal/pal_graphics.h"

#if PAL_HAS_VULKAN
#include <vulkan/vulkan_core.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#endif // PAL_HAS_VULKAN

// ==================================================
// Typedefs, enums and structs
// ==================================================

// TODO: make all these dynamic
#define MAX_BACKENDS 16 // should be fine for now
#define MAX_ADAPTERS 32 // should be enough
#define MAX_DEVICE 16 // should be enough
#define MAX_QUEUE_FAMILIES 8
#define MAX_PHYSICAL_QUEUES 16
#define MAX_COMMAND_QUEUES 32

#if PAL_HAS_VULKAN
// VKAPI_PTR expands to nothing on linux

typedef VkResult (*vkEnumerateInstanceVersionFn)(uint32_t*);

typedef VkResult (*vkEnumerateInstanceExtensionPropertiesFn)(
    const char*, 
    uint32_t*, 
    VkExtensionProperties*);

typedef VkResult (*vkCreateInstanceFn)(
    const VkInstanceCreateInfo*, 
    const VkAllocationCallbacks*, 
    VkInstance*);

typedef void (*vkDestroyInstanceFn)(
    VkInstance, 
    const VkAllocationCallbacks*);

typedef VkResult (*vkEnumeratePhysicalDevicesFn)(
    VkInstance, 
    uint32_t*, 
    VkPhysicalDevice*);

typedef void (*vkGetPhysicalDevicePropertiesFn)(
    VkPhysicalDevice, 
    VkPhysicalDeviceProperties*);

typedef void (*vkGetPhysicalDeviceMemoryPropertiesFn)(
    VkPhysicalDevice, 
    VkPhysicalDeviceMemoryProperties*);

typedef VkResult (*vkEnumerateInstanceLayerPropertiesFn)(
    uint32_t*, 
    VkLayerProperties*);

typedef void (*vkGetPhysicalDeviceQueueFamilyPropertiesFn)(
    VkPhysicalDevice, 
    uint32_t*, 
    VkQueueFamilyProperties*);

typedef VkResult (*vkEnumerateDeviceExtensionPropertiesFn)(
    VkPhysicalDevice, 
    const char*, 
    uint32_t*, 
    VkExtensionProperties*);

typedef void (*vkGetPhysicalDeviceFeaturesFn)(
    VkPhysicalDevice, 
    VkPhysicalDeviceFeatures*);

typedef void (*vkGetPhysicalDeviceFeatures2Fn)(
    VkPhysicalDevice, 
    VkPhysicalDeviceFeatures2*);

typedef void (*vkGetPhysicalDeviceFeatures2KHRFn)(
    VkPhysicalDevice, 
    VkPhysicalDeviceFeatures2*);

typedef VkResult (*vkCreateDeviceFn)(
    VkPhysicalDevice,
    const VkDeviceCreateInfo*,
    const VkAllocationCallbacks*,
    VkDevice*);

typedef void (*vkDestroyDeviceFn)(
    VkDevice,
    const VkAllocationCallbacks*);

typedef void (*vkGetDeviceQueueFn)(
    VkDevice,
    uint32_t,
    uint32_t,
    VkQueue*);

typedef struct {
    bool hasDebug;
    bool versionFallback;
    void* handle;
    VkInstance instance;

    vkEnumerateInstanceVersionFn enumerateInstanceVersion;
    vkEnumerateInstanceExtensionPropertiesFn enumerateInstanceExtensionProperties;
    vkDestroyInstanceFn destroyInstance;
    vkCreateInstanceFn createInstance;
    vkEnumeratePhysicalDevicesFn enumeratePhysicalDevices;
    vkGetPhysicalDevicePropertiesFn getPhysicalDeviceProperties;
    vkGetPhysicalDeviceMemoryPropertiesFn getPhysicalDeviceMemoryProperties;
    vkEnumerateInstanceLayerPropertiesFn enumerateInstanceLayerProperties;
    vkGetPhysicalDeviceQueueFamilyPropertiesFn getPhysicalDeviceQueueFamilyProperties;
    vkEnumerateDeviceExtensionPropertiesFn enumerateDeviceExtensionProperties;
    vkGetPhysicalDeviceFeaturesFn getPhysicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures2Fn getPhysicalDeviceFeatures2;
    vkGetPhysicalDeviceFeatures2KHRFn getPhysicalDeviceFeatures2KHR;

    vkCreateDeviceFn createDevice;
    vkDestroyDeviceFn destroyDevice;
    vkGetDeviceQueueFn getDeviceQueue;

    VkAllocationCallbacks allocator;
} Vulkan;

typedef struct {
    Int32 count;
    Int32 index;
    VkQueueFlags flags;
} QueueFamilyData;

typedef struct {
    VkQueue handle;
    VkQueueFlags usages;
    VkQueueFlags usedUsages;
} PhysicalQueue;

typedef struct {
    VkQueueFlags usage;
    PhysicalQueue* phyQueue;
} VkCommandQueue;

typedef struct {
    Int32 queueCount;
    VkDevice handle;
    PhysicalQueue queues[MAX_PHYSICAL_QUEUES];
} VkGPUDevice;

static Vulkan s_Vk = {0};

#endif // PAL_HAS_VULKAN

typedef struct {
    bool used;
    const PalGPUBackend* backend;
    PalGPUAdapter* adapter;
} AdapterData;

typedef struct {
    bool used;
    PalGPUDevice* device;
    const PalGPUBackend* backend;
} DeviceData;

typedef struct {
    bool used;
    PalGPUDevice* device;
    PalGPUCommandQueue* queue;
    const PalGPUBackend* backend;
} CommandQueueData;

typedef struct {
    const PalGPUBackend* base;
    Uint16 startIndex;
    Uint16 count;
} AttachBackend;

typedef struct {
    bool initialized;
    Int32 backendCount;
    Int32 totalAdapterCount;
    const PalAllocator* allocator;
    AdapterData adapterData[MAX_ADAPTERS];
    DeviceData deviceData[MAX_DEVICE];
    CommandQueueData commandQueueData[MAX_COMMAND_QUEUES];
    AttachBackend backends[MAX_BACKENDS];
} GraphicsLinux;

static GraphicsLinux s_Graphics = {0};

// ==================================================
// Internal API
// ==================================================

static AdapterData* getFreeAdapterData()
{
    for (int i = 0; i < MAX_ADAPTERS; ++i) {
        if (!s_Graphics.adapterData[i].used) {
            s_Graphics.adapterData[i].used = true;
            return &s_Graphics.adapterData[i];
        }
    }  
    return nullptr;
}

static AdapterData* findAdapterData(PalGPUAdapter* adapter)
{
    for (int i = 0; i < MAX_ADAPTERS; ++i) {
        if (s_Graphics.adapterData[i].used &&
            s_Graphics.adapterData[i].adapter == adapter) {
            return &s_Graphics.adapterData[i];
        }
    }
    return nullptr;
}

static DeviceData* getFreeDeviceData()
{
    for (int i = 0; i < MAX_DEVICE; ++i) {
        if (!s_Graphics.deviceData[i].used) {
            s_Graphics.deviceData[i].used = true;
            return &s_Graphics.deviceData[i];
        }
    }  
    return nullptr;
}

static DeviceData* findDeviceData(PalGPUDevice* device)
{
    for (int i = 0; i < MAX_DEVICE; ++i) {
        if (s_Graphics.deviceData[i].used &&
            s_Graphics.deviceData[i].device == device) {
            return &s_Graphics.deviceData[i];
        }
    }
    return nullptr;
}

static CommandQueueData* getFreeCommandQueueData()
{
    for (int i = 0; i < MAX_COMMAND_QUEUES; ++i) {
        if (!s_Graphics.commandQueueData[i].used) {
            s_Graphics.commandQueueData[i].used = true;
            return &s_Graphics.commandQueueData[i];
        }
    }  
    return nullptr;
}

static CommandQueueData* findCommandQueueData(PalGPUCommandQueue* queue)
{
    for (int i = 0; i < MAX_COMMAND_QUEUES; ++i) {
        if (s_Graphics.commandQueueData[i].used &&
            s_Graphics.commandQueueData[i].queue == queue) {
            return &s_Graphics.commandQueueData[i];
        }
    }
    return nullptr;
}

#if PAL_HAS_VULKAN

static PalResult vkResultToPal(VkResult result) 
{
    switch (result) {
        case VK_ERROR_FEATURE_NOT_PRESENT:
        case VK_ERROR_EXTENSION_NOT_PRESENT: {
            return PAL_RESULT_GPU_FEATURE_NOT_SUPPORTED;
        }

        case VK_ERROR_OUT_OF_HOST_MEMORY:
        case VK_ERROR_TOO_MANY_OBJECTS:
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        case VK_ERROR_INITIALIZATION_FAILED: 
        case VK_ERROR_DEVICE_LOST: {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return PAL_RESULT_INVALID_GRAPHICS_DRIVER;

        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        case VK_ERROR_SURFACE_LOST_KHR: {
            return PAL_RESULT_INVALID_WINDOW;
        }

        case VK_TIMEOUT:
            return PAL_RESULT_TIMEOUT;

        default:
            return PAL_RESULT_PLATFORM_FAILURE;
    }
    return PAL_RESULT_PLATFORM_FAILURE;
}

static void* vkAlloc(
    void* pUserData,
    size_t size,
    size_t alignment,
    VkSystemAllocationScope allocationScope)
{
    return palAllocate(s_Graphics.allocator, size, alignment);
}

static void vkFree(
    void* pUserData,
    void* ptr)
{
    palFree(s_Graphics.allocator, ptr);
}

static void* vkRealloc(
    void* pUserData,
    void* pOriginal,
    size_t size,
    size_t alignment,
    VkSystemAllocationScope allocationScope)
{
    // Note: This is a hack which could cost performance but 
    // realloc is not really called that much so it should be fine
    // this is because we dont know the old size
    void* block = realloc(pOriginal, size);
    if (block) {
        void* memory = palAllocate(s_Graphics.allocator, size, alignment);
        if (!memory) {
            free(block);
            return nullptr;
        }

        memcpy(memory, block, size);
        free(block);
        return memory;
    }
    return nullptr;
}

static PalResult vkInitGraphics(bool enableDebugLayer)
{
    // load vulkan
    s_Vk.handle = dlopen("libvulkan.so", RTLD_LAZY);
    if (!s_Vk.handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // clang-format off
    s_Vk.enumerateInstanceVersion = (vkEnumerateInstanceVersionFn)dlsym(
        s_Vk.handle, 
        "vkEnumerateInstanceVersion");

    s_Vk.enumerateInstanceExtensionProperties = (vkEnumerateInstanceExtensionPropertiesFn)dlsym(
        s_Vk.handle, 
        "vkEnumerateInstanceExtensionProperties");

    s_Vk.createInstance = (vkCreateInstanceFn)dlsym(
        s_Vk.handle, 
        "vkCreateInstance");

    s_Vk.destroyInstance = (vkDestroyInstanceFn)dlsym(
        s_Vk.handle, 
        "vkDestroyInstance");

    s_Vk.enumeratePhysicalDevices = (vkEnumeratePhysicalDevicesFn)dlsym(
        s_Vk.handle, 
        "vkEnumeratePhysicalDevices");

    s_Vk.getPhysicalDeviceProperties = (vkGetPhysicalDevicePropertiesFn)dlsym(
        s_Vk.handle, 
        "vkGetPhysicalDeviceProperties");

    s_Vk.getPhysicalDeviceMemoryProperties = (vkGetPhysicalDeviceMemoryPropertiesFn)dlsym(
        s_Vk.handle, 
        "vkGetPhysicalDeviceMemoryProperties");

    s_Vk.enumerateInstanceLayerProperties = (vkEnumerateInstanceLayerPropertiesFn)dlsym(
        s_Vk.handle, 
        "vkEnumerateInstanceLayerProperties");

    s_Vk.getPhysicalDeviceQueueFamilyProperties = (vkGetPhysicalDeviceQueueFamilyPropertiesFn)dlsym(
        s_Vk.handle, 
        "vkGetPhysicalDeviceQueueFamilyProperties");

    s_Vk.enumerateDeviceExtensionProperties = (vkEnumerateDeviceExtensionPropertiesFn)dlsym(
        s_Vk.handle, 
        "vkEnumerateDeviceExtensionProperties");

    s_Vk.getPhysicalDeviceFeatures = (vkGetPhysicalDeviceFeaturesFn)dlsym(
        s_Vk.handle, 
        "vkGetPhysicalDeviceFeatures");

    s_Vk.getPhysicalDeviceFeatures2 = (vkGetPhysicalDeviceFeatures2Fn)dlsym(
        s_Vk.handle, 
        "vkGetPhysicalDeviceFeatures2");

    s_Vk.createDevice = (vkCreateDeviceFn)dlsym(
        s_Vk.handle, 
        "vkCreateDevice");

    s_Vk.destroyDevice = (vkDestroyDeviceFn)dlsym(
        s_Vk.handle, 
        "vkDestroyDevice");

    s_Vk.getDeviceQueue = (vkGetDeviceQueueFn)dlsym(
        s_Vk.handle, 
        "vkGetDeviceQueue");
    
    // clang-format on

    // get version
    bool versionFallback = false;
    Uint32 version = 0;
    if (s_Vk.enumerateInstanceVersion) {
        s_Vk.enumerateInstanceVersion(&version);
        if (version <= VK_API_VERSION_1_0) {
            versionFallback = true;
        }
    }

    VkResult ret;
    Uint32 layerCount = 0;
    bool hasValidationLayer = false;
    if (enableDebugLayer) {
        // layers
        ret = s_Vk.enumerateInstanceLayerProperties(
            &layerCount, 
            nullptr);

        if (ret != VK_SUCCESS) {
            s_Vk.hasDebug = false;
        }

        VkLayerProperties* props = nullptr;
        props = palAllocate(
            s_Graphics.allocator, 
            sizeof(VkLayerProperties) * layerCount,
            0);

        if (!props) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        s_Vk.enumerateInstanceLayerProperties(&layerCount, props);
        for (int i = 0; i < layerCount; i++) {
            const char* name = props[i].layerName;
            if (strcmp(name, "VK_LAYER_KHRONOS_validation") == 0) {
                hasValidationLayer = true;
                break;
            }
        }

        palFree(s_Graphics.allocator, props);
    }

    // extensions
    Uint32 extCount = 0;
    const char* extensions[8];
    ret = s_Vk.enumerateInstanceExtensionProperties(
        nullptr,
        &extCount,
        nullptr);

    if (ret != VK_SUCCESS) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    VkExtensionProperties* extensionProps = nullptr;
    extensionProps = palAllocate(
        s_Graphics.allocator, 
        sizeof(VkExtensionProperties) * extCount, 
        0);

    if (!extensionProps) {
        return PAL_RESULT_SUCCESS;
    }

    s_Vk.enumerateInstanceExtensionProperties(
        nullptr, 
        &extCount, 
        extensionProps);

    bool hasXlib = false;
    bool hasXcb = false;
    bool hasWayland = false;
    bool hasSurface = false;
    bool hasExtDebug = false;

    for (int i = 0; i < extCount; i++) {
        VkExtensionProperties* prop = &extensionProps[i];
        if (strcmp(prop->extensionName, "VK_KHR_xlib_surface") == 0) {
            hasXlib = true;

        } else if (strcmp(prop->extensionName, "VK_KHR_xcb_surface") == 0) {
            hasXcb = true;

        } else if (strcmp(prop->extensionName, "VK_KHR_wayland_surface") == 0) {
            hasWayland = true;

        } else if (strcmp(prop->extensionName, "VK_KHR_surface") == 0) {
            hasSurface = true;

        } else if (strcmp(prop->extensionName, "VK_EXT_debug_utils") == 0) {
            hasExtDebug = true;
        }
    }

    palFree(s_Graphics.allocator, extensionProps);

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
    }

    const char* layers[2];
    layerCount = 0;
    if (hasValidationLayer && hasExtDebug) {
        s_Vk.hasDebug = true;
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

    // vk allocator
    s_Vk.allocator.pfnAllocation = vkAlloc;
    s_Vk.allocator.pfnFree = vkFree;
    s_Vk.allocator.pfnReallocation = vkRealloc;

    VkInstance instance = nullptr;
    VkResult result = s_Vk.createInstance(
        &instanceCreateInfo, 
        &s_Vk.allocator, 
        &instance);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    if (versionFallback) {
        // load get physical device properties2 proc if we are on version 1.0
        // clang-format off
        s_Vk.getPhysicalDeviceFeatures2KHR = (vkGetPhysicalDeviceFeatures2KHRFn)dlsym(
            s_Vk.handle, 
            "vkGetPhysicalDeviceFeatures2KHR");
        // clang-format on

        if (s_Vk.getPhysicalDeviceFeatures2KHR) {
            s_Vk.versionFallback = true;
        } else {
            s_Vk.versionFallback = false;
        }
    }

    s_Vk.instance = instance;
    return PAL_RESULT_SUCCESS;
}

static void vkShutdownGraphics()
{
    if (s_Vk.instance) {
        s_Vk.destroyInstance(s_Vk.instance, &s_Vk.allocator);
        dlclose(s_Vk.handle);
        memset(&s_Vk, 0, sizeof(s_Vk));
    }
}

static PalResult vkEnumerateAdapters(
    Int32* count, 
    PalGPUAdapter** outAdapters)
{
    int _count = 0;
    int maxCount = outAdapters ? *count : 0;
    VkResult result;

    result = s_Vk.enumeratePhysicalDevices(s_Vk.instance, &_count, nullptr);
    if (result != VK_SUCCESS) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    if (!outAdapters) {
        *count = _count;
        return PAL_RESULT_SUCCESS;
    }

    VkPhysicalDevice* devices = nullptr;
    devices = palAllocate(
        s_Graphics.allocator, 
        sizeof(VkPhysicalDevice) * _count,
        0);

    if (!devices) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_Vk.enumeratePhysicalDevices(s_Vk.instance, &_count, devices);
    for (int i = 0; i < _count && i < *count; i++) {
        outAdapters[i] = (PalGPUAdapter*)devices[i];
    }

    palFree(s_Graphics.allocator, devices);
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL vkGetAdapterInfo(
    PalGPUAdapter* adapter,
    PalGPUAdapterInfo* info)
{
    VkPhysicalDevice physicalDevice = (VkPhysicalDevice)adapter;
    VkPhysicalDeviceProperties props = {0};
    VkPhysicalDeviceMemoryProperties memProps = {0};

    s_Vk.getPhysicalDeviceMemoryProperties(physicalDevice, &memProps);
    s_Vk.getPhysicalDeviceProperties(physicalDevice, &props);

    info->apiType = PAL_GPU_API_TYPE_VULKAN;
    info->shaderFormats = PAL_GPU_SHADER_FORMAT_SPIRV;
    strcpy(info->name, props.deviceName);

    info->totalMemory = 0;
    for (int i = 0; i < memProps.memoryHeapCount; i++) {
        if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            info->totalMemory = memProps.memoryHeaps[i].size;
        }
    }

    // get device type
    switch (props.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: {
            info->type = PAL_GPU_TYPE_INTEGRATED;
            break;
        }

        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: {
            info->type = PAL_GPU_TYPE_DISCRETE;
            break;
        }

        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: {
            info->type = PAL_GPU_TYPE_VIRTUAL;
            break;
        }

        case VK_PHYSICAL_DEVICE_TYPE_CPU: {
            info->type = PAL_GPU_TYPE_CPU;
            break;
        }

        default: {
            info->type = PAL_GPU_TYPE_UNKNOWN;
            break;
        }
    }

    // version string
    snprintf(
        info->versionString, 
        PAL_GPU_VERSION_SIZE, 
        "%d.%d.%d",
        VK_VERSION_MAJOR(props.apiVersion),
        VK_VERSION_MINOR(props.apiVersion),
        VK_VERSION_PATCH(props.apiVersion));

    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL vkGetAdapterCapabilities(
    PalGPUAdapter* adapter,
    PalGPUAdapterCapabilities* caps)
{
    VkResult ret = VK_SUCCESS;
    VkPhysicalDevice physicalDevice = (VkPhysicalDevice)adapter;
    caps->debugLayerSupported = s_Vk.hasDebug;

    // get supported queue commands
    Uint32 count;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        &count, 
        nullptr);

    VkQueueFamilyProperties queueProps[MAX_QUEUE_FAMILIES];
    s_Vk.getPhysicalDeviceQueueFamilyProperties(
        physicalDevice, 
        &count, 
        queueProps);

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

    // get supported extensions
    Uint32 extensionCount = 0;
    ret = s_Vk.enumerateDeviceExtensionProperties(
        physicalDevice,
        nullptr, 
        &extensionCount, 
        nullptr);

    if (ret != VK_SUCCESS) {
        // we just return without any modern features which is rare
        return PAL_RESULT_SUCCESS;
    }

    VkExtensionProperties* extensionProps = nullptr;
    extensionProps = palAllocate(
        s_Graphics.allocator, 
        sizeof(VkExtensionProperties) * extensionCount, 
        0);

    if (!extensionProps) {
        return PAL_RESULT_SUCCESS;
    }

    s_Vk.enumerateDeviceExtensionProperties(
        physicalDevice, 
        nullptr, 
        &extensionCount, 
        extensionProps);

    bool rayTracingFound = false;
    bool accelerateFound = false;
    caps->features = 0;

    // clang-format off
    for (int i = 0; i < extensionCount; i++) {
        VkExtensionProperties* props = &extensionProps[i];
        if (strcmp(props->extensionName, "VK_KHR_ray_tracing_pipeline") == 0) {
            rayTracingFound = true;

        } else if (strcmp(props->extensionName, "VK_KHR_acceleration_structure") == 0) {
            accelerateFound = true;

        } else if (strcmp(props->extensionName, "VK_EXT_mesh_shader") == 0) {
            // mesh shader
            VkPhysicalDeviceMeshShaderFeaturesEXT mesh = {0};
            mesh.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &mesh;

            if (s_Vk.getPhysicalDeviceFeatures2KHR) {
                s_Vk.getPhysicalDeviceFeatures2KHR(physicalDevice, &features);

            } else {
                s_Vk.getPhysicalDeviceFeatures2(physicalDevice, &features);
            }

            if (mesh.meshShader && mesh.taskShader) {
                caps->features |= PAL_GPU_FEATURE_MESH_SHADER;
            }

        } else if (strcmp(props->extensionName, "VK_KHR_fragment_shading_rate") == 0) {
            // variable rate shading
            VkPhysicalDeviceFragmentShadingRateFeaturesKHR frag = {0};
            frag.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &frag;

            if (s_Vk.getPhysicalDeviceFeatures2KHR) {
                s_Vk.getPhysicalDeviceFeatures2KHR(physicalDevice, &features);

            } else {
                s_Vk.getPhysicalDeviceFeatures2(physicalDevice, &features);
            }

            if (frag.pipelineFragmentShadingRate) {
                caps->features |= PAL_GPU_FEATURE_VARIABLE_RATE_SHADING;
            }

        } else if (strcmp(props->extensionName, "VK_EXT_descriptor_indexing") == 0) {
            // descriptor indexing
            VkPhysicalDeviceDescriptorIndexingFeatures desc = {0};
            desc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &desc;

            if (s_Vk.getPhysicalDeviceFeatures2KHR) {
                s_Vk.getPhysicalDeviceFeatures2KHR(physicalDevice, &features);

            } else {
                s_Vk.getPhysicalDeviceFeatures2(physicalDevice, &features);
            }

            if (desc.shaderSampledImageArrayNonUniformIndexing) {
                caps->features |= PAL_GPU_FEATURE_DESCRIPTOR_INDEXING;
            }

        } else if (strcmp(props->extensionName, "VK_KHR_swapchain") == 0) {
            // swapchain
            caps->features |= PAL_GPU_FEATURE_SWAPCHAIN;

        } else if (strcmp(props->extensionName, "VK_KHR_dynamic_rendering") == 0) {
            // dynamic rendering
            caps->features |= PAL_GPU_FEATURE_DYNAMIC_RENDERING;

        } else if (strcmp(props->extensionName, "VK_KHR_shader_float16_int8") == 0) {
            // shader float16
            VkPhysicalDeviceShaderFloat16Int8FeaturesKHR shader16 = {0};
            shader16.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &shader16;

            if (s_Vk.getPhysicalDeviceFeatures2KHR) {
                s_Vk.getPhysicalDeviceFeatures2KHR(physicalDevice, &features);

            } else {
                s_Vk.getPhysicalDeviceFeatures2(physicalDevice, &features);
            }

            if (shader16.shaderFloat16) {
                caps->features |= PAL_GPU_FEATURE_SHADER_FLOAT16;
            }

        } else if (strcmp(props->extensionName, "VK_KHR_timeline_semaphore") == 0) {
            // timeline semaphore
            VkPhysicalDeviceTimelineSemaphoreFeatures timeline = {0};
            timeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &timeline;

            if (s_Vk.getPhysicalDeviceFeatures2KHR) {
                s_Vk.getPhysicalDeviceFeatures2KHR(physicalDevice, &features);

            } else {
                s_Vk.getPhysicalDeviceFeatures2(physicalDevice, &features);
            }

            if (timeline.timelineSemaphore) {
                caps->features |= PAL_GPU_FEATURE_TIMELINE_SEMAPHORE;
            }
        }
    }

    if (accelerateFound && rayTracingFound) {
        // ray tracing
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray = {0};
        VkPhysicalDeviceAccelerationStructureFeaturesKHR acc = {0};
        ray.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        acc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

        ray.pNext = &acc;
        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &ray;

        if (s_Vk.getPhysicalDeviceFeatures2KHR) {
            s_Vk.getPhysicalDeviceFeatures2KHR(physicalDevice, &features);

        } else {
            s_Vk.getPhysicalDeviceFeatures2(physicalDevice, &features);
        }

        if (ray.rayTracingPipeline && acc.accelerationStructure) {
            caps->features |= PAL_GPU_FEATURE_RAY_TRACING;
        }
    }
    // clang-format on
    VkPhysicalDeviceFeatures features;
    s_Vk.getPhysicalDeviceFeatures(physicalDevice, &features);

    // check for additional features
    if (features.geometryShader) {
        caps->features |= PAL_GPU_FEATURE_GEOMETRY_SHADER;
    }

    if (features.multiViewport) {
        caps->features |= PAL_GPU_FEATURE_MULTI_VIEWPORT;
    }

    if (features.samplerAnisotropy) {
        caps->features |= PAL_GPU_FEATURE_SAMPLER_ANISOTROPY;
    }

    if (features.sampleRateShading) {
        caps->features |= PAL_GPU_FEATURE_SAMPLE_RATE_SHADING;
    }

    if (features.shaderFloat64) {
        caps->features |= PAL_GPU_FEATURE_SHADER_FLOAT64;
    }

    if (features.shaderInt64) {
        caps->features |= PAL_GPU_FEATURE_SHADER_INT64;
    }

    if (features.shaderInt16) {
        caps->features |= PAL_GPU_FEATURE_SHADER_INT16;
    }

    if (features.tessellationShader) {
        caps->features |= PAL_GPU_FEATURE_TESSELLATION_SHADER;
    }

    palFree(s_Graphics.allocator, extensionProps);
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL vkCreateGPUDevice(
    PalGPUAdapter* adapter,
    PalGPUFeatures features,
    PalGPUDevice** outDevice)
{
    VkResult ret = VK_SUCCESS;
    VkDevice device = nullptr;
    VkPhysicalDevice physicalDevice = (VkPhysicalDevice)adapter;

    Uint32 count = 0;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(
        physicalDevice, 
        &count, 
        nullptr);

    VkQueueFamilyProperties queueProps[MAX_QUEUE_FAMILIES];
    s_Vk.getPhysicalDeviceQueueFamilyProperties(
        physicalDevice, 
        &count, 
        queueProps);

    Int32 queueFamilyCount = 0;
    QueueFamilyData queueFamilyData[MAX_QUEUE_FAMILIES];
    for (int i = 0; i < count; i++) {
        VkQueueFamilyProperties* prop = &queueProps[i];
        QueueFamilyData* data = &queueFamilyData[queueFamilyCount++];
        data->count = prop->queueCount;
        data->flags = prop->queueFlags;
        data->index = i;
    }

    float priority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfos[MAX_QUEUE_FAMILIES];
    for (int i = 0; i < queueFamilyCount; i++) {
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].pNext = nullptr;
        queueCreateInfos[i].pQueuePriorities = &priority;
        queueCreateInfos[i].queueFamilyIndex = queueFamilyData[i].index;
        queueCreateInfos[i].queueCount = queueFamilyData[i].count;
        queueCreateInfos[i].flags = 0;
    }

    // build features and extensions capabilities
    VkPhysicalDeviceFeatures coreFeatures = {0};
    if (features & PAL_GPU_FEATURE_SAMPLER_ANISOTROPY) {
        coreFeatures.samplerAnisotropy = true;
    }

    if (features & PAL_GPU_FEATURE_SAMPLE_RATE_SHADING) {
        coreFeatures.sampleRateShading = true;
    }

    if (features & PAL_GPU_FEATURE_MULTI_VIEWPORT) {
        coreFeatures.multiViewport = true;
    }

    if (features & PAL_GPU_FEATURE_TESSELLATION_SHADER) {
        coreFeatures.tessellationShader = true;
    }

    if (features & PAL_GPU_FEATURE_GEOMETRY_SHADER) {
        coreFeatures.geometryShader = true;
    }

    if (features & PAL_GPU_FEATURE_SHADER_INT16) {
        coreFeatures.shaderInt16 = true;
    }

    if (features & PAL_GPU_FEATURE_SHADER_INT64) {
        coreFeatures.shaderInt64 = true;
    }

    if (features & PAL_GPU_FEATURE_SHADER_FLOAT64) {
        coreFeatures.shaderFloat64 = true;
    }

    // extensions and features2
    int extCount = 0;
    const char* extensions[16] = {0};

    // clang-format off

    const void* start = nullptr;
    VkPhysicalDeviceTimelineSemaphoreFeatures timeline = {0};
    timeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;

    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR shader16 = {0};
    shader16.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

    VkPhysicalDeviceMeshShaderFeaturesEXT mesh = {0};
    mesh.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray = {0};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR acc = {0};
    ray.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    acc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

    VkPhysicalDeviceFragmentShadingRateFeaturesKHR vrs = {0};
    vrs.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;

    VkPhysicalDeviceDescriptorIndexingFeatures descIndex = {0};
    descIndex.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;

    // clang-format on

    if (features & PAL_GPU_FEATURE_SWAPCHAIN) {
        extensions[extCount++] = "VK_KHR_swapchain";
    }

    if (features & PAL_GPU_FEATURE_DYNAMIC_RENDERING) {
        extensions[extCount++] = "VK_KHR_dynamic_rendering";
    }

    if (features & PAL_GPU_FEATURE_TIMELINE_SEMAPHORE) {
        extensions[extCount++] = "VK_KHR_timeline_semaphore";
        timeline.timelineSemaphore = true;
        start = &timeline;
    }

    if (features & PAL_GPU_FEATURE_SHADER_FLOAT16) {
        extensions[extCount++] = "VK_KHR_shader_float16_int8";
        shader16.shaderFloat16 = true;

        if (timeline.timelineSemaphore) {
            timeline.pNext = &shader16;
        }
        start = &shader16;
    }

    if (features & PAL_GPU_FEATURE_RAY_TRACING) {
        extensions[extCount++] = "VK_KHR_ray_tracing_pipeline";
        extensions[extCount++] = "VK_KHR_acceleration_structure";
        ray.rayTracingPipeline = true;
        acc.accelerationStructure = true;
       
        if (shader16.shaderFloat16) {
            shader16.pNext = &ray;

        } else if (timeline.timelineSemaphore) {
            // no shader16, check timeline
            timeline.pNext = &ray;
        }
        ray.pNext = &acc;
        start = &ray;
    }

    if (features & PAL_GPU_FEATURE_MESH_SHADER) {
        extensions[extCount++] = "VK_EXT_mesh_shader";
        mesh.meshShader = true;
        mesh.taskShader = true;

        if (acc.accelerationStructure) {
            acc.pNext = &mesh;

        } else if (shader16.shaderFloat16) {
            shader16.pNext = &mesh;

        } else if (timeline.timelineSemaphore) {
            timeline.pNext = &mesh;
        }
        start = &mesh;
    }

    if (features & PAL_GPU_FEATURE_VARIABLE_RATE_SHADING) {
        extensions[extCount++] = "VK_KHR_fragment_shading_rate";
        vrs.pipelineFragmentShadingRate = true;

        if (mesh.meshShader) {
            mesh.pNext = &vrs;

        } else if (acc.accelerationStructure) {
            acc.pNext = &vrs;

        } else if (shader16.shaderFloat16) {
            shader16.pNext = &vrs;

        } else if (timeline.timelineSemaphore) {
            timeline.pNext = &vrs;
        }
        start = &vrs;
    }

    if (features & PAL_GPU_FEATURE_DESCRIPTOR_INDEXING) {
        extensions[extCount++] = "VK_EXT_descriptor_indexing";
        descIndex.shaderSampledImageArrayNonUniformIndexing = true;

        if (vrs.pipelineFragmentShadingRate) {
            vrs.pNext = &descIndex;

        } else if (mesh.meshShader) {
            mesh.pNext = &descIndex;

        } else if (acc.accelerationStructure) {
            acc.pNext = &descIndex;

        } else if (shader16.shaderFloat16) {
            shader16.pNext = &descIndex;

        } else if (timeline.timelineSemaphore) {
            timeline.pNext = &descIndex;
        }
        start = &descIndex;
    }

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pEnabledFeatures = &coreFeatures;
    createInfo.enabledExtensionCount = extCount;
    createInfo.ppEnabledExtensionNames = extensions;
    createInfo.pQueueCreateInfos = queueCreateInfos;
    createInfo.queueCreateInfoCount = queueFamilyCount;
    createInfo.pNext = start;

    ret = s_Vk.createDevice(
        physicalDevice, 
        &createInfo, 
        &s_Vk.allocator, 
        &device);

    if (ret != VK_SUCCESS) {
        return vkResultToPal(ret);
    }

    VkGPUDevice* gpuDevice = nullptr;
    gpuDevice = palAllocate(s_Graphics.allocator, sizeof(VkGPUDevice), 0);
    if (!gpuDevice) {
        s_Vk.destroyDevice(device, &s_Vk.allocator);
        return PAL_RESULT_OUT_OF_MEMORY;
    }
    memset(gpuDevice, 0, sizeof(VkGPUDevice));

    // get queues
    gpuDevice->handle = device;
    for (int i = 0; i < queueFamilyCount; i++) {
        QueueFamilyData* data = &queueFamilyData[i];
        for (int j = 0; j < data->count; j++) {
            PhysicalQueue* queue = &gpuDevice->queues[gpuDevice->queueCount++];
            s_Vk.getDeviceQueue(device, data->index, 0, &queue->handle);
            queue->usages = data->flags;
            queue->usedUsages = 0;
        }        
    }

    *outDevice = (PalGPUDevice*)gpuDevice;
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL vkDestroyGPUDevice(PalGPUDevice* device)
{
    VkGPUDevice* gpuDevice = (VkGPUDevice*)device;
    s_Vk.destroyDevice(gpuDevice->handle, &s_Vk.allocator);
    palFree(s_Graphics.allocator, gpuDevice);
}

static PalResult PAL_CALL vkCreateGPUCommandQueue(
    PalGPUDevice* device,
    PalGPUCommandQueueType type,
    PalGPUCommandQueue** outQueue)
{
    VkCommandQueue* commandQueue = nullptr;
    VkQueueFlags queueFlag = 0;
    
    VkGPUDevice* gpuDevice = (VkGPUDevice*)device;
    if (!gpuDevice->handle) {
        return PAL_RESULT_INVALID_GPU_DEVICE;
    }

    if (gpuDevice->queueCount == 0) {
        return PAL_RESULT_OUT_OF_GPU_COMMAND_QUEUE;
    }

    switch (type) {
        case PAL_GPU_COMMAND_QUEUE_TYPE_COMPUTE: {
            queueFlag = VK_QUEUE_COMPUTE_BIT;
            break;
        }

        case PAL_GPU_COMMAND_QUEUE_TYPE_GRAPHICS: {
            queueFlag = VK_QUEUE_GRAPHICS_BIT;
            break;
        }

        case PAL_GPU_COMMAND_QUEUE_TYPE_COPY: {
            queueFlag = VK_QUEUE_TRANSFER_BIT;
            break;
        }
    }

    PhysicalQueue* physicalQueue = nullptr;
    for (int i = 0; i < gpuDevice->queueCount; i++) {
        PhysicalQueue* phyQueue = &gpuDevice->queues[i];
        // check if the physical queue supports the requested operation
        // and if its not already used
        if (phyQueue->usages & queueFlag && 
            phyQueue->usedUsages != queueFlag) {
            phyQueue->usedUsages |= queueFlag;
            physicalQueue = phyQueue;
            break;
        } 
    }

    if (physicalQueue) {
        commandQueue = palAllocate(
            s_Graphics.allocator, 
            sizeof(VkCommandQueue), 
            0);
        
        if (!commandQueue) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        commandQueue->phyQueue = physicalQueue;
        commandQueue->usage = queueFlag;

        *outQueue = (PalGPUCommandQueue*)commandQueue;
        return PAL_RESULT_SUCCESS;
    }

    return PAL_RESULT_OUT_OF_GPU_COMMAND_QUEUE;
}

static void PAL_CALL vkDestroyGPUCommandQueue(PalGPUCommandQueue* queue)
{
    VkCommandQueue* commandQueue = (VkCommandQueue*)queue;
    PhysicalQueue* phyQueue = commandQueue->phyQueue;
    phyQueue->usedUsages &= ~commandQueue->usage;
    palFree(s_Graphics.allocator, commandQueue);
}

static PalGPUBackend s_VkBackend = {
    .enumerateGPUAdapters = vkEnumerateAdapters,
    .getGPUAdapterInfo = vkGetAdapterInfo,
    .getGPUAdapterCapabilities = vkGetAdapterCapabilities,
    .createGPUDevice = vkCreateGPUDevice,
    .destroyGPUDevice = vkDestroyGPUDevice,
    .createGPUCommandQueue = vkCreateGPUCommandQueue,
    .destroyGPUCommandQueue = vkDestroyGPUCommandQueue
};

#endif // PAL_HAS_VULKAN

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palInitGraphics(
    bool enableDebugLayer,
    const PalAllocator* allocator)
{
    if (s_Graphics.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    s_Graphics.allocator = allocator;
#if PAL_HAS_VULKAN
    PalResult ret = vkInitGraphics(enableDebugLayer);
    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    AttachBackend* backend = &s_Graphics.backends[s_Graphics.backendCount++];
    backend->base = &s_VkBackend;
    backend->count = 0;
    backend->startIndex = 0;
#endif // PAL_HAS_VULKAN

    s_Graphics.initialized = true;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGraphics()
{
    if (!s_Graphics.initialized) {
        return;
    }

#if PAL_HAS_VULKAN
    vkShutdownGraphics();
#endif // PAL_HAS_VULKAN

    memset(&s_Graphics, 0, sizeof(s_Graphics));
    s_Graphics.initialized = false;
}

// ==================================================
// GPUAdapter
// ==================================================

PalResult PAL_CALL palEnumerateGPUAdapters(
   Int32* count,
   PalGPUAdapter** outAdapters)
{
    // enumerate all adapters for both custom and PAL backends
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!count) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (*count == 0 && outAdapters) {
        return PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    PalResult result;
    int totalCount = 0;
    int index = 0;

    int _count = outAdapters ? *count : 0;
    for (int i = 0; i < s_Graphics.backendCount; i++) {
        AttachBackend* backend = &s_Graphics.backends[i];
        if (outAdapters) {
            // offset into the array so all backends write at the correct index
            PalGPUAdapter** adapters = &outAdapters[backend->startIndex];
            result = backend->base->enumerateGPUAdapters(&_count, adapters);

            for (int i = 0; i < backend->count; i++) {
                AdapterData* data = getFreeAdapterData();
                data->adapter = adapters[i];
                data->backend = backend->base;
            }

        } else {
            result = backend->base->enumerateGPUAdapters(&_count, nullptr);
            backend->startIndex = totalCount;
            backend->count = _count;
            totalCount += _count;
            _count = 0;
        }
        
        // break if a backend fails
        if (result != PAL_RESULT_SUCCESS) {
            return result;
        }
    }

    if (!outAdapters) {
        *count = totalCount;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetGPUAdapterInfo(
    PalGPUAdapter* adapter,
    PalGPUAdapterInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!adapter || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    AdapterData* data = findAdapterData(adapter);
    if (data) {
        return data->backend->getGPUAdapterInfo(adapter, info);
    }

    return PAL_RESULT_INVALID_GPU_ADAPTER;
}

PalResult PAL_CALL palGetGPUAdapterCapabilities(
    PalGPUAdapter* adapter,
    PalGPUAdapterCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!adapter || !caps) {
        return PAL_RESULT_NULL_POINTER;
    }

    AdapterData* data = findAdapterData(adapter);
    if (data) {
        return data->backend->getGPUAdapterCapabilities(adapter, caps);
    }

    return PAL_RESULT_INVALID_GPU_ADAPTER;
}

PalResult PAL_CALL palAddGPUBackend(const PalGPUBackend* backend)
{
    if (s_Graphics.initialized) {
        return PAL_RESULT_INVALID_GPU_BACKEND;
    }

    // check if all the function pointers are set
    // clang-format off
    if (!backend->enumerateGPUAdapters      || 
        !backend->getGPUAdapterInfo         ||
        !backend->getGPUAdapterCapabilities ||
        !backend->createGPUDevice           ||
        !backend->destroyGPUDevice          ||
        !backend->createGPUCommandQueue     ||
        !backend->destroyGPUCommandQueue) {
        return PAL_RESULT_INVALID_GPU_BACKEND;
    }
    // clang-format on

    AttachBackend* attached = &s_Graphics.backends[s_Graphics.backendCount++];
    attached->base = backend;
    attached->startIndex = 0;
    attached->count = 0;

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// GPUDevice
// ==================================================

PalResult PAL_CALL palCreateGPUDevice(
    PalGPUAdapter* adapter,
    PalGPUFeatures features,
    PalGPUDevice** outDevice)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!outDevice) {
        return PAL_RESULT_NULL_POINTER;
    }

    // check if the adapter is from PAL (custom or internal backend)
    AdapterData* adapterData = findAdapterData(adapter);
    if (!adapterData) {
        return PAL_RESULT_INVALID_GPU_ADAPTER;
    }

    PalGPUDevice* device = nullptr;
    PalResult ret;
    ret = adapterData->backend->createGPUDevice(adapter, features, &device);
    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    // create a slot for the created device
    DeviceData* deviceData = getFreeDeviceData();
    if (!deviceData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    deviceData->backend = adapterData->backend;
    deviceData->device = device;

    *outDevice = device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyGPUDevice(PalGPUDevice* device)
{
    if (s_Graphics.initialized && device) {
        DeviceData* data = findDeviceData(device);
        if (data) {
            data->backend->destroyGPUDevice(device);
            data->used = false;
        }
    }
}

PalResult PAL_CALL palCreateGPUCommandQueue(
    PalGPUDevice* device,
    PalGPUCommandQueueType type,
    PalGPUCommandQueue** outQueue)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !outQueue) {
        return PAL_RESULT_NULL_POINTER;
    }

    DeviceData* data = findDeviceData(device);
    if (!data) {
        return PAL_RESULT_INVALID_GPU_DEVICE;
    }

    PalGPUCommandQueue* queue = nullptr;
    PalResult ret;
    ret = data->backend->createGPUCommandQueue(
        device,
        type,
        &queue);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    // create a slot for the created device
    CommandQueueData* queueData = getFreeCommandQueueData();
    if (!queueData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    queueData->backend = data->backend;
    queueData->queue = queue;
    queueData->device = device;

    *outQueue = queue;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyGPUCommandQueue(PalGPUCommandQueue* queue)
{
    if (s_Graphics.initialized && queue) {
        CommandQueueData* data = findCommandQueueData(queue);
        if (data) {
            data->backend->destroyGPUCommandQueue(queue);
            data->used = false;
        }
    }
}