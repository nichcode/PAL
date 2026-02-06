
#include "pal/pal_graphics.h"
#include "tests.h"

#include <stdio.h>

#define BUFFER_SIZE 400

static bool readFile(
    const char* filename,
    void* buffer,
    Uint64* size)
{
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return false;
    }

    fseek(file, 0, SEEK_END);
    Uint64 tmpSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (buffer) {
        fread(buffer, 1, (size_t)size, file);
    }

    fclose(file);
    *size = tmpSize;
    return true;
}

static void PAL_CALL onGraphicsDebug(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg)
{
    palLog(nullptr, msg);
}

bool rayTracingTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Ray Tracing Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalAdapter* adapter = nullptr;
    PalDevice* device = nullptr;
    PalQueue* queue = nullptr;
    PalCommandPool* cmdPool = nullptr;
    PalCommandBuffer* cmdBuffer;

    PalShader* raygenShader = nullptr;
    PalShader* missShader = nullptr;
    PalShader* closestHitShader = nullptr;
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

    PalGraphicsDebugger debugger;
    debugger.callback = onGraphicsDebug;
    debugger.userData = nullptr;

    PalResult result = palInitGraphics(&debugger, nullptr);
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

    PalAdapterCapabilities caps;
    PalAdapterFeatures adapterFeatures = 0;
    bool hasGraphicsQueue = false;
    for (Int32 i = 0; i < adapterCount; i++) {
        adapter = adapters[i];
        result = palGetAdapterCapabilities(adapter, &caps);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter capabilities: %s", error);
            palFree(nullptr, adapters);
            return false;
        }

        // Ray tracing is generally implemented on the graphics queue
        if (caps.maxGraphicsQueues == 0) {
            continue;

        } else {
            hasGraphicsQueue = true;
            adapterFeatures = palGetAdapterFeatures(adapter);
            if (adapterFeatures & PAL_ADAPTER_FEATURE_RAY_TRACING) {
                break;
            }
        }
    }

    palFree(nullptr, adapters);
    if (!adapter) {
        if (hasGraphicsQueue) {
            palLog(nullptr, "Failed to find an adapter that supports graphics queue");

        } else {
            palLog(nullptr, "Failed to find an adapter that supports ray tracing");
        }
        return false;
    }

    PalAdapterInfo adapterInfo = {0};
    result = palGetAdapterInfo(adapter, &adapterInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get adapter info: %s", error);
        return false;
    }

    // create a device
    PalAdapterFeatures features = PAL_ADAPTER_FEATURE_RAY_TRACING;
    result = palCreateDevice(adapter, features, &device);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create device: %s", error);
        return false;
    }

    // create a graphics command queue
    result = palCreateQueue(device, PAL_QUEUE_TYPE_GRAPHICS, &queue);
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

    // create a raygen shader
    Uint64 bytecodeSize = 0;
    void* bytecode = nullptr;
    PalShaderCreateInfo shaderCreateInfo = {0};
    const char* shaderPath = nullptr;
    if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
        shaderPath = "shaders/raygen.spv";
    }

    if (!readFile(shaderPath, nullptr, &bytecodeSize)) {
        palLog(nullptr, "Failed to find shader file");
        return false;
    }

    bytecode = palAllocate(nullptr, bytecodeSize, 0);
    if (!bytecode) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    readFile(shaderPath, bytecode, &bytecodeSize);
    shaderCreateInfo.bytecode = bytecode;
    shaderCreateInfo.bytecodeSize = bytecodeSize;
    shaderCreateInfo.stage = PAL_SHADER_STAGE_RAYGEN;

    result = palCreateShader(device, &shaderCreateInfo, &raygenShader);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create raygen shader: %s", error);
        return false;
    }
    palFree(nullptr, bytecode);

    // create a miss shader
    bytecodeSize = 0;
    bytecode = nullptr;
    if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
        shaderPath = "shaders/miss.spv";
    }

    if (!readFile(shaderPath, nullptr, &bytecodeSize)) {
        palLog(nullptr, "Failed to find shader file");
        return false;
    }

    bytecode = palAllocate(nullptr, bytecodeSize, 0);
    if (!bytecode) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    readFile(shaderPath, bytecode, &bytecodeSize);
    shaderCreateInfo.bytecode = bytecode;
    shaderCreateInfo.bytecodeSize = bytecodeSize;
    shaderCreateInfo.stage = PAL_SHADER_STAGE_MISS;

    result = palCreateShader(device, &shaderCreateInfo, &missShader);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create miss shader: %s", error);
        return false;
    }
    palFree(nullptr, bytecode);

    // create a closest hit shader
    bytecodeSize = 0;
    bytecode = nullptr;
    if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
        shaderPath = "shaders/closest_hit.spv";
    }

    if (!readFile(shaderPath, nullptr, &bytecodeSize)) {
        palLog(nullptr, "Failed to find shader file");
        return false;
    }

    bytecode = palAllocate(nullptr, bytecodeSize, 0);
    if (!bytecode) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    readFile(shaderPath, bytecode, &bytecodeSize);
    shaderCreateInfo.bytecode = bytecode;
    shaderCreateInfo.bytecodeSize = bytecodeSize;
    shaderCreateInfo.stage = PAL_SHADER_STAGE_CLOSEST_HIT;

    result = palCreateShader(device, &shaderCreateInfo, &closestHitShader);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create closest hit shader: %s", error);
        return false;
    }
    palFree(nullptr, bytecode);

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
    PalShaderStage shaderStages[] = { PAL_SHADER_STAGE_RAYGEN };

    descriptorBinding.binding = 0;
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
    writeInfo.binding = 0;
    writeInfo.bufferInfo = &descriptorBufferInfo;
    writeInfo.descriptorSet = descriptorSet;
    writeInfo.descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeInfo.descriptorCount = 1;

    result = palUpdateDescriptorSet(device, 1, &writeInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to update descriptor set: %s", error);
        return false;
    }

    // create pipeline layout
    PalPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {0};
    pipelineLayoutCreateInfo.descriptorSetLayoutCount = 1;
    pipelineLayoutCreateInfo.descriptorSetLayouts = &descriptorSetLayout;

    result = palCreatePipelineLayout(device, &pipelineLayoutCreateInfo, &pipelineLayout);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create pipeline layout: %s", error);
        return false;
    }

    // shader and shader groups
    PalShader* shaders[3];
    shaders[0] = raygenShader;
    shaders[1] = missShader;
    shaders[2] = closestHitShader;

    PalRayTracingShaderGroupCreateInfo shaderGroupCreateInfos[3] = {0};
    shaderGroupCreateInfos[0].type = PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL;
    shaderGroupCreateInfos[0].generalShaderIndex = 0; // must match raygen index
    shaderGroupCreateInfos[0].anyHitShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[0].closestHitShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[0].intersectionShaderIndex = PAL_UNUSED_SHADER_INDEX;

    shaderGroupCreateInfos[1].type = PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL;
    shaderGroupCreateInfos[1].generalShaderIndex = 1; // must match miss index
    shaderGroupCreateInfos[1].anyHitShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[1].closestHitShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[1].intersectionShaderIndex = PAL_UNUSED_SHADER_INDEX;

    shaderGroupCreateInfos[2].type = PAL_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT;
    shaderGroupCreateInfos[2].closestHitShaderIndex = 2; // must match closest hit index
    shaderGroupCreateInfos[2].anyHitShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[2].generalShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[2].intersectionShaderIndex = PAL_UNUSED_SHADER_INDEX;

    // create a ray tracing pipeline
    PalRayTracingPipelineCreateInfo pipelineCreateInfo = {0};
    pipelineCreateInfo.maxRecursionDepth = 1;
    pipelineCreateInfo.pipelineLayout = pipelineLayout;
    pipelineCreateInfo.shaderCount = 3;
    pipelineCreateInfo.shaderGroupCount = 3;
    pipelineCreateInfo.shaderGroups = shaderGroupCreateInfos;
    pipelineCreateInfo.shaders = shaders;

    result = palCreateRayTracingPipeline(device, &pipelineCreateInfo, &pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create ray tracing pipeline: %s", error);
        return false;
    }

    // create fence
    result = palCreateFence(device, false, &fence);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create fence: %s", error);
        return false;
    }

    // record commands
    result = palBeginCommandBuffer(cmdBuffer, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to begin command buffer: %s", error);
        return false;
    }

    result = palBindPipeline(cmdBuffer, pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind pipeline: %s", error);
        return false;
    }

    result = palBindDescriptorSet(cmdBuffer, pipeline, pipelineLayout, 0, descriptorSet);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind descriptor set: %s", error);
        return false;
    }

    // set a barrier on the buffer
    PalUsageStateInfo oldUsageStateInfo = {0};
    oldUsageStateInfo.shaderStage = PAL_SHADER_STAGE_UNDEFINED;
    oldUsageStateInfo.usageState = PAL_USAGE_STATE_UNDEFINED;

    PalUsageStateInfo newUsageStateInfo = {0};
    newUsageStateInfo.shaderStage = PAL_SHADER_STAGE_RAYGEN;
    newUsageStateInfo.usageState = PAL_USAGE_STATE_SHADER_WRITE;

    result = palBufferBarrier(cmdBuffer, buffer, &oldUsageStateInfo, &newUsageStateInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set buffer barrier: %s", error);
        return false;
    }

    result = palTraceRays(cmdBuffer, BUFFER_SIZE, BUFFER_SIZE, 1);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to trace rays: %s", error);
        return false;
    }

    // set a barrier so we only read from the buffer after the shader has
    // written to it
    oldUsageStateInfo = newUsageStateInfo;
    newUsageStateInfo.shaderStage = PAL_SHADER_STAGE_UNDEFINED;
    newUsageStateInfo.usageState = PAL_USAGE_STATE_TRANSFER_READ;

    result = palBufferBarrier(cmdBuffer, buffer, &oldUsageStateInfo, &newUsageStateInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set buffer barrier: %s", error);
        return false;
    }

    // set a barrier on the staging buffer
    oldUsageStateInfo.usageState = PAL_USAGE_STATE_UNDEFINED;
    oldUsageStateInfo.shaderStage = PAL_SHADER_STAGE_UNDEFINED;

    newUsageStateInfo.shaderStage = PAL_SHADER_STAGE_UNDEFINED;
    newUsageStateInfo.usageState = PAL_USAGE_STATE_TRANSFER_WRITE;

    result = palBufferBarrier(cmdBuffer, stagingBuffer, &oldUsageStateInfo, &newUsageStateInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set buffer barrier: %s", error);
        return false;
    }

    // now we copy from the GPU buffer into the staging buffer
    result = palCopyBuffer(cmdBuffer, stagingBuffer, buffer, 0, 0, bufferBytes);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to copy buffer: %s", error);
        return false;
    }

    result = palEndCommandBuffer(cmdBuffer);
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
    result = palWaitFence(fence, UINT64_MAX);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to wait for fence: %s", error);
        return false;
    }

    // now our staging buffer has the contents of the GPU buffer
    // we map it and copy the contents to a ppm buffer and save it
    void* ptr = nullptr;
    result = palMapMemory(device, stagingBufferMemory, 0, bufferBytes, &ptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to map memory: %s", error);
        return false;
    }

    // write to a ppm output file
    FILE* file = fopen("ray_tracing_output.ppm", "wb");
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
    palUnmapMemory(device, stagingBufferMemory);

    palDestroyFence(fence);
    palDestroyPipeline(pipeline);
    palDestroyPipelineLayout(pipelineLayout);
    palDestroyDescriptorPool(descriptorPool);
    palDestroyDescriptorSetLayout(descriptorSetLayout);

    palDestroyBuffer(buffer);
    palDestroyBuffer(stagingBuffer);
    palFreeMemory(device, bufferMemory);
    palFreeMemory(device, stagingBufferMemory);

    palDestroyShader(raygenShader);
    palDestroyShader(missShader);
    palDestroyShader(closestHitShader);

    palFreeCommandBuffer(cmdBuffer);
    palDestroyCommandPool(cmdPool);
    palDestroyQueue(queue);
    palDestroyDevice(device);

    palShutdownGraphics();
    return true;
}
