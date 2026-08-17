
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

PalResult PAL_CALL createSurfaceD3D12(
    PalDevice* device,
    void* window,
    void* windowInstance,
    PalWindowInstanceType instanceType,
    PalSurface** outSurface)
{
    SurfaceD3D12* surface = nullptr;
    surface = palAllocate(s_D3D12.allocator, sizeof(SurfaceD3D12), 0);
    if (!surface) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // validate if the window is valid
    if (!IsWindow((HWND)window)) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    surface->handle = window;
    *outSurface = (PalSurface*)surface;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySurfaceD3D12(PalSurface* surface)
{
    SurfaceD3D12* d3dSurface = (SurfaceD3D12*)surface;
    palFree(s_D3D12.allocator, d3dSurface);
}

void PAL_CALL getSurfaceCapabilitiesD3D12(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps)
{
    SurfaceD3D12* d3dSurface = (SurfaceD3D12*)surface;
    DeviceD3D12* deviceImpl = (DeviceD3D12*)device;
    PalBool supportHDR10 = PAL_FALSE;
    IDXGISwapChain1* swapchain1 = nullptr;
    IDXGISwapChain3* swapchain3 = nullptr;

    DXGI_SWAP_CHAIN_DESC1 desc = {0};
    desc.Width = 8;
    desc.Height = 8;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    s_D3D12.factory->lpVtbl->CreateSwapChainForHwnd(
        s_D3D12.factory,
        (IUnknown*)deviceImpl->queue,
        d3dSurface->handle,
        &desc,
        nullptr,
        nullptr,
        &swapchain1);

    swapchain1->lpVtbl->QueryInterface(swapchain1, &IID_Swapchain, (void**)&swapchain3);
    swapchain1->lpVtbl->Release(swapchain1);

    // check for HDR10 color space support
    UINT flags = 0;
    swapchain3->lpVtbl->CheckColorSpaceSupport(
        swapchain3,
        DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020,
        &flags);

    if (flags & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) {
        supportHDR10 = PAL_TRUE;
    }

    BOOL allowTearing = PAL_FALSE;
    s_D3D12.factory->lpVtbl->CheckFeatureSupport(
        s_D3D12.factory,
        DXGI_FEATURE_PRESENT_ALLOW_TEARING,
        &allowTearing,
        sizeof(allowTearing));

    caps->minImageCount = 2;
    caps->supportedPresentModes = (1u << PAL_PRESENT_MODE_FIFO);
    if (allowTearing) {
        caps->minImageCount = 3;
        caps->supportedPresentModes |= (1u << PAL_PRESENT_MODE_IMMEDIATE);
        caps->supportedPresentModes |= (1u << PAL_PRESENT_MODE_MAILBOX);
    }

    caps->supportedCompositeAlphas |= (1u << PAL_COMPOSITE_ALPHA_OPAQUE);

    caps->maxImageCount = 8; // safe default
    caps->minImageWidth = 1;
    caps->minImageHeight = 1;
    caps->maxImageWidth = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    caps->maxImageHeight = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    caps->maxImageArrayLayers = 1;

    // check support for the base format
    caps->supportedFormats = 0;
    D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = {0};
    DXGI_FORMAT baseFormats[PAL_SURFACE_FORMAT_COUNT];
    baseFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
    baseFormats[1] = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    baseFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    baseFormats[3] = DXGI_FORMAT_R16G16B16A16_FLOAT;

    for (int i = 0; i < PAL_SURFACE_FORMAT_COUNT; i++) {
        formatSupport.Format = baseFormats[i];
        deviceImpl->handle->lpVtbl->CheckFeatureSupport(
            deviceImpl->handle,
            D3D12_FEATURE_FORMAT_SUPPORT,
            &formatSupport,
            sizeof(formatSupport));

        if (formatSupport.Support1 != 0 || formatSupport.Support2 != 0) {
            if (baseFormats[i] == DXGI_FORMAT_B8G8R8A8_UNORM) {
                caps->supportedFormats |= (1u << PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR);
            }

            if (baseFormats[i] == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB) {
                caps->supportedFormats |= (1u << PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR);
            }

            if (baseFormats[i] == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB) {
                caps->supportedFormats |= (1u << PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR);
            }

            if (baseFormats[i] == DXGI_FORMAT_R16G16B16A16_FLOAT) {
                // check HDR10 color space
                if (supportHDR10) {
                    caps->supportedFormats |= (1u << PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10);
                }
            }
        }
    }

    swapchain3->lpVtbl->Release(swapchain3);
}

PalResult PAL_CALL createSwapchainD3D12(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain)
{
    HRESULT result;
    SurfaceD3D12* d3dSurface = (SurfaceD3D12*)surface;
    DeviceD3D12* deviceImpl = (DeviceD3D12*)device;
    QueueD3D12* queueImpl = (QueueD3D12*)queue;
    SwapchainD3D12* swapchain = nullptr;
    PalBool isHDRColorspace = PAL_FALSE;

    if (queueImpl->type != PAL_QUEUE_TYPE_GRAPHICS) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (info->compositeAlpha != PAL_COMPOSITE_ALPHA_OPAQUE) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (info->presentMode == PAL_PRESENT_MODE_MAILBOX && info->imageCount < 3) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    swapchain = palAllocate(s_D3D12.allocator, sizeof(SwapchainD3D12), 0);
    if (!swapchain) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    IDXGISwapChain1* swapchain1 = nullptr;
    DXGI_SWAP_CHAIN_DESC1 desc = {0};
    desc.Width = info->width;
    desc.Height = info->height;
    desc.SampleDesc.Count = 1;
    desc.BufferCount = info->imageCount;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.Stereo = PAL_FALSE;
    desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    desc.Scaling = DXGI_SCALING_NONE;

    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    if (info->presentMode == PAL_PRESENT_MODE_FIFO) {
        swapchain->presentFlags = 0;
        swapchain->syncInterval = 1;

    } else if (info->presentMode == PAL_PRESENT_MODE_IMMEDIATE) {
        desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        swapchain->presentFlags = DXGI_PRESENT_ALLOW_TEARING;
        swapchain->syncInterval = 0;

    } else {
        desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        swapchain->presentFlags = DXGI_PRESENT_ALLOW_TEARING;
        swapchain->syncInterval = 0;
    }

    PalFormat imageFormat;
    if (info->format == PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR) {
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        imageFormat = PAL_FORMAT_B8G8R8A8_UNORM;

    } else if (info->format == PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR) {
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        imageFormat = PAL_FORMAT_B8G8R8A8_SRGB;

    } else if (info->format == PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR) {
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        imageFormat = PAL_FORMAT_R8G8B8A8_SRGB;

    } else if (info->format == PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10) {
        desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        imageFormat = PAL_FORMAT_R16G16B16A16_SFLOAT;
        isHDRColorspace = PAL_TRUE;
    }

    swapchain->format = desc.Format;
    swapchain->flags = desc.Flags;
    result = s_D3D12.factory->lpVtbl->CreateSwapChainForHwnd(
        s_D3D12.factory,
        (IUnknown*)queueImpl->handle,
        d3dSurface->handle,
        &desc,
        nullptr,
        nullptr,
        &swapchain1);

    if (FAILED(result)) {
        return makeResultD3D12(result);
    }
    pollMessagesD3D12(deviceImpl);

    swapchain1->lpVtbl->QueryInterface(swapchain1, &IID_Swapchain, (void**)&swapchain->handle);
    swapchain1->lpVtbl->Release(swapchain1);

    if (isHDRColorspace) {
        swapchain->handle->lpVtbl->SetColorSpace1(
            swapchain->handle,
            DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
    } else {
        swapchain->handle->lpVtbl->SetColorSpace1(
            swapchain->handle,
            DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709);
    }

    // get and cache swapchain images
    swapchain->images = nullptr;
    swapchain->images = palAllocate(s_D3D12.allocator, sizeof(ImageD3D12) * info->imageCount, 0);
    if (!swapchain->imageCount) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // fill all images with the creation info
    for (int i = 0; i < info->imageCount; i++) {
        ID3D12Resource* tmp = nullptr;
        swapchain->handle->lpVtbl->GetBuffer(swapchain->handle, i, &IID_Resource, (void**)&tmp);

        ImageD3D12* image = &swapchain->images[i];
        image->device = deviceImpl;
        image->handle = tmp;

        image->info.belongsToSwapchain = PAL_TRUE;
        image->info.depth = 1;           // always 1
        image->info.arrayLayerCount = 1; // always 1
        image->info.format = imageFormat;
        image->info.usages = PAL_IMAGE_USAGE_COLOR_ATTACHEMENT;
        image->info.height = info->height;
        image->info.width = info->width;
        image->info.mipLevelCount = 1;
        image->info.sampleCount = PAL_SAMPLE_COUNT_1; // swapchain images are not multisampled
        image->info.type = PAL_IMAGE_TYPE_2D;
    }

    // get and cache window size for swapchain out of date error
    RECT windowRect;
    GetClientRect((HWND)d3dSurface->handle, &windowRect);
    swapchain->windowWidth = windowRect.right - windowRect.left;
    swapchain->windowWidth = windowRect.bottom - windowRect.top;

    swapchain->device = deviceImpl;
    swapchain->queue = queueImpl->handle;
    swapchain->imageCount = info->imageCount;
    *outSwapchain = (PalSwapchain*)swapchain;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySwapchainD3D12(PalSwapchain* swapchain)
{
    SwapchainD3D12* swapchainImpl = (SwapchainD3D12*)swapchain;
    swapchainImpl->handle->lpVtbl->Release(swapchainImpl->handle);
    palFree(s_D3D12.allocator, swapchainImpl->images);
    palFree(s_D3D12.allocator, swapchainImpl);
}

PalImage* PAL_CALL getSwapchainImageD3D12(
    PalSwapchain* swapchain,
    uint32_t index)
{
    SwapchainD3D12* swapchainImpl = (SwapchainD3D12*)swapchain;
    if (index > swapchainImpl->imageCount) {
        return nullptr;
    }
    return (PalImage*)&swapchainImpl->images[index];
}

PalResult PAL_CALL getNextSwapchainImageD3D12(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    uint32_t* outIndex)
{
    HRESULT result;
    uint32_t index = 0;
    SwapchainD3D12* swapchainImpl = (SwapchainD3D12*)swapchain;
    ID3D12CommandQueue* queue = swapchainImpl->queue;

    index = swapchainImpl->handle->lpVtbl->GetCurrentBackBufferIndex(swapchainImpl->handle);
    if (info->fence) {
        FenceD3D12* fence = (FenceD3D12*)info->fence;
        fence->value++;
        result = queue->lpVtbl->Signal(queue, fence->handle, fence->value);
        if (FAILED(result)) {
            return makeResultD3D12(result);
        }
        pollMessagesD3D12(swapchainImpl->device);
    }

    if (info->signalSemaphore) {
        SemaphoreD3D12* semaphore = (SemaphoreD3D12*)info->signalSemaphore;
        semaphore->value = 1;
        result = queue->lpVtbl->Signal(queue, semaphore->handle, semaphore->value);
        if (FAILED(result)) {
            return makeResultD3D12(result);
        }
    }

    *outIndex = index;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL presentSwapchainD3D12(
    PalSwapchain* swapchain,
    uint32_t imageIndex,
    PalSemaphore* waitSemaphore)
{
    HRESULT result;
    SwapchainD3D12* swapchainImpl = (SwapchainD3D12*)swapchain;
    ID3D12CommandQueue* queue = swapchainImpl->queue;

    if (waitSemaphore) {
        SemaphoreD3D12* semaphore = (SemaphoreD3D12*)waitSemaphore;
        result = queue->lpVtbl->Wait(queue, semaphore->handle, semaphore->value);
        if (FAILED(result)) {
            return makeResultD3D12(result);
        }

        semaphore->value = 0;
        result = semaphore->handle->lpVtbl->Signal(semaphore->handle, 0);
        if (FAILED(result)) {
            return makeResultD3D12(result);
        }
    }

    // poll pending messages
    pollMessagesD3D12(swapchainImpl->device);

    result = swapchainImpl->handle->lpVtbl->Present(
        swapchainImpl->handle,
        swapchainImpl->syncInterval,
        swapchainImpl->presentFlags);

    if (FAILED(result)) {
        if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET) {
            return makeResultD3D12(result);
        }

        // check if swapchain needs to be resize
        RECT windowRect;
        PalBool ret = GetClientRect((HWND)swapchainImpl->surface->handle, &windowRect);
        uint32_t w = windowRect.right - windowRect.left;
        uint32_t h = windowRect.bottom - windowRect.top;
        if (!ret) {
            return palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE,
                PAL_RESULT_SOURCE_WIN32,
                GetLastError());
        }

        if (w != swapchainImpl->windowWidth || h != swapchainImpl->windowHeight) {
            return PAL_RESULT_CODE_OUT_OF_DATE;
        }

        return makeResultD3D12(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resizeSwapchainD3D12(
    PalSwapchain* swapchain,
    uint32_t newWidth,
    uint32_t newHeight)
{
    HRESULT result;
    SwapchainD3D12* swapchainImpl = (SwapchainD3D12*)swapchain;
    result = swapchainImpl->handle->lpVtbl->ResizeBuffers(
        swapchainImpl->handle,
        swapchainImpl->imageCount,
        newWidth,
        newHeight,
        swapchainImpl->format,
        swapchainImpl->flags);

    if (FAILED(result)) {
        return makeResultD3D12(result);
    }
    pollMessagesD3D12(swapchainImpl->device);

    // fill all images with the creation info
    for (int i = 0; i < swapchainImpl->imageCount; i++) {
        ID3D12Resource* tmp = nullptr;
        swapchainImpl->handle->lpVtbl
            ->GetBuffer(swapchainImpl->handle, i, &IID_Resource, (void**)&tmp);

        ImageD3D12* image = &swapchainImpl->images[i];
        image->handle = tmp;
        image->info.height = newHeight;
        image->info.width = newWidth;
    }

    // get and cache window size for swapchain out of date error
    RECT windowRect;
    SurfaceD3D12* surface = swapchainImpl->surface;
    if (!GetClientRect((HWND)surface->handle, &windowRect)) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE,
            PAL_RESULT_SOURCE_WIN32,
            GetLastError());
    }

    swapchainImpl->windowWidth = windowRect.right - windowRect.left;
    swapchainImpl->windowWidth = windowRect.bottom - windowRect.top;
    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_D3D12_BACKEND