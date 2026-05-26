
#include "pal/pal_graphics.h"
#include "tests.h"

#define BUFFER_SIZE 400

// layout must match shader
typedef struct {
    Uint32 width;
    Uint32 height;
    Uint32 _padding[2];
    float set1Color[4];
    float set2Color[4];
    float set3Color[4];
} PushConstant;

static void PAL_CALL onGraphicsDebug(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg)
{
    palLog(nullptr, msg);
}

bool multiDescriptorSetTest()
{
    PalAdapter* adapter = nullptr;
    PalDevice* device = nullptr;
    PalQueue* queue = nullptr;
    PalCommandPool* cmdPool = nullptr;
    PalCommandBuffer* cmdBuffer;
    PalShader* shader = nullptr;

    PalBuffer* buffers[3];
    PalBuffer* stagingBuffers[3];
    PalMemory* bufferMemories[3];
    PalMemory* stagingBufferMemories[3];

    PalDescriptorPool* descriptorPool = nullptr;
    PalDescriptorSetLayout* descriptorSetLayout;
    PalDescriptorSet* descriptorSets[3];

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
            adapter = nullptr;
            continue;
        }

        // we want an adapter that supports the required bound descriptor sets (3)
        if (caps.resourceCaps.maxBoundSets < 3) {
            adapter = nullptr;
            continue;
        }

        // we want an adapter that supports the required push constant size (64 bytes)
        if (caps.maxPushConstantSize < 64) {
            adapter = nullptr;
            continue;
        }

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

        adapter = nullptr;
        continue;
    }

    palFree(nullptr, adapters);
    if (!adapter) {
        palLog(nullptr, "Failed to find a required adapter");
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
        source = "graphics/shaders/bin/spirv/compute_multi.spv";

    } else if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
        source = "graphics/shaders/bin/dxil/compute_multi.dxil";
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

    PalShaderEntryInfo computeEntry = {0};
    computeEntry.entryName = "main";
    computeEntry.stage = PAL_SHADER_STAGE_COMPUTE;

    shaderCreateInfo.bytecode = bytecode;
    shaderCreateInfo.bytecodeSize = bytecodeSize;
    shaderCreateInfo.entries = &computeEntry;
    shaderCreateInfo.entryCount = 1;

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

    for (int i = 0; i < 3; i++) {
        bufferCreateInfo.usages = PAL_BUFFER_USAGE_STORAGE | PAL_BUFFER_USAGE_TRANSFER_SRC;

        result = palCreateBuffer(device, &bufferCreateInfo, &buffers[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create buffer: %s", error);
            return false;
        }

        bufferCreateInfo.usages = PAL_BUFFER_USAGE_TRANSFER_DST;
        result = palCreateBuffer(device, &bufferCreateInfo, &stagingBuffers[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create buffer: %s", error);
            return false;
        }

        // get buffer memory requirement and allocate memory
        PalMemoryRequirements memReq = {0};
        PalMemoryRequirements stagingMemReq = {0};

        result = palGetBufferMemoryRequirements(buffers[i], &memReq);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
            return false;
        }

        result = palGetBufferMemoryRequirements(stagingBuffers[i], &stagingMemReq);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
            return false;
        }

        result = palAllocateMemory(
            device,
            PAL_MEMORY_TYPE_GPU_ONLY,
            memReq.memoryMask,
            memReq.size,
            &bufferMemories[i]);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
            return false;
        }

        result = palAllocateMemory(
            device,
            PAL_MEMORY_TYPE_CPU_READBACK,
            stagingMemReq.memoryMask,
            stagingMemReq.size,
            &stagingBufferMemories[i]);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
            return false;
        }

        // bind memory
        result = palBindBufferMemory(buffers[i], bufferMemories[i], 0);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to bind memory: %s", error);
            return false;
        }

        result = palBindBufferMemory(stagingBuffers[i], stagingBufferMemories[i], 0);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to bind memory: %s", error);
            return false;
        }
    }

    // create descriptor set layout
    PalDescriptorSetLayoutBinding descriptorBinding = {0};
    descriptorBinding.descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorBinding.descriptorCount = 1; // not an array

    PalDescriptorSetLayoutCreateInfo descriptorSetLayoutcreateInfo = {0};
    descriptorSetLayoutcreateInfo.bindingCount = 1;
    descriptorSetLayoutcreateInfo.bindings = &descriptorBinding;

    PalShaderStage shaderStages[] = { PAL_SHADER_STAGE_COMPUTE };
    descriptorSetLayoutcreateInfo.shaderStageCount = 1;
    descriptorSetLayoutcreateInfo.shaderStages = shaderStages;

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
    storageBufferBindingsize.bindingCount = 3; // across all sets
    storageBufferBindingsize.descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    PalDescriptorPoolCreateInfo descriptorPoolCreateInfo = {0};
    descriptorPoolCreateInfo.maxDescriptorSets = 3;
    descriptorPoolCreateInfo.maxDescriptorBindingSizes = 1; // one binding type
    descriptorPoolCreateInfo.bindingSizes = &storageBufferBindingsize;

    result = palCreateDescriptorPool(device, &descriptorPoolCreateInfo, &descriptorPool);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create descriptor pool: %s", error);
        return false;
    }

    // allocate the sets from the descriptor pool
    for (int i = 0; i < 3; i++) {
        result = palAllocateDescriptorSet(
            device, 
            descriptorPool, 
            descriptorSetLayout, 
            &descriptorSets[i]);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to allocate descriptor set: %s", error);
            return false;
        }
    }

    // write the inital data to the descriptor set since its created empty
    PalDescriptorBufferInfo descriptorBufferInfos[3];
    for (int i = 0; i < 3; i++) {
        descriptorBufferInfos[i].buffer = buffers[i];
        descriptorBufferInfos[i].offset = 0;
        descriptorBufferInfos[i].size = bufferBytes;
        descriptorBufferInfos[i].stride = 16; // sizeof(vec4) or float4.
    }

    PalDescriptorSetWriteInfo writeInfos[3];    
    for (int i = 0; i < 3; i++) {
        writeInfos[i].layoutBindingIndex = 0; // single descriptor binding
        writeInfos[i].bufferInfos = &descriptorBufferInfos[i];
        writeInfos[i].descriptorSet = descriptorSets[i];
        writeInfos[i].descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        writeInfos[i].descriptorCount = 1;

        writeInfos[i].arrayElement = 0;
        writeInfos[i].imageViewInfos = nullptr;
        writeInfos[i].samplerInfos = nullptr;
        writeInfos[i].tlasInfos = nullptr;
    }

    result = palUpdateDescriptorSet(device, 3, writeInfos);
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
    // even if the descriptor set layout is identical, pipeline layout creation needs
    // a descriptor set layout per set (3)
    PalDescriptorSetLayout* descriptorSetLayouts[3];
    descriptorSetLayouts[0] = descriptorSetLayout;
    descriptorSetLayouts[1] = descriptorSetLayout;
    descriptorSetLayouts[2] = descriptorSetLayout;

    PalPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {0};
    pipelineLayoutCreateInfo.descriptorSetLayoutCount = 3;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.descriptorSetLayouts = descriptorSetLayouts;
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

    // set 1 data (red)
    pushConstant.set1Color[0] = 1.0f;
    pushConstant.set1Color[1] = 0.0f;
    pushConstant.set1Color[2] = 0.0f;
    pushConstant.set1Color[3] = 1.0f;

    // set 2 data(green)
    pushConstant.set2Color[0] = 0.0f;
    pushConstant.set2Color[1] = 1.0f;
    pushConstant.set2Color[2] = 0.0f;
    pushConstant.set2Color[3] = 1.0f;

    // set 3 data (blue)
    pushConstant.set3Color[0] = 0.0f;
    pushConstant.set3Color[1] = 0.0f;
    pushConstant.set3Color[2] = 1.0f;
    pushConstant.set3Color[3] = 1.0f;

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

    // bind descriptor sets
    for (int i = 0; i < 3; i++) {
        result = palCmdBindDescriptorSet(cmdBuffer, i, descriptorSets[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to bind descriptor set: %s", error);
            return false;
        }
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
    buildData.workGroupCount[0] = caps.computeCaps.maxWorkGroupCount[0];
    buildData.workGroupCount[1] = caps.computeCaps.maxWorkGroupCount[1];
    buildData.workGroupCount[2] = caps.computeCaps.maxWorkGroupCount[2];

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

    // set a barrier so we only read from the buffer after the shader has written to it
    PalUsageStateInfo oldUsageStateInfo = {0};
    oldUsageStateInfo.usageState = PAL_USAGE_STATE_SHADER_WRITE;
    oldUsageStateInfo.shaderStageCount = 1;
    oldUsageStateInfo.shaderStages = shaderStages;

    PalUsageStateInfo newUsageStateInfo = {0};
    newUsageStateInfo.shaderStageCount = 0;
    newUsageStateInfo.shaderStages = nullptr;
    newUsageStateInfo.usageState = PAL_USAGE_STATE_TRANSFER_READ;

    for (int i = 0; i < 3; i++) {
        result = palCmdBufferBarrier(
            cmdBuffer, 
            buffers[i], 
            &oldUsageStateInfo, 
            &newUsageStateInfo);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set buffer barrier: %s", error);
            return false;
        }

        // now we copy from the GPU buffer into the staging buffer
        PalBufferCopyInfo copyInfo = {0};
        copyInfo.size = bufferBytes;

        result = palCmdCopyBuffer(cmdBuffer, stagingBuffers[i], buffers[i], &copyInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to copy buffer: %s", error);
            return false;
        }
    }

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

    const char* names[3];
    names[0] = "set1_compute_output.ppm";
    names[1] = "set2_compute_output.ppm";
    names[2] = "set3_compute_output.ppm";

    for (int i = 0; i < 3; i++) {
        // now our staging buffer has the contents of the GPU buffer
        // we map it and copy the contents to a ppm buffer and save it
        void* ptr = nullptr;
        result = palMapBufferMemory(stagingBuffers[i], 0, bufferBytes, &ptr);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to map buffer memory: %s", error);
            return false;
        }

        // write to a ppm output file
        FILE* file = fopen(names[i], "wb");
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
        palUnmapBufferMemory(stagingBuffers[i]);
    }

    palDestroyPipeline(pipeline);
    palDestroyPipelineLayout(pipelineLayout);
    palFreeCommandBuffer(cmdBuffer);
    palDestroyCommandPool(cmdPool);

    palDestroyDescriptorPool(descriptorPool);
    palDestroyDescriptorSetLayout(descriptorSetLayout);

    for (int i = 0; i < 3; i++) {
        palDestroyBuffer(buffers[i]);
        palFreeMemory(device, bufferMemories[i]);

        palDestroyBuffer(stagingBuffers[i]);
        palFreeMemory(device, stagingBufferMemories[i]);
    }

    palDestroyFence(fence);
    palDestroyQueue(queue);
    palDestroyDevice(device);
    palShutdownGraphics();
    return true;
}
