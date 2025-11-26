
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
#define MAX_DEVICE 16 // should be enough

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

#endif // PAL_HAS_VULKAN

typedef struct {
    bool used;
    const PalGPUBackend* backend;
    PalGPUAdapter* adapter;
} AdapterData;

typedef struct {
    bool used;
    PalGPUDevice* device;
    AdapterData* adapterData;
} DeviceData;

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

    void* enumerateInstanceVersion;
    void* enumerateInstanceExtensionProperties;
    void* destroyInstance;
    void* createInstance;
    void* enumeratePhysicalDevices;
    void* getPhysicalDeviceProperties;
    void* getPhysicalDeviceMemoryProperties;
    void* enumerateInstanceLayerProperties;
    void* getPhysicalDeviceQueueFamilyProperties;
    void* enumerateDeviceExtensionProperties;
    void* getPhysicalDeviceFeatures;
    void* getPhysicalDeviceFeatures2;

    AdapterData adapterData[MAX_ADAPTERS];
    DeviceData deviceData[MAX_DEVICE];
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

static DeviceData* getFreeDeviceData()
{
    for (int i = 0; i < MAX_DEVICE; ++i) {
        if (!s_VkGPU.deviceData[i].used) {
            s_VkGPU.deviceData[i].used = true;
            return &s_VkGPU.deviceData[i];
        }
    }  
}

