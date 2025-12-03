
#include "pal/pal_graphics.h"
#include "tests.h"

bool imageTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Image Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    // initialize the graphics system
    PalResult result = palInitGraphics(false, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize graphics: %s", error);
        return false;
    }

    // enumerate all available adapters
    Int32 count = 0;
    result = palEnumerateAdapters(&count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get query adapters: %s", error);
        return false;
    }

    if (count == 0) {
        palLog(nullptr, "No adapters found");
        return false;
    }
    palLog(nullptr, "Adapter count: %d", count);

    // allocate an array of adapters or use a fixed array
    // Example: PalAdapter* adapters[12];
    PalAdapter** adapters = nullptr;
    adapters = palAllocate(nullptr, sizeof(PalAdapter*) * count, 0);
    if (!adapters) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    result = palEnumerateAdapters(&count, adapters);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get query adapters: %s", error);
        return false;
    }

    // filter the adapters for Vulkan
    PalAdapter* vulkanAdapter = nullptr;
    PalAdapterInfo info = {0};

    for (Int32 i = 0; i < count; i++) {
        PalAdapter* adapter = adapters[i];
        result = palGetAdapterInfo(adapter, &info);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter info: %s", error);
            palFree(nullptr, adapters);
            return false;
        }

        // check if its Vulkan
        if (info.apiType == PAL_ADAPTER_API_TYPE_VULKAN) {
            vulkanAdapter = adapter;
            break;
        }
    }

    palFree(nullptr, adapters);
    if (!vulkanAdapter) {
        palLog(nullptr, "Failed to find a vulkan adapter");
        return false;
    }

    // get capabilities about the adapter
    PalAdapterCapabilities caps = {0};
    result = palGetAdapterCapabilities(vulkanAdapter, &caps);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get adapter capabilities: %s", error);
        return false;
    }

    // create a device with the vulkan adapter
    PalDevice* device = nullptr;
    PalAdapterFeatures features = PAL_ADAPTER_FEATURE_SWAPCHAIN;
    result = palCreateDevice(vulkanAdapter, features, &device);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create device: %s", error);
        return false;
    }

    // create a simple 2d image allocated on the gpu
    // we can either enumerate all the supported formats andd choose one
    // or we can choose our preffered format and check for support

    // use palEnumerateFormats() to get all supported formats 
    // and their image usages

    PalFormat format = PAL_FORMAT_R8G8B8A8_UNORM;
    PalImageUsages usage = PAL_IMAGE_USAGE_COLOR_ATTACHEMENT;
    if (!palIsFormatSupported(vulkanAdapter, format)) {
        palLog(nullptr, "The preffered format is not supported");
        return false;
    }

    if(!(usage & palQueryFormatUsages(vulkanAdapter, format))) {
        palLog(nullptr, 
            "The preffered format does not support color attachement");
        return false;
    }

    PalImage* image = nullptr;
    PalImageCreateInfo imageCreateInfo = {0};
    imageCreateInfo.format.format = format;
    imageCreateInfo.format.usages = usage;
    imageCreateInfo.height = 240;
    imageCreateInfo.mipLevels = 1; 
    imageCreateInfo.samples = 1; // very simple
    imageCreateInfo.width = 320;

    // if the image type is 1D or 2D
    // PalImageCreateInfo::depthOrArraySize is used for the array size
    imageCreateInfo.type == PAL_IMAGE_TYPE_2D;
    imageCreateInfo.depthOrArraySize = 1;

    result = palCreateImage(device, &imageCreateInfo, &image);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create image: %s", error);
        return false;
    }

    // bind memory to the create image
    PalMemoryRequirements imageMemReq;
    result = palGetImageMemoryRequirements(device, image, &imageMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get image memory requirements: %s", error);
        return false;
    }

    // allocate memory for the image
    if (!imageMemReq.memoryTypeAllowed[PAL_MEMORY_TYPE_GPU_ONLY]) {
        palLog(nullptr, "Cannot allocate gpu only memory");
    }

    PalMemory* imageMemory = nullptr;
    result = palAllocateMemory(
        device, 
        PAL_MEMORY_TYPE_GPU_ONLY, 
        imageMemReq.size, 
        &imageMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory for image: %s", error);
        return false;
    }

    PalImageViewUsages viewUsage = PAL_IMAGE_VIEW_USAGE_COLOR;
    if(!(viewUsage & palQueryFormatViewUsages(vulkanAdapter, format))) {
        palLog(nullptr, 
            "The preffered format does not support color image view");
        return false;
    }

    // create an image view from the image
    PalImageView* imageView = nullptr;
    PalImageViewCreateInfo imageViewCreateInfo = {0};
    imageViewCreateInfo.startMipLevel = 0; // start from the first
    imageViewCreateInfo.startArrayLayer = 0; // start from the first
    imageViewCreateInfo.layerArrayCount = imageCreateInfo.depthOrArraySize;
    imageViewCreateInfo.mipLevelCount = imageCreateInfo.mipLevels;
    imageViewCreateInfo.type = PAL_IMAGE_VIEW_TYPE_2D;

    result = palCreateImageView(
        device, 
        image, 
        &imageViewCreateInfo, 
        &imageView);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create image view: %s", error);
        return false;
    }

    palDestroyImageView(imageView);

    // bind the memory to the image
    result = palBindImageMemory(
        device, 
        image, 
        imageMemory, 
        PAL_DEFAULT_MEMORY_OFFSET);
        
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind image memory: %s", error);
        return false;
    }

    // destroy image
    palDestroyImage(image);

    // free the image memory
    palFreeMemory(device, imageMemory);

    // destroy the device
    palDestroyDevice(device);

    // shutdown the graphics system
    palShutdownGraphics();
    
    return true;
}