
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
        tmpSize = *size;
        size_t read = fread(buffer, 1, tmpSize, file);
        if (read != tmpSize) {
            return false;
        }
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
    PalBuffer* vertexBuffer = nullptr;
    PalBuffer* instanceBuffer = nullptr;
    PalBuffer* blasBuffer = nullptr;
    PalBuffer* tlasBuffer = nullptr;
    PalBuffer* scratchBuffer = nullptr;

    PalMemory* bufferMemory = nullptr;
    PalMemory* stagingBufferMemory = nullptr;
    PalMemory* vertexBufferMemory = nullptr;
    PalMemory* instanceBufferMemory = nullptr;
    PalMemory* blasBufferMemory = nullptr;
    PalMemory* tlasBufferMemory = nullptr;
    PalMemory* scratchBufferMemory = nullptr;

    PalDescriptorSetLayout* descriptorSetLayout = nullptr;
    PalDescriptorPool* descriptorPool = nullptr;
    PalDescriptorSet* descriptorSet = nullptr;
    PalPipelineLayout* pipelineLayout = nullptr;
    PalPipeline* pipeline = nullptr;
    PalFence* fence = nullptr;

    PalShaderBindingTable* sbt = nullptr;
    PalAccelerationStructure* blas = nullptr;
    PalAccelerationStructure* tlas = nullptr;

    PalGraphicsDebugger debugger;
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

    // create a vertex buffer to store the vertices in
    float vertices[] = {
        0.0f, 1.0f,
        1.0f, -1.0f,
       -1.0f, -1.0f};

    bufferCreateInfo.size = sizeof(vertices);
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE;

    result = palCreateBuffer(device, &bufferCreateInfo, &vertexBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create vertex buffer: %s", error);
        return false;
    }

    // we dont create a staging buffer to copy the vertices
    // since we just need the buffer to create the acceleration structure
    // its still recommended to create a staging buffer so the vertex buffer
    // is GPU memory only
    result = palGetBufferMemoryRequirements(vertexBuffer, &bufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
        return false;
    }

    result = palAllocateMemory(
        device,
        PAL_MEMORY_TYPE_CPU_UPLOAD,
        bufferMemReq.memoryMask,
        bufferMemReq.size,
        &vertexBufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
        return false;
    }

    result = palBindBufferMemory(vertexBuffer, vertexBufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind memory: %s", error);
        return false;
    }

    // copy vertices
    void* data = nullptr;
    result = palMapBufferMemory(vertexBuffer, 0, sizeof(vertices), &data);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to map buffer memory: %s", error);
        return false;
    }

    memcpy(data, vertices, sizeof(vertices));
    palUnmapBufferMemory(vertexBuffer);

    // fill BLAS and triangle geometry
    PalDeviceAddress vertexBufferAddress = palGetBufferDeviceAddress(vertexBuffer);
    PalGeometryDataTriangle triangle = {0};
    triangle.vertexBufferAddress = vertexBufferAddress;
    triangle.vertexCount = 3;
    triangle.vertexType = PAL_VERTEX_TYPE_FLOAT2;
    triangle.vertexStride = sizeof(float) * 2;

    PalGeometry geometry = {0};
    geometry.data = &triangle;
    geometry.primitiveCount = 1;
    geometry.type = PAL_GEOMETRY_TYPE_TRIANGLE;

    PalAccelerationStructureBuildInfo blasBuildInfo = {0};
    blasBuildInfo.type = PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    blasBuildInfo.geometryCount = 1;
    blasBuildInfo.geometries = &geometry;
    blasBuildInfo.buildHints = PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD;
    blasBuildInfo.buildMode = PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD;

    // get the build sizes for blas
    PalAccelerationStructureBuildSize buildSizes = {0};
    result = palGetAccelerationStructureBuildSize(device, &blasBuildInfo, &buildSizes);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get blas build sizes: %s", error);
        return false;
    }

    // create the blas buffer and blas
    bufferCreateInfo.size = buildSizes.accelerationStructureSize;
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE;

    result = palCreateBuffer(device, &bufferCreateInfo, &blasBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create blas buffer: %s", error);
        return false;
    }

    result = palGetBufferMemoryRequirements(blasBuffer, &bufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
        return false;
    }

    result = palAllocateMemory(
        device,
        PAL_MEMORY_TYPE_GPU_ONLY,
        bufferMemReq.memoryMask,
        bufferMemReq.size,
        &blasBufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
        return false;
    }

    result = palBindBufferMemory(blasBuffer, blasBufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind memory: %s", error);
        return false;
    }

    PalAccelerationStructureCreateInfo asCreateInfo = {0};
    asCreateInfo.type = PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    asCreateInfo.buffer = blasBuffer;
    asCreateInfo.size = buildSizes.accelerationStructureSize;

    result = palCreateAccelerationstructure(device, &asCreateInfo, &blas);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create blas: %s", error);
        return false;
    }

    // create instance buffer
    PalAccelerationStructureInstance asInstance = {0};
    asInstance.blas = blas;
    asInstance.instanceId = 0;
    asInstance.mask = 0xFF;

    float transform[12] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f
    };
    memcpy(asInstance.transform, transform, sizeof(float) * 12);

    PalInstanceBufferRequirements instanceBufferReq = {0};
    result = palComputeInstanceBufferRequirements(device, &instanceBufferReq, 1);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to compute instance buffer requirement: %s", error);
        return false;
    }

    bufferCreateInfo.size = instanceBufferReq.size;
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE;

    result = palCreateBuffer(device, &bufferCreateInfo, &instanceBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create instance buffer: %s", error);
        return false;
    }

    result = palGetBufferMemoryRequirements(instanceBuffer, &bufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
        return false;
    }

    result = palAllocateMemory(
        device,
        PAL_MEMORY_TYPE_CPU_UPLOAD,
        bufferMemReq.memoryMask,
        bufferMemReq.size,
        &instanceBufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
        return false;
    }

    result = palBindBufferMemory(instanceBuffer, instanceBufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind memory: %s", error);
        return false;
    }

    // copy instance struct to the buffer
    data = nullptr;
    result = palMapBufferMemory(
        instanceBuffer,
        0,
        instanceBufferReq.size,
        &data);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to map buffer memory: %s", error);
        return false;
    }

    // we can not use a direct memcpy for instance buffers
    result = palWriteInstancesToMappedMemory(device, data, &asInstance, 1);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to write instances to instance buffer: %s", error);
        return false;
    }

    palUnmapBufferMemory(instanceBuffer);

    // fill TLAS and instance geometry
    PalDeviceAddress instanceBufferAddress = palGetBufferDeviceAddress(instanceBuffer);
    PalAccelerationStructureBuildInfo tlasBuildInfo = {0};
    tlasBuildInfo.type = PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    tlasBuildInfo.instanceCount = 1;
    tlasBuildInfo.instanceBufferAddress = instanceBufferAddress;
    tlasBuildInfo.buildHints = PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD;
    tlasBuildInfo.buildMode = PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD;

    // get the build sizes for tlas
    Uint32 blasScratchSize = buildSizes.scratchBufferSize;
    result = palGetAccelerationStructureBuildSize(device, &tlasBuildInfo, &buildSizes);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get tlas build sizes: %s", error);
        return false;
    }

    // create the tlas buffer and tlas
    bufferCreateInfo.size = buildSizes.accelerationStructureSize;
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE;

    result = palCreateBuffer(device, &bufferCreateInfo, &tlasBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create tlas buffer: %s", error);
        return false;
    }

    result = palGetBufferMemoryRequirements(tlasBuffer, &bufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
        return false;
    }

    result = palAllocateMemory(
        device,
        PAL_MEMORY_TYPE_GPU_ONLY,
        bufferMemReq.memoryMask,
        bufferMemReq.size,
        &tlasBufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
        return false;
    }

    result = palBindBufferMemory(tlasBuffer, tlasBufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind memory: %s", error);
        return false;
    }

    asCreateInfo.type = PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    asCreateInfo.buffer = tlasBuffer;
    asCreateInfo.size = buildSizes.accelerationStructureSize;

    result = palCreateAccelerationstructure(device, &asCreateInfo, &tlas);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create tlas: %s", error);
        return false;
    }

    // create scratch buffer
    bufferCreateInfo.size = buildSizes.scratchBufferSize + blasScratchSize;;
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_STORAGE;
    bufferCreateInfo.usages |= PAL_BUFFER_USAGE_DEVICE_ADDRESS;

    result = palCreateBuffer(device, &bufferCreateInfo, &scratchBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create scratch buffer: %s", error);
        return false;
    }

    result = palGetBufferMemoryRequirements(scratchBuffer, &bufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
        return false;
    }

    result = palAllocateMemory(
        device,
        PAL_MEMORY_TYPE_GPU_ONLY,
        bufferMemReq.memoryMask,
        bufferMemReq.size,
        &scratchBufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
        return false;
    }

    result = palBindBufferMemory(scratchBuffer, scratchBufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind memory: %s", error);
        return false;
    }

    // create descriptor set layout
    PalDescriptorSetLayoutBinding descriptorBindings[2];
    PalShaderStage shaderStages[] = { PAL_SHADER_STAGE_RAYGEN };

    // storage buffer to write to
    descriptorBindings[0].binding = 0;
    descriptorBindings[0].descriptorCount = 1; // not an array
    descriptorBindings[0].descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorBindings[0].shaderStageCount = 1;
    descriptorBindings[0].shaderStages = shaderStages;

    // acceleration buffer
    descriptorBindings[1].binding = 1;
    descriptorBindings[1].descriptorCount = 1; // not an array
    descriptorBindings[1].descriptorType = PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE;
    descriptorBindings[1].shaderStageCount = 1;
    descriptorBindings[1].shaderStages = shaderStages;

    PalDescriptorSetLayoutCreateInfo descriptorSetLayoutcreateInfo = {0};
    descriptorSetLayoutcreateInfo.bindingCount = 2;
    descriptorSetLayoutcreateInfo.bindings = descriptorBindings;

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
    PalDescriptorPoolBindingSize bindingSizes[2];
    bindingSizes[0].bindingCount = 1;
    bindingSizes[0].descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    bindingSizes[1].bindingCount = 1;
    bindingSizes[1].descriptorType = PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE;

    PalDescriptorPoolCreateInfo descriptorPoolCreateInfo = {0};
    descriptorPoolCreateInfo.maxDescriptorSets = 1; // only one set
    descriptorPoolCreateInfo.maxDescriptorBindingSizes = 2; // two binding type
    descriptorPoolCreateInfo.bindingSizes = bindingSizes;

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
    PalDescriptorBufferInfo descriptorStorageBufferInfo = {0};
    descriptorStorageBufferInfo.buffer = buffer;
    descriptorStorageBufferInfo.offset = 0;
    descriptorStorageBufferInfo.size = bufferBytes;

    PalDescriptorTLASInfo descriptorTlasInfo;
    descriptorTlasInfo.tlas = tlas;

    PalDescriptorSetWriteInfo writeInfos[2];
    writeInfos[0].binding = 0;
    writeInfos[0].bufferInfo = &descriptorStorageBufferInfo;
    writeInfos[0].descriptorSet = descriptorSet;
    writeInfos[0].descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeInfos[0].descriptorCount = 1;
    writeInfos[0].arrayElement = 0;
    writeInfos[0].imageViewInfo = nullptr;
    writeInfos[0].tlasInfo = nullptr;

    writeInfos[1].binding = 1;
    writeInfos[1].tlasInfo = &descriptorTlasInfo;
    writeInfos[1].descriptorSet = descriptorSet;
    writeInfos[1].descriptorType = PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE;
    writeInfos[1].descriptorCount = 1;
    writeInfos[1].arrayElement = 0;
    writeInfos[1].imageViewInfo = nullptr;
    writeInfos[1].bufferInfo = nullptr;

    result = palUpdateDescriptorSet(device, 2, writeInfos);
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

    // create shader binding table
    PalShaderBindingTableCreateInfo sbtCreateInfo = {0};
    sbtCreateInfo.rayTracingPipeline = pipeline;
    sbtCreateInfo.raygenGroupCount = 1;
    sbtCreateInfo.missGroupCount = 1;
    sbtCreateInfo.hitGroupCount = 1;

    result = palCreateShaderBindingTable(device, &sbtCreateInfo, &sbt);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create shader binding table: %s", error);
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
    result = palCmdBegin(cmdBuffer, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to begin command buffer: %s", error);
        return false;
    }

    PalPipelineBindPoint bindPoint = PAL_PIPELINE_BIND_POINT_RAY_TRACING;
    result = palCmdBindPipeline(cmdBuffer, bindPoint, pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind pipeline: %s", error);
        return false;
    }

    result = palCmdBindDescriptorSet(cmdBuffer, bindPoint, pipelineLayout, 0, descriptorSet);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind descriptor set: %s", error);
        return false;
    }

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

    // build the blas and tlas infos
    PalDeviceAddress scratchBufferAddress = palGetBufferDeviceAddress(scratchBuffer);
    blasBuildInfo.dst = blas;
    blasBuildInfo.scratchBufferAddress = scratchBufferAddress;

    tlasBuildInfo.dst = tlas;
    tlasBuildInfo.scratchBufferAddress = scratchBufferAddress;

    result = palCmdBuildAccelerationStructure(cmdBuffer, &blasBuildInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to build blas: %s", error);
        return false;
    }

    // make sure the BLAS builds before the TLAS
    PalUsageStateInfo oldAsUsageStateInfo = {0};
    oldAsUsageStateInfo.usageState = PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE;

    PalUsageStateInfo newAsUsageStateInfo = {0};
    newAsUsageStateInfo.usageState = PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ;

    result = palCmdMemoryBarrier(cmdBuffer, &oldAsUsageStateInfo, &newAsUsageStateInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set memory barrier: %s", error);
        return false;
    }

    result = palCmdBuildAccelerationStructure(cmdBuffer, &tlasBuildInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to build tlas: %s", error);
        return false;
    }

    // make sure the TLAS builds before the tracing
    result = palCmdMemoryBarrier(cmdBuffer, &oldAsUsageStateInfo, &newAsUsageStateInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set memory barrier: %s", error);
        return false;
    }

    result = palCmdTraceRays(cmdBuffer, sbt, 0, BUFFER_SIZE, BUFFER_SIZE, 1);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to trace rays: %s", error);
        return false;
    }

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
    oldUsageStateInfo.shaderStageCount = 0;
    oldUsageStateInfo.shaderStages = nullptr;
    oldUsageStateInfo.usageState = PAL_USAGE_STATE_UNDEFINED;
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
    result = palWaitFence(fence, UINT64_MAX);
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
    palUnmapBufferMemory(stagingBuffer);

    palDestroyAccelerationstructure(blas);
    palDestroyAccelerationstructure(tlas);

    palDestroyFence(fence);
    palDestroyPipeline(pipeline);
    palDestroyShaderBindingTable(sbt);
    palDestroyPipelineLayout(pipelineLayout);
    palDestroyDescriptorPool(descriptorPool);
    palDestroyDescriptorSetLayout(descriptorSetLayout);

    palDestroyBuffer(buffer);
    palDestroyBuffer(stagingBuffer);
    palDestroyBuffer(vertexBuffer);
    palDestroyBuffer(instanceBuffer);
    palDestroyBuffer(blasBuffer);
    palDestroyBuffer(tlasBuffer);
    palDestroyBuffer(scratchBuffer);

    palFreeMemory(device, bufferMemory);
    palFreeMemory(device, stagingBufferMemory);
    palFreeMemory(device, vertexBufferMemory);
    palFreeMemory(device, instanceBufferMemory);
    palFreeMemory(device, blasBufferMemory);
    palFreeMemory(device, tlasBufferMemory);
    palFreeMemory(device, scratchBufferMemory);

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