static DeviceData* findDeviceData(PalGPUDevice* device)
{
    for (int i = 0; i < MAX_DEVICE; ++i) {
        if (s_VkGPU.deviceData[i].used &&
            s_VkGPU.deviceData[i].device == device) {
            return &s_VkGPU.deviceData[i];
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

PalResult vkInitGraphics(
    bool enableDebug,
    Int32 versionHint)
{
    // load vulkan
    s_VkGPU.handle = dlopen("libvulkan.so", RTLD_LAZY);
    if (!s_VkGPU.handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_VkGPU.enumerateInstanceVersion = dlsym(
        s_VkGPU.handle, 
        "vkEnumerateInstanceVersion");

    s_VkGPU.enumerateInstanceExtensionProperties = dlsym(
        s_VkGPU.handle, 
        "vkEnumerateInstanceExtensionProperties");

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

    s_VkGPU.getPhysicalDeviceFeatures = dlsym(
        s_VkGPU.handle, 
        "vkGetPhysicalDeviceFeatures");

    s_VkGPU.getPhysicalDeviceFeatures2 = dlsym(
        s_VkGPU.handle, 
        "vkGetPhysicalDeviceFeatures2");

    // get version
    Uint32 version = 0;
    if (versionHint == PAL_VERSION_DEFAULT) {
        vkEnumerateInstanceVersionFn getInstanceVersion;
        getInstanceVersion = s_VkGPU.enumerateInstanceVersion;
        if (getInstanceVersion) {
            getInstanceVersion(&version);
        } else {
            version = VK_API_VERSION_1_0;
        }

    } else {
        Uint16 major = PAL_VERSION_MAJOR(versionHint);
        Uint16 minor = PAL_VERSION_MINOR(versionHint);
        version = VK_MAKE_VERSION(major, minor, 0);
    }

    // layers
    Uint32 layerCount = 0;
    vkEnumerateInstanceLayerPropertiesFn enumerateProperties;
    enumerateProperties = s_VkGPU.enumerateInstanceLayerProperties;
    VkResult ret = enumerateProperties(&layerCount, nullptr);
    if (ret != VK_SUCCESS) {
        s_VkGPU.hasDebug = false;
    }

    VkLayerProperties* props = nullptr;
    props = palAllocate(
        s_VkGPU.allocator, 
        sizeof(VkLayerProperties) * layerCount,
        0);

    if (!props) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    enumerateProperties(&layerCount, props);
    bool hasValidationLayer = false;
    for (int i = 0; i < layerCount; i++) {
        if (strcmp(props[i].layerName, "VK_LAYER_KHRONOS_validation") == 0) {
            hasValidationLayer = true;
            break;
        }
    }

    palFree(s_VkGPU.allocator, props);

    // extensions
    const char* extensions[8];
    vkEnumerateInstanceExtensionPropertiesFn getExtensionProperties;
    getExtensionProperties = s_VkGPU.enumerateInstanceExtensionProperties;

    // get supported extensions
    Uint32 extCount = 0;
    ret = getExtensionProperties(
        nullptr,
        &extCount, 
        nullptr);

    if (ret != VK_SUCCESS) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    VkExtensionProperties* extensionProps = nullptr;
    extensionProps = palAllocate(
        s_VkGPU.allocator, 
        sizeof(VkExtensionProperties) * extCount, 
        0);

    if (!extensionProps) {
        return PAL_RESULT_SUCCESS;
    }

    getExtensionProperties(
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

    palFree(s_VkGPU.allocator, extensionProps);

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
        s_VkGPU.hasDebug = true;
        extensions[extensionCount++] = "VK_EXT_debug_utils";
        layers[layerCount++] = "VK_LAYER_KHRONOS_validation";
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
    VkPhysicalDevice physicalDevice = (VkPhysicalDevice)adapter;
    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDevicePropertiesFn getProperties;
    vkGetPhysicalDeviceMemoryPropertiesFn getMemoryProperties;
    vkGetPhysicalDeviceQueueFamilyPropertiesFn getQueueProperties;
    vkEnumerateDeviceExtensionPropertiesFn getExtensionProperties;
    vkGetPhysicalDeviceFeaturesFn getFeatures;
    vkGetPhysicalDeviceFeatures2Fn getFeatures2;

    getProperties = s_VkGPU.getPhysicalDeviceProperties;
    getMemoryProperties = s_VkGPU.getPhysicalDeviceMemoryProperties;
    getQueueProperties = s_VkGPU.getPhysicalDeviceQueueFamilyProperties;
    getExtensionProperties = s_VkGPU.enumerateDeviceExtensionProperties;
    getFeatures = s_VkGPU.getPhysicalDeviceFeatures;
    getFeatures2 = s_VkGPU.getPhysicalDeviceFeatures2;

    getProperties(physicalDevice, &props);
    getMemoryProperties(physicalDevice, &memProps);

    strcpy(info->name, props.deviceName);
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

    // shader format
    info->shaderFormats = PAL_GPU_SHADER_FORMAT_SPIRV;

    // version string
    snprintf(
        info->versionString, 
        PAL_GPU_VERSION_SIZE, 
        "%d.%d.%d",
        VK_VERSION_MAJOR(props.apiVersion),
        VK_VERSION_MINOR(props.apiVersion),
        VK_VERSION_PATCH(props.apiVersion));

    // get supported queue commands
    Uint32 count;
    getQueueProperties(
        physicalDevice, 
        &count, 
        nullptr);

    // not that huge, we allocate on the stack rather (8 for safety)
    VkQueueFamilyProperties queueProps[8];
    getQueueProperties(
        physicalDevice, 
        &count, 
        queueProps);

    info->commandQueues = 0;
    for (int i = 0; i < count; i++) {
        if (queueProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            info->commandQueues |= PAL_GPU_COMMAND_QUEUE_COMPUTE;
        }

        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            info->commandQueues |= PAL_GPU_COMMAND_QUEUE_GRAPHICS;
        }

        if (queueProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            info->commandQueues |= PAL_GPU_COMMAND_QUEUE_TRANSFER;
        }
    }

    // get supported extensions
    Uint32 extensionCount = 0;
    VkResult ret = getExtensionProperties(
        physicalDevice, 
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
        physicalDevice, 
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
            getFeatures2(physicalDevice, &features);

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
            getFeatures2(physicalDevice, &features);

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
            getFeatures2(physicalDevice, &features);

            if (desc.shaderSampledImageArrayNonUniformIndexing) {
                info->features |= PAL_GPU_FEATURE_DESCRIPTOR_INDEXING;
            }

        } else if (strcmp(props->extensionName, "VK_KHR_swapchain") == 0) {
            // swapchain
            info->features |= PAL_GPU_FEATURE_SWAPCHAIN;

        } else if (strcmp(props->extensionName, "VK_KHR_dynamic_rendering") == 0) {
            // dynamic rendering
            info->features |= PAL_GPU_FEATURE_DYNAMIC_RENDERING;

        } else if (strcmp(props->extensionName, "VK_KHR_shader_float16_int8") == 0) {
            // shader float16
            VkPhysicalDeviceShaderFloat16Int8FeaturesKHR shader16 = {0};
            shader16.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &shader16;
            getFeatures2(physicalDevice, &features);

            if (shader16.shaderFloat16) {
                info->features |= PAL_GPU_FEATURE_SHADER_FLOAT16;
            }

        } else if (strcmp(props->extensionName, "VK_KHR_timeline_semaphore") == 0) {
            // timeline semaphore
            VkPhysicalDeviceTimelineSemaphoreFeatures timeline = {0};
            timeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &timeline;
            getFeatures2(physicalDevice, &features);

            if (timeline.timelineSemaphore) {
                info->features |= PAL_GPU_FEATURE_TIMELINE_SEMAPHORE;
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
        getFeatures2(physicalDevice, &features);

        if (ray.rayTracingPipeline && acc.accelerationStructure) {
            info->features |= PAL_GPU_FEATURE_RAY_TRACING;
        }
    }
    // clang-format on

    getFeatures = s_VkGPU.getPhysicalDeviceFeatures2;
    VkPhysicalDeviceFeatures features;
    getFeatures(physicalDevice, &features);

    // check for additional features
    if (features.geometryShader) {
        info->features |= PAL_GPU_FEATURE_GEOMETRY_SHADER;
    }

    if (features.multiViewport) {
        info->features |= PAL_GPU_FEATURE_MULTI_VIEWPORT;
    }

    if (features.samplerAnisotropy) {
        info->features |= PAL_GPU_FEATURE_SAMPLER_ANISOTROPY;
    }

    if (features.sampleRateShading) {
        info->features |= PAL_GPU_FEATURE_SAMPLE_RATE_SHADING;
    }

    if (features.shaderFloat64) {
        info->features |= PAL_GPU_FEATURE_SHADER_FLOAT64;
    }

    if (features.shaderInt64) {
        info->features |= PAL_GPU_FEATURE_SHADER_INT64;
    }

    if (features.shaderInt16) {
        info->features |= PAL_GPU_FEATURE_SHADER_INT16;
    }

    if (features.tessellationShader) {
        info->features |= PAL_GPU_FEATURE_TESSELLATION_SHADER;
    }

    palFree(s_VkGPU.allocator, extensionProps);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL vkCreateGPUDevice(
    PalGPUAdapter* adapter,
    const PalGPUDeviceCreateInfo* info,
    PalGPUDevice** outDevice)
{
    VkDevice device = nullptr;
    VkPhysicalDevice physicalDevice = (VkPhysicalDevice)adapter;

    // check if the requested queue is supported ny the Adapter and select it
    vkGetPhysicalDeviceQueueFamilyPropertiesFn getQueueProperties;
    getQueueProperties = s_VkGPU.getPhysicalDeviceQueueFamilyProperties;
    Uint32 count;
    getQueueProperties(
        physicalDevice, 
        &count, 
        nullptr);

    // not that huge, we allocate on the stack rather (8 for safety)
    VkQueueFamilyProperties queueProps[8];
    getQueueProperties(
        physicalDevice, 
        &count, 
        queueProps);

    // find the index of all the supported queue families
    Int32 computeFamily, graphicsFamily, transferFamily = -1;
    for (int i = 0; i < count; i++) {
        if (queueProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            if (computeFamily == -1) {
                computeFamily = i;
            }
        }

        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            if (graphicsFamily == -1) {
                graphicsFamily = i;
            }
        }

        if (queueProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            if (transferFamily == -1) {
                transferFamily = i;
            }
        }
    }

    // build queue families
    Int32 queueFamilies[3]; // compute, graphics, transfer
    Int32 queueFamilyCount = 0;
    if (info->commandQueues & PAL_GPU_COMMAND_QUEUE_COMPUTE) {
        if (computeFamily == -1) {
            // not supported
            return PAL_RESULT_GPU_COMMAND_QUEUE_NOT_SUPPORTED;
        }
        queueFamilies[queueFamilyCount++] = computeFamily;
    }

    if (info->commandQueues & PAL_GPU_COMMAND_QUEUE_GRAPHICS) {
        if (graphicsFamily == -1) {
            // not supported
            return PAL_RESULT_GPU_COMMAND_QUEUE_NOT_SUPPORTED;
        }

        if (graphicsFamily != computeFamily) {
            // different families, add a new family entry
            queueFamilies[queueFamilyCount++] = graphicsFamily;
        }        
    }

    if (info->commandQueues & PAL_GPU_COMMAND_QUEUE_TRANSFER) {
        if (transferFamily == -1) {
            // not supported
            return PAL_RESULT_GPU_COMMAND_QUEUE_NOT_SUPPORTED;
        }

        if (transferFamily != computeFamily && 
            transferFamily != graphicsFamily) {
            // different families, add a new family entry
            queueFamilies[queueFamilyCount++] = transferFamily;
        }
    }

    float priority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfos[3];
    for (int i = 0; i < queueFamilyCount; i++) {
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].pNext = nullptr;
        queueCreateInfos[i].pQueuePriorities = &priority;
        queueCreateInfos[i].queueFamilyIndex = queueFamilies[i];
        queueCreateInfos[i].queueCount = 1;
        queueCreateInfos[i].flags = 0;
    }

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pEnabledFeatures = 0;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = nullptr;
    createInfo.pQueueCreateInfos = queueCreateInfos;
    createInfo.queueCreateInfoCount = queueFamilyCount;
    createInfo.pNext = nullptr;

    // VkResult result = vkCreateDevice(
    //     physicalDevice, 
    //     &createInfo, 
    //     &s_VkAllocator, 
    //     &device);

    // if (result != VK_SUCCESS) {

    // }

    *outDevice = (PalGPUDevice*)device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL vkDestroyGPUDevice(PalGPUDevice* device)
{
    // vkDestroyDevice((VkDevice)device, &s_VkAllocator);
}

static PalGPUBackend s_VkBackend = {
    .enumerateGPUAdapters = vkEnumerateAdapters,
    .getGPUAdapterInfo = vkGetAdapterInfo
};

#endif // PAL_HAS_VULKAN

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palInitGraphics(
    bool enableDebug,
    Int32 versionHint,
    const PalAllocator* allocator)
{
    if (s_VkGPU.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    s_VkGPU.allocator = allocator;
#if PAL_HAS_VULKAN
    PalResult ret = vkInitGraphics(
        enableDebug, 
        versionHint);

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
        !backend->getGPUAdapterInfo    ||
        !backend->createGPUDevice    ||
        !backend->destroyGPUDevice) {
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
    PalGPUAdapter* adapter,
    const PalGPUDeviceCreateInfo* info,
    PalGPUDevice** outDevice)
{
    if (!s_VkGPU.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!info || !outDevice) {
        return PAL_RESULT_NULL_POINTER;
    }

    // check if the adapter is from PAL (custom or internal backend)
    AdapterData* adapterData = findAdapterData(adapter);
    if (!adapterData) {
        return PAL_RESULT_INVALID_GPU_ADAPTER;
    }

    PalGPUDevice* device = nullptr;
    PalResult ret;
    ret = adapterData->backend->createGPUDevice(adapter, info, &device);
    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    // create a slot for the created device
    DeviceData* deviceData = getFreeDeviceData();
    if (!deviceData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    deviceData->adapterData = adapterData;
    deviceData->device = device;

    *outDevice = device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyGPUDevice(PalGPUDevice* device)
{
    if (s_VkGPU.initialized && device) {
        DeviceData* data = findDeviceData(device);
        if (data) {
            data->adapterData->backend->destroyGPUDevice(device);
        }
        data->used = false;
    }
}