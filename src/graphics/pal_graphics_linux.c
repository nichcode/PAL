
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

#define MAX_BACKENDS 16 // should be fine for now
#define MAX_ADAPTERS 32 // should be enough

#if PAL_HAS_VULKAN
// VKAPI_PTR expands to nothing on linux

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

typedef void (*vkGetPhysicalDeviceFeatures2Fn)(
    VkPhysicalDevice, 
    VkPhysicalDeviceFeatures2*);

#endif // PAL_HAS_VULKAN

typedef struct {
    bool used;
    const PalGPUBackend* backend;
    PalGPUAdapter* adapter;
} AdapterData;

typedef struct {
    const PalGPUBackend* base;
    Uint16 startIndex;
    Uint16 count;
} AttachGPUBackend;

typedef struct {
    bool initialized;
    bool hasDebug;
    Int32 backendCount;
    Int32 totalAdapterCount;
    const PalAllocator* allocator;
    void* instance;
    void* handle;

    void* destroyInstance;
    void* createInstance;
    void* enumeratePhysicalDevices;
    void* getPhysicalDeviceProperties;
    void* getPhysicalDeviceMemoryProperties;
    void* enumerateInstanceLayerProperties;
    void* getPhysicalDeviceQueueFamilyProperties;
    void* enumerateDeviceExtensionProperties;
    void* getPhysicalDeviceFeatures2;

    AdapterData adapterData[MAX_ADAPTERS];
    AttachGPUBackend backends[MAX_BACKENDS];
} VkGPU;

static VkGPU s_VkGPU = {0};

// ==================================================
// Internal API
// ==================================================

static AdapterData* getFreeAdapterData()
{
    for (int i = 0; i < MAX_ADAPTERS; ++i) {
        if (!s_VkGPU.adapterData[i].used) {
            s_VkGPU.adapterData[i].used = true;
            return &s_VkGPU.adapterData[i];
        }
    }  
}

static AdapterData* findAdapterData(PalGPUAdapter* adapter)
{
    for (int i = 0; i < MAX_ADAPTERS; ++i) {
        if (s_VkGPU.adapterData[i].used &&
            s_VkGPU.adapterData[i].adapter == adapter) {
            return &s_VkGPU.adapterData[i];
        }
    }
    return nullptr;
}

#if PAL_HAS_VULKAN
// we dont want to fill this everytime we want to use
static VkAllocationCallbacks s_VkAllocator = {0};

void* vkAlloc(
    void* pUserData,
    size_t size,
    size_t alignment,
    VkSystemAllocationScope allocationScope)
{
    return palAllocate(s_VkGPU.allocator, size, alignment);
}

void vkFree(
    void* pUserData,
    void* ptr)
{
    palFree(s_VkGPU.allocator, ptr);
}

void* vkRealloc(
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
        void* memory = palAllocate(s_VkGPU.allocator, size, alignment);
        if (!memory) {
            free(block);
            return nullptr;
        }

        memcpy(memory, block, size);
        return memory;
    }
    return nullptr;
}

