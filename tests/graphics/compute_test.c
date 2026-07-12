
#include "pal/pal_graphics.h"
#include "tests.h"

#define BUFFER_SIZE 400

// layout must match shader
typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t _padding[2];
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

PalBool computeTest()
{
    PalAdapter* adapter = nullptr;
    PalDevice* device = nullptr;
    PalQueue* queue = nullptr;
    PalCommandPool* cmdPool = nullptr;
    PalCommandBuffer* cmdBuffer;
    PalShader* shader = nullptr;
    PalBuffer* buffer = nullptr;
    PalBuffer* stagingBuffer = nullptr;

    PalDescriptorSetLayout* descriptorSetLayout = nullptr;
    PalDescriptorPool* descriptorPool = nullptr;
    PalDescriptorSet* descriptorSet = nullptr;
    PalPipelineLayout* pipelineLayout = nullptr;
    PalPipeline* pipeline = nullptr;
    PalFence* fence = nullptr;

    PalGraphicsDebugger debugger = {0};
    debugger.callback = onGraphicsDebug;
    debugger.userData = nullptr;

    PalResult result = palInitGraphics(nullptr, nullptr, 0, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize graphics");
        return PAL_FALSE;
    }

    // enumerate all available adapters
    int32_t adapterCount = 0;
    result = palEnumerateAdapters(&adapterCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get adapters");
        return PAL_FALSE;
    }

    if (adapterCount == 0) {
        palLog(nullptr, "No adapters found");
        return PAL_FALSE;
    }
    palLog(nullptr, "Adapter count: %d", adapterCount);

    PalAdapter** adapters = nullptr;
    adapters = palAllocate(nullptr, sizeof(PalAdapter*) * adapterCount, 0);
    if (!adapters) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
    }

    result = palEnumerateAdapters(&adapterCount, adapters);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get adapters");
        return PAL_FALSE;
    }

    PalAdapterCapabilities caps = {0};
    PalAdapterFeatures adapterFeatures = 0;
    PalAdapterInfo adapterInfo = {0};
    for (int32_t i = 0; i < adapterCount; i++) {
        adapter = adapters[i];
        palGetAdapterCapabilities(adapter, &caps);
        if (caps.maxComputeQueues == 0) {
            adapter = nullptr;
            continue;
        }

        // We want an adapter that supports spirv 1.0 or dxil 6.0
        palGetAdapterInfo(adapter, &adapterInfo);

        // we prefer spirv first if an adapter supports multiple shader formats
        uint32_t target = 0;
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
        return PAL_FALSE;
    }

    // create a device
    result = palCreateDevice(adapter, 0, &device);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create device");
        return PAL_FALSE;
    }

    // create a compute command queue
    result = palCreateQueue(device, PAL_QUEUE_TYPE_COMPUTE, &queue);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create queue");
        return PAL_FALSE;
    }

    result = palCreateCommandPool(device, queue, &cmdPool);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create command pool");
        return PAL_FALSE;
    }

    result = palAllocateCommandBuffer(
        device,
        cmdPool,
        PAL_COMMAND_BUFFER_TYPE_PRIMARY,
        &cmdBuffer);

    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to allocate command buffer");
        return PAL_FALSE;
    }

    // create a compute shader
    uint64_t bytecodeSize = 0;
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
        return PAL_FALSE;
    }

    bytecode = palAllocate(nullptr, bytecodeSize, 0);
    if (!bytecode) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
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
        logResult(result, "Failed to create shader");
        return PAL_FALSE;
    }

    palFree(nullptr, bytecode);

    // create a storage buffer
    uint32_t bufferBytes = BUFFER_SIZE * BUFFER_SIZE * sizeof(float) * 4; // must match shader
    PalBufferCreateInfo bufferCreateInfo = {0};
    bufferCreateInfo.size = bufferBytes;
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_STORAGE | PAL_BUFFER_USAGE_TRANSFER_SRC;
    bufferCreateInfo.memoryUsage = PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY;

    result = palCreateBuffer(device, &bufferCreateInfo, &buffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create buffer");
        return PAL_FALSE;
    }

    bufferCreateInfo.usages = PAL_BUFFER_USAGE_TRANSFER_DST;
    bufferCreateInfo.memoryUsage = PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_READBACK;
    result = palCreateBuffer(device, &bufferCreateInfo, &stagingBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create buffer");
        return PAL_FALSE;
    }

    // create descriptor set layout
    PalDescriptorSetLayoutBinding descriptorBinding = {0};
    descriptorBinding.descriptorCount = 1; // not an array
    descriptorBinding.descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    PalDescriptorSetLayoutCreateInfo descriptorSetLayoutcreateInfo = {0};
    descriptorSetLayoutcreateInfo.bindingCount = 1;
    descriptorSetLayoutcreateInfo.bindings = &descriptorBinding;

    result = palCreateDescriptorSetLayout(
        device,
        &descriptorSetLayoutcreateInfo,
        &descriptorSetLayout);

    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create descriptor set layout");
        return PAL_FALSE;
    }

    // create descriptor pool
    PalDescriptorPoolBindingSize storageBufferBindingsize = {0};
    storageBufferBindingsize.bindingCount = 1;
    storageBufferBindingsize.descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    PalDescriptorPoolCreateInfo descriptorPoolCreateInfo = {0};
    descriptorPoolCreateInfo.maxDescriptorSets = 1; // only one set
    descriptorPoolCreateInfo.bindingSizeCount = 1; // one binding type
    descriptorPoolCreateInfo.bindingSizes = &storageBufferBindingsize;

    result = palCreateDescriptorPool(device, &descriptorPoolCreateInfo, &descriptorPool);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create descriptor pool");
        return PAL_FALSE;
    }

    // allocate a single descriptor set from the descriptor pool
    // using the layout we created above
    result = palAllocateDescriptorSet(device, descriptorPool, descriptorSetLayout, &descriptorSet);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to allocate descriptor set");
        return PAL_FALSE;
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
        logResult(result, "Failed to update descriptor set");
        return PAL_FALSE;
    }

    // create pipeline layout
    PalPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {0};
    pipelineLayoutCreateInfo.descriptorSetLayoutCount = 1;
    pipelineLayoutCreateInfo.descriptorSetLayouts = &descriptorSetLayout;
    pipelineLayoutCreateInfo.usePushConstant = PAL_TRUE;
    pipelineLayoutCreateInfo.pushConstantInfo.offset = 0;
    pipelineLayoutCreateInfo.pushConstantInfo.size = sizeof(PushConstant); // must match shader

    result = palCreatePipelineLayout(device, &pipelineLayoutCreateInfo, &pipelineLayout);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create pipeline layout");
        return PAL_FALSE;
    }

    // create a compute pipeline
    PalComputePipelineCreateInfo pipelineCreateInfo = {0};
    pipelineCreateInfo.computeShader = shader;
    pipelineCreateInfo.pipelineLayout = pipelineLayout;

    result = palCreateComputePipeline(device, &pipelineCreateInfo, &pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create pipeline");
        return PAL_FALSE;
    }

    palDestroyShader(shader);

    // create fence
    result = palCreateFence(device, PAL_FALSE, &fence);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create fence");
        return PAL_FALSE;
    }

    // record commands
    result = palCmdBegin(cmdBuffer, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to begin command buffer");
        return PAL_FALSE;
    }

    PushConstant pushConstant = {0};
    pushConstant.width = BUFFER_SIZE;
    pushConstant.height = BUFFER_SIZE;
    pushConstant.color[0] = 1.0f;
    pushConstant.color[1] = 0.0f;
    pushConstant.color[2] = 0.0f;
    pushConstant.color[3] = 1.0f;

    palCmdBindPipeline(cmdBuffer, pipeline);
    palCmdPushConstants(cmdBuffer, 0, sizeof(PushConstant), &pushConstant);
    palCmdBindDescriptorSet(cmdBuffer, 0, descriptorSet);

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

    uint32_t workGroupInfoCount = 0;
    PalWorkGroupInfo* workGroupInfos = nullptr;
    palBuildWorkGroupInfo(&buildData, &workGroupInfoCount, nullptr);
   
    workGroupInfos = palAllocate(nullptr, sizeof(PalWorkGroupInfo) * workGroupInfoCount, 0);
    if (!workGroupInfos) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
    }

    palBuildWorkGroupInfo(&buildData, &workGroupInfoCount, workGroupInfos);

    // dispatch with the work group info
    for (int i = 0; i < workGroupInfoCount; i++) {
        // palDispatchBase is not supported on all platforms so we dont use it for this example
        // We cap the buffer size small so we only get a single dispatch
        // but you can add fields in yout constants to send the base to the shader driectly
        uint32_t groupCountX = workGroupInfos[i].workGroupCount[0];
        uint32_t groupCountY = workGroupInfos[i].workGroupCount[1];
        uint32_t groupCountZ = workGroupInfos[i].workGroupCount[2];
        palCmdDispatch(cmdBuffer, groupCountX, groupCountY, groupCountZ);
    }
    palFree(nullptr, workGroupInfos);

    // set a barrier so we only read from the buffer after the shader has written to it
    PalBarrierInfo barrierInfo = {0};
    barrierInfo.oldState = PAL_USAGE_STATE_SHADER_WRITE;
    barrierInfo.srcStages = PAL_PIPELINE_STAGE_COMPUTE_SHADER;
    barrierInfo.newState = PAL_USAGE_STATE_TRANSFER_READ;
    barrierInfo.dstStages = PAL_PIPELINE_STAGE_TRANSFER;
    palCmdBufferBarrier(cmdBuffer, buffer, &barrierInfo);

    // now we copy from the GPU buffer into the staging buffer
    PalBufferCopyInfo copyInfo = {0};
    copyInfo.size = bufferBytes;
    palCmdCopyBuffer(cmdBuffer, stagingBuffer, buffer, &copyInfo);

    result = palCmdEnd(cmdBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to end command buffer");
        return PAL_FALSE;
    }

    // submit the command buffer to the GPU
    PalCommandBufferSubmitInfo submitInfo = {0};
    submitInfo.cmdBuffer = cmdBuffer;
    submitInfo.fence = fence;
    submitInfo.waitStages = PAL_PIPELINE_STAGE_COMPUTE_SHADER;
    
    result = palSubmitCommandBuffer(queue, &submitInfo);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to submit command buffer");
        return PAL_FALSE;
    }

    // wait for the fence
    result = palWaitFence(fence, PAL_INFINITE);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to wait fence");
        return PAL_FALSE;
    }

    // now our staging buffer has the contents of the GPU buffer
    // we map it and copy the contents to a ppm buffer and save it
    void* ptr = nullptr;
    result = palMapBuffer(stagingBuffer, 0, bufferBytes, &ptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to map buffer");
        return PAL_FALSE;
    }

    // write to a ppm output file
    FILE* file = fopen("compute_output.ppm", "wb");
    fprintf(file, "P6\n%d %d\n255\n", BUFFER_SIZE, BUFFER_SIZE);
    float* pixels = (float*)ptr;
    for (int y = 0; y < BUFFER_SIZE; y++) {
        int row = BUFFER_SIZE - 1 - y; // flip y
        for (int x = 0; x < BUFFER_SIZE; x++) {
            int index = row * BUFFER_SIZE + x;
            uint8_t rgb[3];

            rgb[0] = pixels[index * 4 + 0] > 0.5f ? 255: 0;
            rgb[1] = pixels[index * 4 + 1] > 0.5f ? 255: 0;
            rgb[2] = pixels[index * 4 + 2] > 0.5f ? 255: 0;
            fwrite(rgb, 1, 3, file);
        }
    }

    fclose(file);
    palUnmapBuffer(stagingBuffer);

    palDestroyPipeline(pipeline);
    palDestroyPipelineLayout(pipelineLayout);
    palFreeCommandBuffer(cmdBuffer);
    palDestroyCommandPool(cmdPool);

    palDestroyDescriptorPool(descriptorPool);
    palDestroyDescriptorSetLayout(descriptorSetLayout);

    palDestroyBuffer(buffer);
    palDestroyBuffer(stagingBuffer);

    palDestroyFence(fence);
    palDestroyQueue(queue);
    palDestroyDevice(device);
    palShutdownGraphics();
    return PAL_TRUE;
}
