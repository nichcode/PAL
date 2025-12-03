
#include "pal/pal_graphics.h"
#include "pal/pal_video.h"
#include "tests.h"

bool swapchainTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Swapchain Test");
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
        palLog(nullptr, "Failed to get adapter info: %s", error);
        return false;
    }

    if (caps.maxGraphicsQueues == 0) {
        palLog(nullptr, "adapter does not support graphics queues");
        return false;
    }

    // create a device with the vulkan adapter
    PalDevice* device = nullptr;
    PalAdapterFeatures features = PAL_ADAPTER_FEATURE_SWAPCHAIN;
    
    // enable multi viewport if supported
    if (caps.features & PAL_ADAPTER_FEATURE_MULTI_VIEWPORT) {
        features |= PAL_ADAPTER_FEATURE_MULTI_VIEWPORT;
    }

    result = palCreateDevice(vulkanAdapter, features, &device);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create device: %s", error);
        return false;
    }

    // create a graphics queue
    PalQueue* gfxQueue = nullptr;
    result = palCreateQueue(device, PAL_QUEUE_TYPE_GRAPHICS, &gfxQueue);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create queue: %s", error);
        return false;
    }

    /// we need a window. We use PAL video system to create the window
    result = palInitVideo(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video: %s", error);
        return false;
    }

    PalWindow* window = nullptr;
    PalWindowCreateInfo createInfo = {0};
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = true;

    // check if we support decorated windows (title bar, close etc)
    PalVideoFeatures64 videoFeatures = palGetVideoFeaturesEx();
    if (!(videoFeatures & PAL_VIDEO_FEATURE64_DECORATED_WINDOW)) {
        // if we dont support, we need to create a borderless window
        // and create the decorations ourselves
        createInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create window: %s", error);
        return false;
    }

    PalGfxWindow gfxWindow = {0};
    PalWindowHandleInfo winInfo = palGetWindowHandleInfo(window);
    gfxWindow.display = winInfo.nativeDisplay;
    gfxWindow.window = winInfo.nativeWindow;

    bool canPresent = palCanQueuePresent(gfxQueue, &gfxWindow);
    if (!canPresent) {
        palLog(nullptr, "Queue could not present to window");
        return false;
    }

    // query swapchain capabilities of the adapter and the window
    PalSwapchainCapabilities swapchainCaps = {0};
    result = palQuerySwapchainCapabilities(
        vulkanAdapter, 
        &gfxWindow, 
        &swapchainCaps);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to query swapchain capabilities: %s", error);
        return false;
    }
    
    // log swapchain capabilities
    Uint32 imageArrayLayers = swapchainCaps.maxImageArrayLayers;
    palLog(nullptr, "Swapchain capabilities:");
    palLog(nullptr, " Min image count: %d", swapchainCaps.minImageCount);
    palLog(nullptr, " Max image count: %d", swapchainCaps.maxImageCount);
    palLog(nullptr, " Max image array layers: %d", imageArrayLayers);

    palLog(nullptr, " Min image width: %d", swapchainCaps.minImageWidth);
    palLog(nullptr, " Min image height: %d", swapchainCaps.minImageHeight);
    palLog(nullptr, " Max image width: %d", swapchainCaps.maxImageWidth);
    palLog(nullptr, " Max image height: %d", swapchainCaps.maxImageHeight);

    Uint32 bgraSrgbSrgb = PAL_SWAPCHAIN_FORMAT_BGRA8_SRGB_SRGB;
    Uint32 bgraUnormSrgb = PAL_SWAPCHAIN_FORMAT_BGRA8_UNORM_SRGB;
    Uint32 rgbaFloatHdr = PAL_SWAPCHAIN_FORMAT_RGBA16_FLOAT_HDR10;
    Uint32 rgbaUnormSrgb = PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB;

    palLog(nullptr, " Supported formats:");
    if (swapchainCaps.swapchainFormatsAllowed[bgraSrgbSrgb]) {
        palLog(nullptr, "  BGRA8 SRGB SRGB");
    }

    if (swapchainCaps.swapchainFormatsAllowed[bgraUnormSrgb]) {
        palLog(nullptr, "  BGRA8 UNORM SRGB");
    }

    if (swapchainCaps.swapchainFormatsAllowed[rgbaFloatHdr]) {
        palLog(nullptr, "  RGBA16 FLOAT HDR10");
    }

    if (swapchainCaps.swapchainFormatsAllowed[rgbaUnormSrgb]) {
        palLog(nullptr, "  RGBA8 UNORM SRGB");
    }

    Uint32 fifo = PAL_PRESENT_MODE_FIFO;
    Uint32 immediate = PAL_PRESENT_MODE_IMMEDIATE;
    Uint32 mailbox = PAL_PRESENT_MODE_MAILBOX;

    palLog(nullptr, " Supported present modes:");
    if (swapchainCaps.presentModessAllowed[fifo]) {
        palLog(nullptr, "  FIFO");
    }

    if (swapchainCaps.presentModessAllowed[immediate]) {
        palLog(nullptr, "  Immediate");
    }

    if (swapchainCaps.presentModessAllowed[mailbox]) {
        palLog(nullptr, "  Mailbox");
    }

    Uint32 opaque = PAL_COMPOSITE_ALPHA_OPAQUE;
    Uint32 postMultiplied = PAL_COMPOSITE_ALPHA_POST_MULTIPLIED;
    Uint32 preMultiplied = PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED;

    palLog(nullptr, " Supported composite alphas:");
    if (swapchainCaps.compositeAlphasAllowed[opaque]) {
        palLog(nullptr, "  Opaque");
    }

    if (swapchainCaps.compositeAlphasAllowed[preMultiplied]) {
        palLog(nullptr, "  Pre Multiplied");
    }

    if (swapchainCaps.compositeAlphasAllowed[postMultiplied]) {
        palLog(nullptr, "  Post Multiplied");
    }

    // create swapchain
    PalSwapchain* swapchain = nullptr;
    PalSwapchainCreateInfo swapchainCreateInfo = {0};
    swapchainCreateInfo.clipped = true;
    swapchainCreateInfo.compositeAlpha = opaque;
    swapchainCreateInfo.format = rgbaUnormSrgb;
    swapchainCreateInfo.imageArrayLayerCount = 1; // 2 VR

    // check the number and increment it but not passed
    // PalSwapchainCapabilities::maxImageCount
    swapchainCreateInfo.imageCount = swapchainCaps.minImageCount;
    swapchainCreateInfo.presentMode = fifo;

    // set size
    swapchainCreateInfo.width = 640;
    swapchainCreateInfo.height = 480;
    if (640 > swapchainCaps.maxImageWidth) {
        // we set it to the minimal to mak it work across systems
        swapchainCreateInfo.width = swapchainCaps.minImageWidth;
    }

    if (480 > swapchainCaps.maxImageHeight) {
        // we set it to the minimal to mak it work across systems
        swapchainCreateInfo.height = swapchainCaps.minImageHeight;
    }

    result = palCreateSwapchain(
        device,
        gfxQueue, 
        &gfxWindow, 
        &swapchainCreateInfo,
        &swapchain);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create swapchain: %s", error);
        return false;
    }

    // if you need to get all images and create image views for them
    // use palGetSwapchainImage() to get the image count.
    // but this is the same number as the requested images 
    // when creating the swapchain

    // we only get the first image and use it for our needs
    PalImage* swapchainImage = palGetSwapchainImage(swapchain, 0);
    if (!swapchainImage) {
        palLog(nullptr, "Failed to get swapchain image");
        return false;
    }

    // create an image view for the image
    // swapchain images do not need memory to be bound to them
    PalImageView* imageView = nullptr;
    PalImageViewCreateInfo imageViewCreateInfo = {0};
    PalImageInfo imageInfo = {0};

    // get the swapchain info and use it to create the image view
    result = palGetImageInfo(swapchainImage, &imageInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get image info: %s", error);
        return false;
    }

    imageViewCreateInfo.layerArrayCount = imageInfo.depthOrArraySize;
    imageViewCreateInfo.mipLevelCount = imageInfo.mipLevelCount;
    imageViewCreateInfo.startArrayLayer = 0; // always 0 for swapchain
    imageViewCreateInfo.startMipLevel = 0; // always 0 for swapchain
    imageViewCreateInfo.type = PAL_IMAGE_VIEW_TYPE_2D; // only 2D
    imageViewCreateInfo.usages = PAL_IMAGE_VIEW_USAGE_COLOR;

    result = palCreateImageView(
        device,
        swapchainImage, 
        &imageViewCreateInfo, 
        &imageView);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create image view: %s", error);
        return false;
    }

    // destroy the image view
    palDestroyImageView(imageView);

    // destroy the swapchain
    palDestroySwapchain(swapchain);

    palDestroyWindow(window);
    palShutdownVideo();

    // destroy the graphics queue
    palDestroyQueue(gfxQueue);

    // destroy the device
    palDestroyDevice(device);

    // shutdown the graphics system
    palShutdownGraphics();
    
    return true;
}
