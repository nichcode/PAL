
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

PalAdapterFeatures PAL_CALL getAdapterFeaturesD3D12(PalAdapter*);
uint32_t PAL_CALL getHighestSupportedShaderTargetD3D12(PalAdapter*, PalShaderFormats);

static void convertToWcharD3D12(
    const char* src,
    wchar_t dst[PAL_SHADER_ENTRY_NAME_SIZE])
{
    int i = 0;
    for (; i < PAL_SHADER_ENTRY_NAME_SIZE - 1 && src[i]; i++) {
        dst[i] = (wchar_t)src[i];
    }
    dst[i] = L'\0';
}

PalResult PAL_CALL createDeviceD3D12(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice)
{
    HRESULT result;
    DeviceD3D12* device = nullptr;
    AdapterD3D12* d3d12Adapter = (AdapterD3D12*)adapter;

    // check if any of the features are not supported
    PalAdapterFeatures adapterFeatures = getAdapterFeaturesD3D12(adapter);
    PalBool valid = (adapterFeatures & features) == features;
    if (!valid) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    device = palAllocate(s_D3D12.allocator, sizeof(DeviceD3D12), 0);
    if (!device) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    ID3D12Device* tmpDevice = nullptr;
    memset(device, 0, sizeof(DeviceD3D12));
    DeviceLimits* limits = &device->limits;

    // get and cache highest shader model
    device->shaderModel = getHighestSupportedShaderTargetD3D12(adapter, PAL_SHADER_FORMAT_DXIL);
    result = s_D3D12.createDevice(
        (IUnknown*)d3d12Adapter->handle,
        d3d12Adapter->level,
        &IID_Device,
        (void**)&tmpDevice);

    if (FAILED(result)) {
        palFree(s_D3D12.allocator, device);
        return makeResultD3D12(result);
    }

    result = tmpDevice->lpVtbl->QueryInterface(
        tmpDevice,
        &IID_Device5,
        (void**)&device->handle);

    tmpDevice->lpVtbl->Release(tmpDevice);
    if (s_D3D12.debugLayer) {
        result = device->handle->lpVtbl->QueryInterface(
            device->handle,
            &IID_InfoQueue,
            (void**)&device->infoQueue);

        if (SUCCEEDED(result)) {
            // allocate a scratch buffer for messages. 16KB
            device->scratchBuffer = palAllocate(s_D3D12.allocator, 16384, 0);
            if (!device->scratchBuffer) {
                return PAL_RESULT_CODE_OUT_OF_MEMORY;
            }

            D3D12_MESSAGE_ID denyIDs[] = { 
                D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
                D3D12_MESSAGE_ID_LIVE_OBJECT_SUMMARY
            };

            device->infoQueue->lpVtbl->SetBreakOnSeverity(
                device->infoQueue, 
                D3D12_MESSAGE_SEVERITY_ERROR, 
                TRUE);

            device->infoQueue->lpVtbl->SetBreakOnSeverity(
                device->infoQueue, 
                D3D12_MESSAGE_SEVERITY_CORRUPTION, 
                TRUE);

            D3D12_INFO_QUEUE_FILTER filter = {0};
            filter.AllowList.NumSeverities = s_D3D12.severityCount;
            filter.AllowList.pSeverityList = s_D3D12.severities;
            filter.AllowList.NumCategories = s_D3D12.categoryCount;
            filter.AllowList.pCategoryList = s_D3D12.categories;
            filter.DenyList.NumIDs = sizeof(denyIDs) / sizeof(D3D12_MESSAGE_ID);
            filter.DenyList.pIDList = denyIDs;
            device->infoQueue->lpVtbl->PushStorageFilter(device->infoQueue, &filter);
        }
    }

    // create a temporary graphics queue
    D3D12_COMMAND_QUEUE_DESC desc = {0};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    result = device->handle->lpVtbl->CreateCommandQueue(
        device->handle,
        &desc,
        &IID_Queue,
        (void**)&device->queue);

    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    // create command signatures
    D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {0};
    D3D12_COMMAND_SIGNATURE_DESC signatureDesc = {0};
    signatureDesc.NumArgumentDescs = 1;
    signatureDesc.pArgumentDescs = &argumentDesc;

    if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH) {
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH;
        signatureDesc.ByteStride = sizeof(D3D12_DISPATCH_MESH_ARGUMENTS);

        result = device->handle->lpVtbl->CreateCommandSignature(
            device->handle,
            &signatureDesc,
            nullptr,
            &IID_CommandSignature,
            (void**)&device->meshSignature);

        if (FAILED(result)) {
            return makeResultD3D12(result);
        }
    }

    if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_RAYS;
        signatureDesc.ByteStride = sizeof(D3D12_DISPATCH_RAYS_DESC);

        result = device->handle->lpVtbl->CreateCommandSignature(
            device->handle,
            &signatureDesc,
            nullptr,
            &IID_CommandSignature,
            (void**)&device->raySignature);

        if (FAILED(result)) {
            return makeResultD3D12(result);
        }
    }

    if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW) {
        // draw indexed
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
        signatureDesc.ByteStride = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS);

        result = device->handle->lpVtbl->CreateCommandSignature(
            device->handle,
            &signatureDesc,
            nullptr,
            &IID_CommandSignature,
            (void**)&device->drawIndexedSignature);

        if (FAILED(result)) {
            return makeResultD3D12(result);
        }

        // draw
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
        signatureDesc.ByteStride = sizeof(D3D12_DRAW_ARGUMENTS);

        result = device->handle->lpVtbl->CreateCommandSignature(
            device->handle,
            &signatureDesc,
            nullptr,
            &IID_CommandSignature,
            (void**)&device->drawSignature);

        if (FAILED(result)) {
            return makeResultD3D12(result);
        }
    }

    if (features & PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH) {
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
        signatureDesc.ByteStride = sizeof(D3D12_DISPATCH_ARGUMENTS);

        result = device->handle->lpVtbl->CreateCommandSignature(
            device->handle,
            &signatureDesc,
            nullptr,
            &IID_CommandSignature,
            (void**)&device->dispatchSignature);

        if (FAILED(result)) {
            return makeResultD3D12(result);
        }
    }

    // create an internal heap for RTV and DSV
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {0};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.NumDescriptors = MAX_RTV;

    result = device->handle->lpVtbl->CreateDescriptorHeap(
        device->handle,
        &heapDesc,
        &IID_DescriptorHeap,
        (void**)&device->rtvAllocator.heap);

    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heapDesc.NumDescriptors = MAX_DSV;
    result = device->handle->lpVtbl->CreateDescriptorHeap(
        device->handle,
        &heapDesc,
        &IID_DescriptorHeap,
        (void**)&device->dsvAllocator.heap);

    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    for (int i = 0; i < MAX_RTV; i++) {
        device->rtvAllocator.freeList[i] = i + 1;
    }

    for (int i = 0; i < MAX_DSV; i++) {
        device->dsvAllocator.freeList[i] = i + 1;
    }

    device->rtvAllocator.freeTop = 0;
    device->rtvAllocator.incrementSize = device->handle->lpVtbl->GetDescriptorHandleIncrementSize(
        device->handle,
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    device->dsvAllocator.freeTop = 0;
    device->dsvAllocator.incrementSize = device->handle->lpVtbl->GetDescriptorHandleIncrementSize(
        device->handle,
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    // get base CPU pointer
    D3D12_CPU_DESCRIPTOR_HANDLE __ret, dst;
    dst = *device->rtvAllocator.heap->lpVtbl->GetCPUDescriptorHandleForHeapStart(
        device->rtvAllocator.heap,
        &__ret
    );
    device->rtvAllocator.baseOffset = dst.ptr;

    dst = *device->dsvAllocator.heap->lpVtbl->GetCPUDescriptorHandleForHeapStart(
        device->dsvAllocator.heap,
        &__ret
    );
    device->dsvAllocator.baseOffset = dst.ptr;

    // API enforced limits
    limits->freeComputeQueues = 2;
    limits->freeGraphicsQueues = 2;
    limits->freeCopyQueues = 2;
    limits->maxVertexLayouts = 30;
    limits->maxVertexAttributes = 30;

    limits->maxAnisotropy = 16;
    limits->maxPushConstantSize = 256;
    limits->maxTessellationPatchPoint = 32;
    limits->maxBoundDescriptorSets = 30;

    if (features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
        limits->maxDescriptorSampledImages = 4096;
        limits->maxDescriptorStorageImages = 1024;
        limits->maxDescriptorSamplers = 512;
        limits->maxDescriptorStorageBuffers = 2048;
        limits->maxDescriptorUniformBuffers = 256;

    } else {
        limits->maxDescriptorSampledImages = 1024;
        limits->maxDescriptorStorageImages = 512;
        limits->maxDescriptorSamplers = 256;
        limits->maxDescriptorStorageBuffers = 512;
        limits->maxDescriptorUniformBuffers = 256;
        limits->maxBoundDescriptorSets = 30;
    }

    if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        limits->maxRecursionDepth = 31;
        limits->maxHitAttributeSize = 32;
        limits->maxPayloadSize = 64;
        limits->maxDispatchInvocations = 16000000;
        limits->maxDescriptorAccelerationStructures = 4;
    }

    if (features & PAL_ADAPTER_FEATURE_FENCE_RESET) {
        device->canFenceReset = PAL_TRUE;
    }

    device->adapter = d3d12Adapter->handle;
    *outDevice = (PalDevice*)device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDeviceD3D12(PalDevice* device)
{
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    if (d3d12Device->meshSignature) {
        d3d12Device->meshSignature->lpVtbl->Release(d3d12Device->meshSignature);
    }

    if (d3d12Device->raySignature) {
        d3d12Device->raySignature->lpVtbl->Release(d3d12Device->raySignature);
    }

    if (d3d12Device->dispatchSignature) {
        d3d12Device->dispatchSignature->lpVtbl->Release(d3d12Device->dispatchSignature);
    }

    if (d3d12Device->drawIndexedSignature) {
        d3d12Device->drawIndexedSignature->lpVtbl->Release(d3d12Device->drawIndexedSignature);
    }

    if (d3d12Device->drawSignature) {
        d3d12Device->drawSignature->lpVtbl->Release(d3d12Device->drawSignature);
    }

    d3d12Device->rtvAllocator.heap->lpVtbl->Release(d3d12Device->rtvAllocator.heap);
    d3d12Device->dsvAllocator.heap->lpVtbl->Release(d3d12Device->dsvAllocator.heap);

    d3d12Device->queue->lpVtbl->Release(d3d12Device->queue);
    d3d12Device->handle->lpVtbl->Release(d3d12Device->handle);

    if (d3d12Device->infoQueue) {
        d3d12Device->infoQueue->lpVtbl->Release(d3d12Device->infoQueue);
        palFree(s_D3D12.allocator, d3d12Device->scratchBuffer);
    }

    palFree(s_D3D12.allocator, d3d12Device);
}

PalResult PAL_CALL allocateMemoryD3D12(
    PalDevice* device,
    PalMemoryType type,
    uint64_t memoryMask,
    uint64_t size,
    PalMemory** outMemory)
{
    HRESULT result;
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    MemoryD3D12* memory = nullptr;

    memory = palAllocate(s_D3D12.allocator, sizeof(MemoryD3D12), 0);
    if (!memory) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    D3D12_HEAP_DESC desc = {0};
    desc.SizeInBytes = size;
    desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
    if (type == PAL_MEMORY_TYPE_CPU_READBACK) {
        desc.Properties.Type = D3D12_HEAP_TYPE_READBACK;

    } else if (type == PAL_MEMORY_TYPE_CPU_UPLOAD) {
        desc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
    }

    result = d3d12Device->handle->lpVtbl->CreateHeap(
        d3d12Device->handle,
        &desc,
        &IID_Heap,
        (void**)&memory->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        return makeResultD3D12(result);
    }

    memory->type = type;
    *outMemory = (PalMemory*)memory;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL freeMemoryD3D12(PalMemory* memory)
{
    MemoryD3D12* d3d12Memory = (MemoryD3D12*)memory;
    d3d12Memory->handle->lpVtbl->Release(d3d12Memory->handle);
    palFree(s_D3D12.allocator, d3d12Memory);
}

void PAL_CALL querySamplerAnisotropyCapabilitiesD3D12(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps)
{
    caps->maxAnisotropy = 16; // default on most d3d12 hardwares
}

void PAL_CALL queryMultiViewCapabilitiesD3D12(
    PalDevice* device,
    PalMultiViewCapabilities* caps)
{
    caps->maxViewCount = D3D12_MAX_VIEW_INSTANCE_COUNT;
}

void PAL_CALL queryMultiViewportCapabilitiesD3D12(
    PalDevice* device,
    PalMultiViewportCapabilities* caps)
{
    caps->maxCount = D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
}

void PAL_CALL queryDepthStencilCapabilitiesD3D12(
    PalDevice* device,
    PalDepthStencilCapabilities* caps)
{
    // depth resolve modes
    caps->supportedDepthResolveModes |= (1u << PAL_RESOLVE_MODE_AVERAGE);
    caps->supportedDepthResolveModes |= (1u << PAL_RESOLVE_MODE_MIN);
    caps->supportedDepthResolveModes |= (1u << PAL_RESOLVE_MODE_MAX);

    // stencil resolve modes
    caps->supportedStencilResolveModes |= (1u << PAL_RESOLVE_MODE_MIN);
    caps->supportedStencilResolveModes |= (1u << PAL_RESOLVE_MODE_MAX);

    caps->supportsIndependentResolve = PAL_FALSE;
    caps->supportsIndependentResolveNone = PAL_FALSE;
}

void PAL_CALL queryFragmentShadingRateCapabilitiesD3D12(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps)
{
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    caps->supportedShadingRates = (1u << PAL_FRAGMENT_SHADING_RATE_1X1);
    caps->supportedShadingRates |= (1u << PAL_FRAGMENT_SHADING_RATE_1X2);
    caps->supportedShadingRates |= (1u << PAL_FRAGMENT_SHADING_RATE_2X1);
    caps->supportedShadingRates |= (1u << PAL_FRAGMENT_SHADING_RATE_2X2);

    D3D12_FEATURE_DATA_D3D12_OPTIONS6 options = {0};
    d3d12Device->handle->lpVtbl->CheckFeatureSupport(
        d3d12Device->handle,
        D3D12_FEATURE_D3D12_OPTIONS6,
        &options,
        sizeof(options));

    if (options.AdditionalShadingRatesSupported) {
        caps->supportedShadingRates |= (1u << PAL_FRAGMENT_SHADING_RATE_2X4);
        caps->supportedShadingRates |= (1u << PAL_FRAGMENT_SHADING_RATE_4X2);
        caps->supportedShadingRates |= (1u << PAL_FRAGMENT_SHADING_RATE_4X4);
    }

    // there are supported if fragment shading rate feature is
    caps->supportedCombinerOps = 0;
    caps->supportedCombinerOps |= (1u << PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP);
    caps->supportedCombinerOps |= (1u << PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE);
    caps->supportedCombinerOps |= (1u << PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN);
    caps->supportedCombinerOps |= (1u << PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX);
    caps->supportedCombinerOps |= (1u << PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL);

    caps->minTexelWidth = 1; // safe default
    caps->minTexelHeight = 1; // safe default
    caps->maxTexelWidth = options.ShadingRateImageTileSize;
    caps->maxTexelHeight = options.ShadingRateImageTileSize;
}

void PAL_CALL queryMeshShaderCapabilitiesD3D12(
    PalDevice* device,
    PalMeshShaderCapabilities* caps)
{
    // these are not exposed by d3d12. We use the offical mesh shader spec
    caps->maxOutputPrimitives = 256;
    caps->maxOutputVertices = 256;
    caps->maxWorkGroupInvocations = 128;
    caps->maxTaskWorkGroupInvocations = 128;

    caps->maxWorkGroupCount[0] = 65535;
    caps->maxWorkGroupCount[1] = 65535;
    caps->maxWorkGroupCount[2] = 65535;

    caps->maxTaskWorkGroupCount[0] = 65535;
    caps->maxTaskWorkGroupCount[1] = 65535;
    caps->maxTaskWorkGroupCount[2] = 65535;
}

void PAL_CALL queryRayTracingCapabilitiesD3D12(
    PalDevice* device,
    PalRayTracingCapabilities* caps)
{
    // these are safe defaults. D3d12 does not expose them.
    caps->maxRecursionDepth = 31;
    caps->maxHitAttributeSize = 32;
    caps->maxInstanceCount = 1000000;
    caps->maxPrimitiveCount = 10000000;
    caps->maxGeometryCount = 100000;
    caps->maxPayloadSize = 64;
    caps->maxDispatchInvocations = 16000000;
}

void PAL_CALL queryDescriptorIndexingCapabilitiesD3D12(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps)
{
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    caps->flags = PAL_DESCRIPTOR_INDEXING_FLAG_NON_UNIFORM_INDEXING;
    caps->flags |= PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND;
    getDescriptorTierLimitsD3D12(d3d12Device->handle, nullptr, caps);
}

PalResult PAL_CALL createQueueD3D12(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue)
{
    HRESULT result;
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    QueueD3D12* queue = nullptr;
    D3D12_COMMAND_QUEUE_DESC desc = {0};

    switch (type) {
        case PAL_QUEUE_TYPE_COMPUTE: {
            desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
            if (!d3d12Device->limits.freeComputeQueues) {
                return PAL_RESULT_CODE_OUT_OF_MEMORY;
            }
            d3d12Device->limits.freeComputeQueues--;
            break;
        }

        case PAL_QUEUE_TYPE_GRAPHICS: {
            desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            if (!d3d12Device->limits.freeGraphicsQueues) {
                return PAL_RESULT_CODE_OUT_OF_MEMORY;
            }
            d3d12Device->limits.freeGraphicsQueues--;
            break;
        }

        case PAL_QUEUE_TYPE_COPY: {
            desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
            if (!d3d12Device->limits.freeCopyQueues) {
                return PAL_RESULT_CODE_OUT_OF_MEMORY;
            }
            d3d12Device->limits.freeCopyQueues--;
            break;
        }
    }

    queue = palAllocate(s_D3D12.allocator, sizeof(QueueD3D12), 0);
    if (!queue) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    result = d3d12Device->handle->lpVtbl->CreateCommandQueue(
        d3d12Device->handle,
        &desc,
        &IID_Queue,
        (void**)&queue->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        palFree(s_D3D12.allocator, queue);
        return makeResultD3D12(result);
    }

    // create fence used for queue wait
    result = d3d12Device->handle->lpVtbl->CreateFence(
        d3d12Device->handle,
        0,
        0,
        &IID_Fence,
        (void**)&queue->fence);

    if (FAILED(result)) {
        palFree(s_D3D12.allocator, queue);
        return makeResultD3D12(result);
    }

    // create event
    queue->fenceEvent = CreateEvent(nullptr, PAL_FALSE, PAL_FALSE, nullptr);
    if (!queue->fenceEvent) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE,
            PAL_RESULT_SOURCE_WIN32, 
            GetLastError());
    }

    queue->fenceValue = 0;
    queue->type = type;
    *outQueue = (PalQueue*)queue;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyQueueD3D12(PalQueue* queue)
{
    QueueD3D12* d3d12Queue = (QueueD3D12*)queue;
    d3d12Queue->fence->lpVtbl->Release(d3d12Queue->fence);
    d3d12Queue->handle->lpVtbl->Release(d3d12Queue->handle);
    CloseHandle(d3d12Queue->fenceEvent);
    palFree(s_D3D12.allocator, d3d12Queue);
}

PalResult PAL_CALL waitQueueD3D12(PalQueue* queue)
{
    QueueD3D12* d3d12Queue = (QueueD3D12*)queue;
    ID3D12Fence* fence = d3d12Queue->fence;
    HANDLE event = d3d12Queue->fenceEvent;

    // wait on the fence if the submited work is not done
    if (fence->lpVtbl->GetCompletedValue(fence) < d3d12Queue->fenceValue) {
        fence->lpVtbl->SetEventOnCompletion(fence, d3d12Queue->fenceValue, event);
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }
    return PAL_RESULT_SUCCESS;
}

PalBool PAL_CALL canQueuePresentD3D12(
    PalQueue* queue,
    PalSurface* surface)
{
    QueueD3D12* d3d12Queue = (QueueD3D12*)queue;
    if (d3d12Queue->type == PAL_QUEUE_TYPE_GRAPHICS) {
        return PAL_TRUE; // all graphics queues support presentation
    }
    return PAL_FALSE;
}

PalResult PAL_CALL createShaderD3D12(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader)
{
    ShaderD3D12* shader = nullptr;
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    void* bytecode = nullptr;

    shader = palAllocate(s_D3D12.allocator, sizeof(ShaderD3D12), 0);
    bytecode = palAllocate(s_D3D12.allocator, info->bytecodeSize, 0);
    if (!shader || !bytecode) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // allocate entries array
    shader->entries = palAllocate(s_D3D12.allocator, sizeof(ShaderEntry) * info->entryCount, 0);
    if (!shader->entries) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    for (int i = 0; i < info->entryCount; i++) {
        ShaderEntry* entry = &shader->entries[i];
        convertToWcharD3D12(info->entries[i].entryName, entry->entryName);
        entry->patchControlPoints = info->entries[i].patchControlPoints;
        entry->stage = info->entries[i].stage; 
    }

    memcpy(bytecode, info->bytecode, info->bytecodeSize);
    shader->byteCode.pShaderBytecode = bytecode;
    shader->byteCode.BytecodeLength = info->bytecodeSize;

    shader->entryCount = info->entryCount;
    *outShader = (PalShader*)shader;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyShaderD3D12(PalShader* shader)
{
    ShaderD3D12* d3dShader = (ShaderD3D12*)shader;
    palFree(s_D3D12.allocator, (void*)d3dShader->byteCode.pShaderBytecode);
    palFree(s_D3D12.allocator, d3dShader->entries);
    palFree(s_D3D12.allocator, d3dShader);
}

#endif // PAL_HAS_D3D12_BACKEND