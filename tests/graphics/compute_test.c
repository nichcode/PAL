
#include "pal/pal_graphics.h"
#include "tests.h"

#define BUFFER_SIZE 400

// layout must match shader
typedef struct {
    Uint32 width;
    Uint32 height;
    Uint32 _padding[2];
    float color[4];
} PushConstant;

static void PAL_CALL onGraphicsDebug(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg)
{
    palLog(nullptr, msg);
}

bool computeTest()
{
    PalAdapter* adapter = nullptr;
    PalDevice* device = nullptr;
    PalQueue* queue = nullptr;
    PalCommandPool* cmdPool = nullptr;
    PalCommandBuffer* cmdBuffer;
    PalShader* shader = nullptr;
    PalBuffer* buffer = nullptr;
    PalBuffer* stagingBuffer = nullptr;
    PalMemory* bufferMemory = nullptr;
    PalMemory* stagingBufferMemory = nullptr;

    PalDescriptorSetLayout* descriptorSetLayout = nullptr;
    PalDescriptorPool* descriptorPool = nullptr;
    PalDescriptorSet* descriptorSet = nullptr;
    PalPipelineLayout* pipelineLayout = nullptr;
    PalPipeline* pipeline = nullptr;
    PalFence* fence = nullptr;

    PalGraphicsDebugger debugger = {0};
    debugger.callback = onGraphicsDebug;
    debugger.userData = nullptr;

    PalResult result = palInitGraphics(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize graphics: %s", error);
        return false;
    }

    // enumerate all available adapters
    Int32 adapterCount = 0;
    result = palEnumerateAdapters(&adapterCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get query adapters: %s", error);
        return false;
    }

    if (adapterCount == 0) {
        palLog(nullptr, "No adapters found");
        return false;
    }
    palLog(nullptr, "Adapter count: %d", adapterCount);

    PalAdapter** adapters = nullptr;
    adapters = palAllocate(nullptr, sizeof(PalAdapter*) * adapterCount, 0);
    if (!adapters) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    result = palEnumerateAdapters(&adapterCount, adapters);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get query adapters: %s", error);
        return false;
    }

    PalAdapterCapabilities caps = {0};
    PalAdapterFeatures adapterFeatures = 0;
    PalAdapterInfo adapterInfo = {0};
    bool hasComputeQueue = false;
    for (Int32 i = 0; i < adapterCount; i++) {
        adapter = adapters[i];
        result = palGetAdapterCapabilities(adapter, &caps);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter capabilities: %s", error);
            palFree(nullptr, adapters);
            return false;
        }

        if (caps.maxComputeQueues == 0) {
            hasComputeQueue = false;
            continue;

        } else {
            hasComputeQueue = true;
        }

        if (hasComputeQueue) {
            // We want an adapter that supports spirv 1.0 or dxil 6.0
            result = palGetAdapterInfo(adapter, &adapterInfo);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to get adapter info: %s", error);
                return false;
            }

            // we prefer spirv first if an adapter supports multiple shader formats
            Uint32 target = 0;
            if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
                target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_SPIRV);
                if (target >= PAL_MAKE_SHADER_TARGET(1, 0)) {
                    break;
                }
            }

            if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
                target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_DXIL);
                if (target >= PAL_MAKE_SHADER_TARGET(6, 0)) {
                    break;
                }
            }
        }
        adapter = nullptr;
    }

    palFree(nullptr, adapters);
    if (!adapter) {
        if (!hasComputeQueue) {
            palLog(nullptr, "Failed to find an adapter that supports compute queue");

        } else {
            palLog(nullptr, "Failed to find an adapter that supports required shader target");
        }
        return false;
    }

    // create a device
    result = palCreateDevice(adapter, 0, &device);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create device: %s", error);
        return false;
    }

    // create a compute command queue
    result = palCreateQueue(device, PAL_QUEUE_TYPE_COMPUTE, &queue);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create queue: %s", error);
        return false;
    }

    result = palCreateCommandPool(device, queue, &cmdPool);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create command pool: %s", error);
        return false;
    }

    result = palAllocateCommandBuffer(
        device,
        cmdPool,
        PAL_COMMAND_BUFFER_TYPE_PRIMARY,
        &cmdBuffer);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate command buffer: %s", error);
        return false;
    }

    // create a compute shader
    Uint64 bytecodeSize = 0;
    void* bytecode = nullptr;
    const char* source = nullptr;

    PalShaderCreateInfo shaderCreateInfo = {0};
    if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
        source = "graphics/shaders/bin/spirv/compute.spv";

    } else if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
        source = "graphics/shaders/bin/dxil/compute.dxil";
    }

    // read file
    if (!readFile(source, nullptr, &bytecodeSize)) {
        palLog(nullptr, "Failed to read shader file");
        return false;
    }

    bytecode = palAllocate(nullptr, bytecodeSize, 0);
    if (!bytecode) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    readFile(source, bytecode, &bytecodeSize);

    shaderCreateInfo.bytecode = bytecode;
    shaderCreateInfo.bytecodeSize = bytecodeSize;
    shaderCreateInfo.entryName = "main";
    shaderCreateInfo.stage = PAL_SHADER_STAGE_COMPUTE;

    result = palCreateShader(device, &shaderCreateInfo, &shader);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create compute shader: %s", error);
        return false;
    }

    palFree(nullptr, bytecode);

    // create a storage buffer
    Uint32 bufferBytes = BUFFER_SIZE * BUFFER_SIZE * sizeof(float) * 4; // must match shader
    PalBufferCreateInfo bufferCreateInfo = {0};
    bufferCreateInfo.size = bufferBytes;
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_STORAGE | PAL_BUFFER_USAGE_TRANSFER_SRC;

    result = palCreateBuffer(device, &bufferCreateInfo, &buffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create buffer: %s", error);
        return false;
    }

    bufferCreateInfo.size = bufferBytes;
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_TRANSFER_DST;

    result = palCreateBuffer(device, &bufferCreateInfo, &stagingBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create staging buffer: %s", error);
        return false;
    }

    // get buffer memory requirement and allocate memory
    PalMemoryRequirements bufferMemReq = {0};
    PalMemoryRequirements stagingBufferMemReq = {0};
    result = palGetBufferMemoryRequirements(buffer, &bufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
        return false;
    }

    result = palGetBufferMemoryRequirements(stagingBuffer, &stagingBufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
        return false;
    }

    // we need to check if the memory type we want are supported
    // but almost every GPU supports a GPU only memory
    // and CPU writable memory
    result = palAllocateMemory(
        device,
        PAL_MEMORY_TYPE_GPU_ONLY,
        bufferMemReq.memoryMask,
        bufferMemReq.size,
        &bufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
        return false;
    }

    result = palAllocateMemory(
        device,
        PAL_MEMORY_TYPE_CPU_READBACK,
        stagingBufferMemReq.memoryMask,
        stagingBufferMemReq.size,
        &stagingBufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
        return false;
    }

    // bind memory
    result = palBindBufferMemory(buffer, bufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind memory: %s", error);
        return false;
    }

    result = palBindBufferMemory(stagingBuffer, stagingBufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind memory: %s", error);
        return false;
    }

    // create descriptor set layout
    PalDescriptorSetLayoutBinding descriptorBinding = {0};
    PalShaderStage shaderStages[] = { PAL_SHADER_STAGE_COMPUTE };

    descriptorBinding.descriptorCount = 1; // not an array
    descriptorBinding.descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorBinding.shaderStageCount = 1;
    descriptorBinding.shaderStages = shaderStages;

    PalDescriptorSetLayoutCreateInfo descriptorSetLayoutcreateInfo = {0};
    descriptorSetLayoutcreateInfo.bindingCount = 1;
    descriptorSetLayoutcreateInfo.bindings = &descriptorBinding;

    result = palCreateDescriptorSetLayout(
        device,
        &descriptorSetLayoutcreateInfo,
        &descriptorSetLayout);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create descriptor set layout: %s", error);
        return false;
    }

    // create descriptor pool
    PalDescriptorPoolBindingSize storageBufferBindingsize = {0};
    storageBufferBindingsize.bindingCount = 1;
    storageBufferBindingsize.descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    PalDescriptorPoolCreateInfo descriptorPoolCreateInfo = {0};
    descriptorPoolCreateInfo.maxDescriptorSets = 1; // only one set
    descriptorPoolCreateInfo.maxDescriptorBindingSizes = 1; // one binding type
    descriptorPoolCreateInfo.bindingSizes = &storageBufferBindingsize;

    result = palCreateDescriptorPool(device, &descriptorPoolCreateInfo, &descriptorPool);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create descriptor pool: %s", error);
        return false;
    }

    // allocate a single descriptor set from the descriptor pool
    // using the layout we created above
    result = palAllocateDescriptorSet(device, descriptorPool, descriptorSetLayout, &descriptorSet);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate descriptor set: %s", error);
        return false;
    }

    // write the inital data to the descriptor set since its created empty
    PalDescriptorBufferInfo descriptorBufferInfo = {0};
    descriptorBufferInfo.buffer = buffer;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.size = bufferBytes;

    PalDescriptorSetWriteInfo writeInfo = {0};
    writeInfo.layoutBindingIndex = 0;
    writeInfo.bufferInfos = &descriptorBufferInfo;
    writeInfo.descriptorSet = descriptorSet;
    writeInfo.descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeInfo.descriptorCount = 1;

    result = palUpdateDescriptorSet(device, 1, &writeInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to update descriptor set: %s", error);
        return false;
    }

    // push constants
    // size must be less than the max size from the adapter capabilities struct
    PalPushConstantRange pushConstantRange = {0};
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstant); // must match shader
    pushConstantRange.shaderStageCount = 1;
    pushConstantRange.shaderStages = shaderStages;

    // create pipeline layout
    PalPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {0};
    pipelineLayoutCreateInfo.descriptorSetLayoutCount = 1;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.descriptorSetLayouts = &descriptorSetLayout;
    pipelineLayoutCreateInfo.pushConstantRanges = &pushConstantRange;

    result = palCreatePipelineLayout(device, &pipelineLayoutCreateInfo, &pipelineLayout);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create pipeline layout: %s", error);
        return false;
    }

    // create a compute pipeline
    PalComputePipelineCreateInfo pipelineCreateInfo = {0};
    pipelineCreateInfo.computeShader = shader;
    pipelineCreateInfo.pipelineLayout = pipelineLayout;

    result = palCreateComputePipeline(device, &pipelineCreateInfo, &pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create compute pipeline: %s", error);
        return false;
    }

    palDestroyShader(shader);

    // create fence
    result = palCreateFence(device, false, &fence);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create fence: %s", error);
        return false;
    }

    // record commands
    result = palCmdBegin(cmdBuffer, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to begin command buffer: %s", error);
        return false;
    }

    PushConstant pushConstant = {0};
    pushConstant.width = BUFFER_SIZE;
    pushConstant.height = BUFFER_SIZE;
    pushConstant.color[0] = 1.0f;
    pushConstant.color[1] = 0.0f;
    pushConstant.color[2] = 0.0f;
    pushConstant.color[3] = 1.0f;

    result = palCmdBindPipeline(cmdBuffer, pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind pipeline: %s", error);
        return false;
    }

    result = palCmdPushConstants(
        cmdBuffer,
        1,
        shaderStages,
        0,
        sizeof(PushConstant),
        &pushConstant);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to push constants: %s", error);
        return false;
    }

    result = palCmdBindDescriptorSet(cmdBuffer, 0, descriptorSet);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind descriptor set: %s", error);
        return false;
    }

    // we will use a helper function to calculate the number of group count
    // we need on each axis. The workGroupSize on each axis must be less or equal
    // to maxComputeWorkGroupInvocations from the adapter capabilities struct
    PalWorkGroupBuildData buildData = {0};
    buildData.workCount[0] = BUFFER_SIZE;
    buildData.workCount[1] = BUFFER_SIZE;
    buildData.workCount[2] = 1; // no Z

    buildData.workGroupSize[0] = 16; // must match shader (local_size on glsl)
    buildData.workGroupSize[1] = 16; // must match shader (local_size on glsl)
    buildData.workGroupSize[2] = 1; // must match shader (local_size on glsl)

    // device limits
    buildData.workGroupCount[0] = caps.maxComputeWorkGroupCount[0];
    buildData.workGroupCount[1] = caps.maxComputeWorkGroupCount[1];
    buildData.workGroupCount[2] = caps.maxComputeWorkGroupCount[2];

    Uint32 workGroupInfoCount = 0;
    PalWorkGroupInfo* workGroupInfos = nullptr;
    bool ret = palBuildWorkGroupInfo(&buildData, &workGroupInfoCount, nullptr);
    if (!ret) {
        palLog(nullptr, "Failed to build work group info");
        return false;
    }

    workGroupInfos = palAllocate(nullptr, sizeof(PalWorkGroupInfo) * workGroupInfoCount, 0);
    if (!workGroupInfos) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    palBuildWorkGroupInfo(&buildData, &workGroupInfoCount, workGroupInfos);

    // set a barrier on the buffer
    PalUsageStateInfo oldUsageStateInfo = {0};
    PalUsageStateInfo newUsageStateInfo = {0};
    newUsageStateInfo.shaderStageCount = 1;
    newUsageStateInfo.shaderStages = shaderStages;
    newUsageStateInfo.usageState = PAL_USAGE_STATE_SHADER_WRITE;

    result = palCmdBufferBarrier(cmdBuffer, buffer, &oldUsageStateInfo, &newUsageStateInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set buffer barrier: %s", error);
        return false;
    }

    // dispatch with the work group info
    for (int i = 0; i < workGroupInfoCount; i++) {
        // palDispatchBase is not supported on all platforms so we dont use it for this example
        // We cap the buffer size small so we only get a single dispatch
        // but you can add fields in yout constants to send the base to the shader driectly
        Uint32 groupCountX = workGroupInfos[i].workGroupCount[0];
        Uint32 groupCountY = workGroupInfos[i].workGroupCount[1];
        Uint32 groupCountZ = workGroupInfos[i].workGroupCount[2];

        result = palCmdDispatch(cmdBuffer, groupCountX, groupCountY, groupCountZ);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to dispatch: %s", error);
            return false;
        }
    }
    palFree(nullptr, workGroupInfos);

    // set a barrier so we only read from the buffer after the shader has
    // written to it
    oldUsageStateInfo = newUsageStateInfo;
    newUsageStateInfo.shaderStageCount = 0;
    newUsageStateInfo.shaderStages = nullptr;
    newUsageStateInfo.usageState = PAL_USAGE_STATE_TRANSFER_READ;

    result = palCmdBufferBarrier(cmdBuffer, buffer, &oldUsageStateInfo, &newUsageStateInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set buffer barrier: %s", error);
        return false;
    }

    // set a barrier on the staging buffer
    oldUsageStateInfo.usageState = PAL_USAGE_STATE_UNDEFINED;
    oldUsageStateInfo.shaderStageCount = 0;
    oldUsageStateInfo.shaderStages = nullptr;
    newUsageStateInfo.usageState = PAL_USAGE_STATE_TRANSFER_WRITE;

    result = palCmdBufferBarrier(cmdBuffer, stagingBuffer, &oldUsageStateInfo, &newUsageStateInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set buffer barrier: %s", error);
        return false;
    }

    // now we copy from the GPU buffer into the staging buffer
    PalBufferCopyInfo copyInfo = {0};
    copyInfo.size = bufferBytes;

    result = palCmdCopyBuffer(cmdBuffer, stagingBuffer, buffer, &copyInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to copy buffer: %s", error);
        return false;
    }

    // if we want to copy to the ppm buffer in the command buffer
    // we need a barrier to with the state PAL_USAGE_STATE_HOST_READ
    // but we map and copy outside the command buffer since
    // we wait for a fence (the command buffer has been executed).

    result = palCmdEnd(cmdBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to end command buffer: %s", error);
        return false;
    }

    // submit the command buffer to the GPU
    PalCommandBufferSubmitInfo submitInfo = {0};
    submitInfo.cmdBuffer = cmdBuffer;
    submitInfo.fence = fence;
    result = palSubmitCommandBuffer(queue, &submitInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to submit command buffer: %s", error);
        return false;
    }

    // wait for the fence
    result = palWaitFence(fence, PAL_INFINITE);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to wait for fence: %s", error);
        return false;
    }

    // now our staging buffer has the contents of the GPU buffer
    // we map it and copy the contents to a ppm buffer and save it
    void* ptr = nullptr;
    result = palMapBufferMemory(stagingBuffer, 0, bufferBytes, &ptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to map buffer memory: %s", error);
        return false;
    }

    // write to a ppm output file
    FILE* file = fopen("compute_output.ppm", "wb");
    fprintf(file, "P6\n%d %d\n255\n", BUFFER_SIZE, BUFFER_SIZE);
    float* pixels = (float*)ptr;
    for (int y = 0; y < BUFFER_SIZE; y++) {
        int row = BUFFER_SIZE - 1 - y; // flip y
        for (int x = 0; x < BUFFER_SIZE; x++) {
            int index = row * BUFFER_SIZE + x;
            Uint8 rgb[3];

            rgb[0] = pixels[index * 4 + 0] > 0.5f ? 255: 0;
            rgb[1] = pixels[index * 4 + 1] > 0.5f ? 255: 0;
            rgb[2] = pixels[index * 4 + 2] > 0.5f ? 255: 0;
            fwrite(rgb, 1, 3, file);
        }
    }

    fclose(file);
    palUnmapBufferMemory(stagingBuffer);

    palDestroyPipeline(pipeline);
    palDestroyPipelineLayout(pipelineLayout);
    palFreeCommandBuffer(cmdBuffer);
    palDestroyCommandPool(cmdPool);

    palDestroyDescriptorPool(descriptorPool);
    palDestroyDescriptorSetLayout(descriptorSetLayout);

    palDestroyBuffer(buffer);
    palDestroyBuffer(stagingBuffer);
    palFreeMemory(device, bufferMemory);
    palFreeMemory(device, stagingBufferMemory);

    palDestroyFence(fence);
    palDestroyQueue(queue);
    palDestroyDevice(device);
    palShutdownGraphics();
    return true;
}