PalResult vkInitGraphics()
{
    // load vulkan
    s_VkGPU.handle = dlopen("libvulkan.so", RTLD_LAZY);
    if (!s_VkGPU.handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_VkGPU.createInstance = dlsym(s_VkGPU.handle, "vkCreateInstance");
    s_VkGPU.destroyInstance = dlsym(s_VkGPU.handle, "vkDestroyInstance");

    s_VkGPU.enumeratePhysicalDevices = dlsym(
        s_VkGPU.handle, 
        "vkEnumeratePhysicalDevices");

    s_VkGPU.getPhysicalDeviceProperties = dlsym(
        s_VkGPU.handle, 
        "vkGetPhysicalDeviceProperties");

    s_VkGPU.getPhysicalDeviceMemoryProperties = dlsym(
        s_VkGPU.handle, 
        "vkGetPhysicalDeviceMemoryProperties");

    s_VkGPU.enumerateInstanceLayerProperties = dlsym(
        s_VkGPU.handle, 
        "vkEnumerateInstanceLayerProperties");

    s_VkGPU.getPhysicalDeviceQueueFamilyProperties = dlsym(
        s_VkGPU.handle, 
        "vkGetPhysicalDeviceQueueFamilyProperties");

    s_VkGPU.enumerateDeviceExtensionProperties = dlsym(
        s_VkGPU.handle, 
        "vkEnumerateDeviceExtensionProperties");

    s_VkGPU.getPhysicalDeviceFeatures2 = dlsym(
        s_VkGPU.handle, 
        "vkGetPhysicalDeviceFeatures2");

    // create a dummy instance
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_0; // for wider support
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Engine";
    appInfo.pApplicationName = "App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instanceCreateInfo = {0};
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    // vk allocator
    s_VkAllocator.pfnAllocation = vkAlloc;
    s_VkAllocator.pfnFree = vkFree;
    s_VkAllocator.pfnReallocation = vkRealloc;

    VkInstance instance = nullptr;
    vkCreateInstanceFn vkCreateInstancePtr = s_VkGPU.createInstance;
    VkResult result = vkCreateInstancePtr(
        &instanceCreateInfo, 
        &s_VkAllocator, 
        &instance);

    if (result != VK_SUCCESS) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    Uint32 count = 0;
    vkEnumerateInstanceLayerPropertiesFn enumerateProperties;
    enumerateProperties = s_VkGPU.enumerateInstanceLayerProperties;

    VkResult ret = enumerateProperties(&count, nullptr);
    if (ret != VK_SUCCESS) {
        s_VkGPU.hasDebug = false;
    }

    VkLayerProperties* props = nullptr;
    props = palAllocate(
        s_VkGPU.allocator, 
        sizeof(VkLayerProperties) * count,
        0);

    if (!props) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }
    
    enumerateProperties(&count, props);
    for (int i = 0; i < count; i++) {
        if (strcmp(props[i].layerName, "VK_LAYER_KHRONOS_validation") == 0) {
            s_VkGPU.hasDebug = true;
            break;
        }
    }

    palFree(s_VkGPU.allocator, props);
    s_VkGPU.instance = instance;
    return PAL_RESULT_SUCCESS;
}

void vkShutdownGraphics()
{
    if (s_VkGPU.instance) {
        vkDestroyInstanceFn vkDestroyInstancePtr = s_VkGPU.destroyInstance;
        vkDestroyInstancePtr(s_VkGPU.instance, &s_VkAllocator);
        dlclose(s_VkGPU.handle);
    }
}

PalResult vkEnumerateAdapters(
    Int32* count, 
    PalGPUAdapter** outAdapters)
{
    int _count = 0;
    int maxCount = outAdapters ? *count : 0;

    vkEnumeratePhysicalDevicesFn enumerate = s_VkGPU.enumeratePhysicalDevices;
    VkResult result = enumerate(s_VkGPU.instance, &_count, nullptr);
    if (result != VK_SUCCESS) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    if (outAdapters) {
        VkPhysicalDevice* devices = nullptr;
        devices = palAllocate(
            s_VkGPU.allocator, 
            sizeof(VkPhysicalDevice) * _count,
            0);

        if (!devices) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        result = enumerate(s_VkGPU.instance, &_count, devices);
        if (result != VK_SUCCESS) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        // write to user array
        for (int i = 0; i < _count && i < *count; i++) {
            outAdapters[i] = (PalGPUAdapter*)devices[i];
        }

        palFree(s_VkGPU.allocator, devices);
    }

    if (!outAdapters) {
        *count = _count;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL vkGetAdapterInfo(
    PalGPUAdapter* adapter,
    PalGPUAdapterInfo* info)
{
    VkPhysicalDevice vkPhysicalDevice = (VkPhysicalDevice)adapter;
    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDevicePropertiesFn getProperties;
    vkGetPhysicalDeviceMemoryPropertiesFn getMemoryProperties;
    vkGetPhysicalDeviceQueueFamilyPropertiesFn getQueueProperties;
    vkEnumerateDeviceExtensionPropertiesFn getExtensionProperties;
    vkGetPhysicalDeviceFeatures2Fn getFeatures2;

    getProperties = s_VkGPU.getPhysicalDeviceProperties;
    getMemoryProperties = s_VkGPU.getPhysicalDeviceMemoryProperties;
    getQueueProperties = s_VkGPU.getPhysicalDeviceQueueFamilyProperties;
    getExtensionProperties = s_VkGPU.enumerateDeviceExtensionProperties;
    getFeatures2 = s_VkGPU.getPhysicalDeviceFeatures2;

    getProperties(vkPhysicalDevice, &props);
    getMemoryProperties(vkPhysicalDevice, &memProps);

    strcpy(info->name, props.deviceName);
    info->version = props.apiVersion;
    info->debugLayerSupported = s_VkGPU.hasDebug;

    // get total memory
    Uint64 memory = 0;
    for (int i = 0; i < memProps.memoryHeapCount; i++) {
        if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            memory += memProps.memoryHeaps[i].size;
        }
    }

    info->totalMemory = memory;
    info->apiType = PAL_GPU_API_VULKAN;

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
        VK_VERSION_MAJOR(info->version),
        VK_VERSION_MINOR(info->version),
        VK_VERSION_PATCH(info->version));

    // get supported queue commands
    Uint32 count;
    getQueueProperties(
        vkPhysicalDevice, 
        &count, 
        nullptr);

    // not that huge, we allocate on the stack rather (16 for safety)
    VkQueueFamilyProperties queueProps[16];
    getQueueProperties(
        vkPhysicalDevice, 
        &count, 
        queueProps);

    info->commands = 0;
    for (int i = 0; i < count; i++) {
        if (queueProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            info->commands |= PAL_GPU_COMMAND_COMPUTE;
        }

        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            info->commands |= PAL_GPU_COMMAND_GRAPHICS;
        }

        if (queueProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            info->commands |= PAL_GPU_COMMAND_TRANSFER;
        }
    }

    // get supported extensions
    Uint32 extensionCount = 0;
    VkResult ret = getExtensionProperties(
        vkPhysicalDevice, 
        nullptr, 
        &extensionCount, 
        nullptr);

    if (ret != VK_SUCCESS) {
        // we just return without any modern features
        return PAL_RESULT_SUCCESS;
    }

    VkExtensionProperties* extensionProps = nullptr;
    extensionProps = palAllocate(
        s_VkGPU.allocator, 
        sizeof(VkExtensionProperties) * extensionCount, 
        0);

    if (!extensionProps) {
        return PAL_RESULT_SUCCESS;
    }

    getExtensionProperties(
        vkPhysicalDevice, 
        nullptr, 
        &extensionCount, 
        extensionProps);

    bool rayTracingFound = false;
    bool accelerateFound = false;
    info->features = 0;

    // clang-format off
    for (int i = 0; i < extensionCount; i++) {
        VkExtensionProperties* props = &extensionProps[i];
        if (strcmp(props->extensionName, "VK_KHR_ray_tracing_pipeline") == 0) {
            rayTracingFound = true;

        } else if (strcmp(props->extensionName, "VK_KHR_acceleration_structur") == 0) {
            accelerateFound = true;

        } else if (strcmp(props->extensionName, "VK_EXT_mesh_shader") == 0) {
            // mesh shader
            VkPhysicalDeviceMeshShaderFeaturesEXT mesh = {0};
            mesh.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &mesh;
            getFeatures2(vkPhysicalDevice, &features);

            if (mesh.meshShader && mesh.taskShader) {
                info->features |= PAL_GPU_FEATURE_MESH_SHADER;
            }

        } else if (strcmp(props->extensionName, "VK_KHR_fragment_shading_rate") == 0) {
            // variable rate shading
            VkPhysicalDeviceFragmentShadingRateFeaturesKHR frag = {0};
            frag.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &frag;
            getFeatures2(vkPhysicalDevice, &features);

            if (frag.pipelineFragmentShadingRate) {
                info->features |= PAL_GPU_FEATURE_VARIABLE_RATE_SHADING;
            }

        } else if (strcmp(props->extensionName, "VK_EXT_descriptor_indexing") == 0) {
            // descriptor indexing
            VkPhysicalDeviceDescriptorIndexingFeatures desc = {0};
            desc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &desc;
            getFeatures2(vkPhysicalDevice, &features);

            if (desc.shaderSampledImageArrayNonUniformIndexing) {
                info->features |= PAL_GPU_FEATURE_DESCRIPTOR_INDEXING;
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
        getFeatures2(vkPhysicalDevice, &features);

        if (ray.rayTracingPipeline && acc.accelerationStructure) {
            info->features |= PAL_GPU_FEATURE_RAY_TRACING;
        }
    }
    // clang-format on

    palFree(s_VkGPU.allocator, extensionProps);
    return PAL_RESULT_SUCCESS;
}

static PalGPUBackend s_VkBackend = {
    .enumerateGPUAdapters = vkEnumerateAdapters,
    .getGPUAdapterInfo = vkGetAdapterInfo
};

#endif // PAL_HAS_VULKAN

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palInitGraphics(const PalAllocator* allocator)
{
    if (s_VkGPU.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    s_VkGPU.allocator = allocator;
#if PAL_HAS_VULKAN
    PalResult ret = vkInitGraphics();
    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    AttachGPUBackend* backend = &s_VkGPU.backends[s_VkGPU.backendCount++];
    backend->base = &s_VkBackend;
    backend->count = 0;
    backend->startIndex = 0;
#endif // PAL_HAS_VULKAN

    s_VkGPU.initialized = true;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGraphics()
{
    if (!s_VkGPU.initialized) {
        return;
    }

#if PAL_HAS_VULKAN
    vkShutdownGraphics();
#endif // PAL_HAS_VULKAN

    memset(&s_VkGPU, 0, sizeof(VkGPU));
    s_VkGPU.initialized = false; // just in case
}

// ==================================================
// GPUAdapter
// ==================================================

PalResult PAL_CALL palEnumerateGPUAdapters(
   Int32* count,
   PalGPUAdapter** outAdapters)
{
    // enumerate all adapters for both custom and PAL backends
    if (!s_VkGPU.initialized) {
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
    for (int i = 0; i < s_VkGPU.backendCount; i++) {
        AttachGPUBackend* backend = &s_VkGPU.backends[i];
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
    if (!s_VkGPU.initialized) {
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

PalResult PAL_CALL palAddGPUBackend(const PalGPUBackend* backend)
{
    if (s_VkGPU.initialized) {
        return PAL_RESULT_INVALID_GPU_BACKEND;
    }

    // check if all the function pointers are set
    // clang-format off
    if (!backend->enumerateGPUAdapters || 
        !backend->getGPUAdapterInfo) {
        return PAL_RESULT_INVALID_GPU_BACKEND;
    }
    // clang-format on

    AttachGPUBackend* attached = &s_VkGPU.backends[s_VkGPU.backendCount++];
    attached->base = backend;
    attached->startIndex = 0;
    attached->count = 0;

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// GPUDevice
// ==================================================

PalResult PAL_CALL palCreateGPUDevice(
    bool debug,
    PalGPUAdapter* adapter,
    PalGPUDevice** outDevice)
{

}

void PAL_CALL palDestroyGPUDevice(PalGPUDevice* device)
{

}