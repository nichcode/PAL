
#include "pal/pal_graphics.h"
#include "tests.h"

#define BUFFER_SIZE 400
#define max(a, b) (a > b) ? a : b

typedef struct {
    float color[3]; // closest hit and miss color
} LocalData;

static void PAL_CALL onGraphicsDebug(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg)
{
    palLog(nullptr, msg);
}

PalBool rayTracingTest()
{
    PalAdapter* adapter = nullptr;
    PalDevice* device = nullptr;
    PalQueue* queue = nullptr;
    PalCommandPool* cmdPool = nullptr;
    PalCommandBuffer* cmdBuffer;

    PalShader* rayTracingShader = nullptr;

    PalBuffer* buffer = nullptr;
    PalBuffer* stagingBuffer = nullptr;
    PalBuffer* vertexBuffer = nullptr;
    PalBuffer* instanceBuffer = nullptr;
    PalBuffer* blasBuffer = nullptr;
    PalBuffer* tlasBuffer = nullptr;
    PalBuffer* scratchBuffer = nullptr;

    PalDescriptorSetLayout* descriptorSetLayout = nullptr;
    PalDescriptorPool* descriptorPool = nullptr;
    PalDescriptorSet* descriptorSet = nullptr;
    PalPipelineLayout* pipelineLayout = nullptr;
    PalPipeline* pipeline = nullptr;
    PalFence* fence = nullptr;

    PalShaderBindingTable* sbt = nullptr;
    PalAccelerationStructure* blas = nullptr;
    PalAccelerationStructure* tlas = nullptr;

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
    PalAdapterInfo adapterInfo = {0};
    PalAdapterFeatures adapterFeatures = 0;
    for (int32_t i = 0; i < adapterCount; i++) {
        adapter = adapters[i];
        result = palGetAdapterCapabilities(adapter, &caps);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to get adapter capabilities");
            palFree(nullptr, adapters);
            return PAL_FALSE;
        }

        // Ray tracing is generally implemented on the graphics queue
        if (caps.maxGraphicsQueues == 0) {
            adapter = nullptr;
            continue;
        }

        adapterFeatures = palGetAdapterFeatures(adapter);
        if (!(adapterFeatures & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
            adapter = nullptr;
            continue;
        }

        if (!(adapterFeatures & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS)) {
            adapter = nullptr;
            continue;
        }
        
        // We want an adapter that supports spirv 1.4 or dxil 6.3
        result = palGetAdapterInfo(adapter, &adapterInfo);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to get adapter info");
            return PAL_FALSE;
        }

        // we prefer spirv first if an adapter supports multiple shader formats
        uint32_t target = 0;
        if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
            target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_SPIRV);
            if (target >= PAL_MAKE_SHADER_TARGET(1, 4)) {
                break;
            }
        }

        if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
            target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_DXIL);
            if (target >= PAL_MAKE_SHADER_TARGET(6, 3)) {
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
    PalAdapterFeatures features = PAL_ADAPTER_FEATURE_RAY_TRACING;
    features |= PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS;
    result = palCreateDevice(adapter, features, &device);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create device");
        return PAL_FALSE;
    }

    // create a graphics command queue
    result = palCreateQueue(device, PAL_QUEUE_TYPE_GRAPHICS, &queue);
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

    // create ray tracing shaders
    uint64_t bytecodeSize = 0;
    void* bytecode = nullptr;
    const char* source = nullptr;
    PalShaderEntryInfo entries[3];

    PalShaderCreateInfo shaderCreateInfo = {0};
    if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
        source = "graphics/shaders/bin/spirv/ray_tracing.spv";

    } else if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
        source = "graphics/shaders/bin/dxil/ray_tracing.dxil";
    }

    entries[0].stage = PAL_SHADER_STAGE_RAYGEN;
    entries[0].entryName = "raygenMain";
    entries[0].patchControlPoints = 0;

    entries[1].stage = PAL_SHADER_STAGE_MISS;
    entries[1].entryName = "missMain";
    entries[1].patchControlPoints = 0;

    entries[2].stage = PAL_SHADER_STAGE_CLOSEST_HIT;
    entries[2].entryName = "closestHitMain";
    entries[2].patchControlPoints = 0;

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

    shaderCreateInfo.bytecode = bytecode;
    shaderCreateInfo.bytecodeSize = bytecodeSize;
    shaderCreateInfo.entries = entries;
    shaderCreateInfo.entryCount = 3;

    result = palCreateShader(device, &shaderCreateInfo, &rayTracingShader);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create shader");
        return PAL_FALSE;
    }

    palFree(nullptr, bytecode);

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

    bufferCreateInfo.size = bufferBytes;
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_TRANSFER_DST;
    bufferCreateInfo.memoryUsage = PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_READBACK;

    result = palCreateBuffer(device, &bufferCreateInfo, &stagingBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create buffer");
        return PAL_FALSE;
    }

    // create a vertex buffer to store the vertices in
    float vertices[] = {
        0.0f, 1.0f,
        1.0f, -1.0f,
       -1.0f, -1.0f};

    bufferCreateInfo.size = sizeof(vertices);
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_READ_ONLY_INPUT;
    bufferCreateInfo.usages |= PAL_BUFFER_USAGE_DEVICE_ADDRESS;
    bufferCreateInfo.memoryUsage = PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD;

    result = palCreateBuffer(device, &bufferCreateInfo, &vertexBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create buffer");
        return PAL_FALSE;
    }

    // copy vertices
    void* data = nullptr;
    result = palMapBuffer(vertexBuffer, 0, sizeof(vertices), &data);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to map buffer");
        return PAL_FALSE;
    }

    memcpy(data, vertices, sizeof(vertices));
    palUnmapBuffer(vertexBuffer);

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
    blasBuildInfo.count = 1;
    blasBuildInfo.geometries = &geometry;
    blasBuildInfo.buildHints = PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD;
    blasBuildInfo.buildMode = PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD;

    // get the build sizes for blas
    PalAccelerationStructureBuildSize buildSizes = {0};
    result = palGetAccelerationStructureBuildSize(device, &blasBuildInfo, &buildSizes);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get accleration structure build size");
        return PAL_FALSE;
    }

    // create the blas buffer and blas
    bufferCreateInfo.size = buildSizes.accelerationStructureSize;
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE;
    bufferCreateInfo.usages |= PAL_BUFFER_USAGE_DEVICE_ADDRESS;
    bufferCreateInfo.memoryUsage = PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY;

    result = palCreateBuffer(device, &bufferCreateInfo, &blasBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create buffer");
        return PAL_FALSE;
    }

    PalAccelerationStructureCreateInfo asCreateInfo = {0};
    asCreateInfo.type = PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    asCreateInfo.buffer = blasBuffer;
    asCreateInfo.size = buildSizes.accelerationStructureSize;

    result = palCreateAccelerationstructure(device, &asCreateInfo, &blas);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create acceleration structure");
        return PAL_FALSE;
    }

    // create instance buffer
    PalAccelerationStructureInstance asInstance = {0};
    asInstance.blas = blas;
    asInstance.instanceId = 0;
    asInstance.mask = 0xFF;
    asInstance.hitGroupOffset = 0; // we only have 1 hitGroup

    float transform[12] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f
    };
    memcpy(asInstance.transform, transform, sizeof(float) * 12);

    uint64_t instanceBufferSize = 0;
    result = palComputeInstanceBufferRequirements(
        device, 
        1,
        &instanceBufferSize);

    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to compute buffer requirement");
        return PAL_FALSE;
    }

    bufferCreateInfo.size = instanceBufferSize;
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_READ_ONLY_INPUT;
    bufferCreateInfo.usages |= PAL_BUFFER_USAGE_DEVICE_ADDRESS;
    bufferCreateInfo.memoryUsage = PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD;

    result = palCreateBuffer(device, &bufferCreateInfo, &instanceBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create buffer");
        return PAL_FALSE;
    }

    // copy instance struct to the buffer
    data = nullptr;
    result = palMapBuffer(
        instanceBuffer,
        0,
        instanceBufferSize,
        &data);

    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to map buffer");
        return PAL_FALSE;
    }

    // we can not use a direct memcpy for instance buffers
    result = palWriteToInstanceBuffer(device, data, &asInstance, 1);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to write to buffer");
        return PAL_FALSE;
    }

    palUnmapBuffer(instanceBuffer);

    // fill TLAS and instance geometry
    PalDeviceAddress instanceBufferAddress = palGetBufferDeviceAddress(instanceBuffer);
    PalAccelerationStructureBuildInfo tlasBuildInfo = {0};
    tlasBuildInfo.type = PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    tlasBuildInfo.count = 1;
    tlasBuildInfo.instanceBufferAddress = instanceBufferAddress;
    tlasBuildInfo.buildHints = PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD;
    tlasBuildInfo.buildMode = PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD;

    // get the build sizes for tlas
    uint32_t blasScratchSize = buildSizes.scratchBufferSize;
    result = palGetAccelerationStructureBuildSize(device, &tlasBuildInfo, &buildSizes);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get accleration structure build size");
        return PAL_FALSE;
    }

    // create the tlas buffer and tlas
    bufferCreateInfo.size = buildSizes.accelerationStructureSize;
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE;
    bufferCreateInfo.usages |= PAL_BUFFER_USAGE_DEVICE_ADDRESS;
    bufferCreateInfo.memoryUsage = PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY;

    result = palCreateBuffer(device, &bufferCreateInfo, &tlasBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create buffer");
        return PAL_FALSE;
    }

    asCreateInfo.type = PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    asCreateInfo.buffer = tlasBuffer;
    asCreateInfo.size = buildSizes.accelerationStructureSize;

    result = palCreateAccelerationstructure(device, &asCreateInfo, &tlas);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create acceleration structure");
        return PAL_FALSE;
    }

    // create scratch buffer
    bufferCreateInfo.size = max(buildSizes.scratchBufferSize, blasScratchSize);
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH;
    bufferCreateInfo.usages |= PAL_BUFFER_USAGE_DEVICE_ADDRESS;
    bufferCreateInfo.memoryUsage = PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY;

    result = palCreateBuffer(device, &bufferCreateInfo, &scratchBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create buffer");
        return PAL_FALSE;
    }

    // create descriptor set layout
    PalDescriptorSetLayoutBinding descriptorBindings[2];
    descriptorBindings[0].descriptorCount = 1; // not an array
    descriptorBindings[0].descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    descriptorBindings[1].descriptorCount = 1; // not an array
    descriptorBindings[1].descriptorType = PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE;

    PalDescriptorSetLayoutCreateInfo descriptorSetLayoutcreateInfo = {0};
    descriptorSetLayoutcreateInfo.bindingCount = 2;
    descriptorSetLayoutcreateInfo.bindings = descriptorBindings;

    result = palCreateDescriptorSetLayout(
        device,
        &descriptorSetLayoutcreateInfo,
        &descriptorSetLayout);

    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create descriptor set layout");
        return PAL_FALSE;
    }

    // create descriptor pool
    PalDescriptorPoolBindingSize bindingSizes[2];
    bindingSizes[0].bindingCount = 1;
    bindingSizes[0].descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    bindingSizes[1].bindingCount = 1;
    bindingSizes[1].descriptorType = PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE;

    PalDescriptorPoolCreateInfo descriptorPoolCreateInfo = {0};
    descriptorPoolCreateInfo.maxDescriptorSets = 1; // only one set
    descriptorPoolCreateInfo.bindingSizeCount = 2; // two binding type
    descriptorPoolCreateInfo.bindingSizes = bindingSizes;

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
    PalDescriptorBufferInfo descriptorStorageBufferInfo = {0};
    descriptorStorageBufferInfo.buffer = buffer;
    descriptorStorageBufferInfo.offset = 0;
    descriptorStorageBufferInfo.size = bufferBytes;

    PalDescriptorTLASInfo descriptorTlasInfo;
    descriptorTlasInfo.tlas = tlas;

    PalDescriptorSetWriteInfo writeInfos[2];
    writeInfos[0].layoutBindingIndex = 0;
    writeInfos[0].bufferInfos = &descriptorStorageBufferInfo;
    writeInfos[0].descriptorSet = descriptorSet;
    writeInfos[0].descriptorType = PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeInfos[0].descriptorCount = 1;
    writeInfos[0].arrayElement = 0;
    writeInfos[0].imageViewInfos = nullptr;
    writeInfos[0].tlasInfos = nullptr;

    writeInfos[1].layoutBindingIndex = 1;
    writeInfos[1].tlasInfos = &descriptorTlasInfo;
    writeInfos[1].descriptorSet = descriptorSet;
    writeInfos[1].descriptorType = PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE;
    writeInfos[1].descriptorCount = 1;
    writeInfos[1].arrayElement = 0;
    writeInfos[1].imageViewInfos = nullptr;
    writeInfos[1].bufferInfos = nullptr;

    result = palUpdateDescriptorSet(device, 2, writeInfos);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to update descriptor set");
        return PAL_FALSE;
    }

    // create pipeline layout
    PalPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {0};
    pipelineLayoutCreateInfo.descriptorSetLayoutCount = 1;
    pipelineLayoutCreateInfo.descriptorSetLayouts = &descriptorSetLayout;

    result = palCreatePipelineLayout(device, &pipelineLayoutCreateInfo, &pipelineLayout);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create pipeline layout");
        return PAL_FALSE;
    }

    // the shader group array must respect the corect layout
    // [raygen][miss][hitGroup][callable]
    // [raygen][raygen][miss][hitGroup][hitGroup][callable]
    PalRayTracingShaderGroupCreateInfo shaderGroupCreateInfos[3] = {0};

    // raygen must be packed first always
    shaderGroupCreateInfos[0].type = PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL;
    shaderGroupCreateInfos[0].generalShaderIndex = 0;
    shaderGroupCreateInfos[0].generalShaderEntryIndex = 0;
    shaderGroupCreateInfos[0].anyHitShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[0].closestHitShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[0].intersectionShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[0].maxDataSize = 0; // no extra data

    // miss must be packed second always
    shaderGroupCreateInfos[1].type = PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL;
    shaderGroupCreateInfos[1].generalShaderIndex = 0;
    shaderGroupCreateInfos[1].generalShaderEntryIndex = 1;
    shaderGroupCreateInfos[1].anyHitShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[1].closestHitShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[1].intersectionShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[1].maxDataSize = sizeof(LocalData);

    // hitGroup must be packed third always
    shaderGroupCreateInfos[2].type = PAL_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT;
    shaderGroupCreateInfos[2].closestHitShaderIndex = 0;
    shaderGroupCreateInfos[2].closestHitShaderEntryIndex = 2;
    shaderGroupCreateInfos[2].anyHitShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[2].generalShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[2].intersectionShaderIndex = PAL_UNUSED_SHADER_INDEX;
    shaderGroupCreateInfos[2].maxDataSize = sizeof(LocalData);

    // create a ray tracing pipeline
    PalRayTracingPipelineCreateInfo pipelineCreateInfo = {0};
    pipelineCreateInfo.maxRecursionDepth = 1;
    pipelineCreateInfo.maxPayloadSize = 16;
    pipelineCreateInfo.maxAttributeSize = 8;
    pipelineCreateInfo.pipelineLayout = pipelineLayout;
    pipelineCreateInfo.shaderCount = 1;
    pipelineCreateInfo.shaderGroupCount = 3;
    pipelineCreateInfo.shaderGroups = shaderGroupCreateInfos;
    pipelineCreateInfo.shaders = &rayTracingShader; 

    result = palCreateRayTracingPipeline(device, &pipelineCreateInfo, &pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create pipeline");
        return PAL_FALSE;
    }

    palDestroyShader(rayTracingShader);

    // create shader binding table
    // we need 3 records, only closest hit (git group) only uses the local data
    LocalData missLocalData; // black color for miss
    missLocalData.color[0] = 0.0f;
    missLocalData.color[1] = 0.0f;
    missLocalData.color[2] = 0.0f;
    
    LocalData closestLocalData; // green color for miss
    closestLocalData.color[0] = 0.0f;
    closestLocalData.color[1] = 1.0f;
    closestLocalData.color[2] = 0.0f;

    // the record array must respect the corect layout
    // [raygen][miss][hitGroup][callable]
    // [raygen][raygen][miss][hitGroup][hitGroup][callable]

    PalShaderBindingTableRecordInfo records[3];
    records[0].groupIndex = 0; // raygen group index
    records[0].localDataSize = 0;
    records[0].localData = nullptr;

    records[1].groupIndex = 1; // miss group index
    records[1].localDataSize = sizeof(LocalData);
    records[1].localData = &missLocalData;

    records[2].groupIndex = 2; // hit group index
    records[2].localDataSize = sizeof(LocalData);
    records[2].localData = &closestLocalData;

    PalShaderBindingTableCreateInfo sbtCreateInfo = {0};
    sbtCreateInfo.rayTracingPipeline = pipeline;
    sbtCreateInfo.records = records;
    sbtCreateInfo.recordCount = 3;

    result = palCreateShaderBindingTable(device, &sbtCreateInfo, &sbt);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create shader binding table");
        return PAL_FALSE;
    }

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

    result = palCmdBindPipeline(cmdBuffer, pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to bind pipeline");
        return PAL_FALSE;
    }
    
    result = palCmdBindDescriptorSet(cmdBuffer, 0, descriptorSet);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to bind descriptor set");
        return PAL_FALSE;
    }

    // build the blas and tlas infos
    PalDeviceAddress scratchBufferAddress = palGetBufferDeviceAddress(scratchBuffer);
    blasBuildInfo.dst = blas;
    blasBuildInfo.scratchBufferAddress = scratchBufferAddress;

    tlasBuildInfo.dst = tlas;
    tlasBuildInfo.scratchBufferAddress = scratchBufferAddress;

    result = palCmdBuildAccelerationStructure(cmdBuffer, &blasBuildInfo);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to build acceleration structure");
        return PAL_FALSE;
    }

    // make sure the BLAS builds before the TLAS. We need this barrier because
    // BLAS and TLAS share the same scratch buffer
    PalUsageState oldAsUsageState = PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE;
    PalUsageState newAsUsageState = PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ;

    result = palCmdAccelerationStructureBarrier(
        cmdBuffer, 
        blas, 
        oldAsUsageState, 
        newAsUsageState);
        
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to set barrier");
        return PAL_FALSE;
    }

    result = palCmdBuildAccelerationStructure(cmdBuffer, &tlasBuildInfo);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to build acceleration structure");
        return PAL_FALSE;
    }

    // make sure the TLAS builds before the tracing
    result = palCmdAccelerationStructureBarrier(
        cmdBuffer, 
        tlas, 
        oldAsUsageState, 
        newAsUsageState);

    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to set barrier");
        return PAL_FALSE;
    }

    result = palCmdTraceRays(cmdBuffer, sbt, 0, BUFFER_SIZE, BUFFER_SIZE, 1);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to issue trace command");
        return PAL_FALSE;
    }

    // set a barrier so we only read from the buffer after the shader has written to it
    PalUsageState oldUsageState = PAL_USAGE_STATE_UNDEFINED;
    PalUsageState newUsageState = PAL_USAGE_STATE_TRANSFER_READ;

    result = palCmdBufferBarrier(cmdBuffer, buffer, oldUsageState, newUsageState);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to set barrier");
        return PAL_FALSE;
    }

    // now we copy from the GPU buffer into the staging buffer
    PalBufferCopyInfo copyInfo = {0};
    copyInfo.size = bufferBytes;

    result = palCmdCopyBuffer(cmdBuffer, stagingBuffer, buffer, &copyInfo);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to copy buffer");
        return PAL_FALSE;
    }

    result = palCmdEnd(cmdBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to end command buffer");
        return PAL_FALSE;
    }

    // submit the command buffer to the GPU
    PalCommandBufferSubmitInfo submitInfo = {0};
    submitInfo.cmdBuffer = cmdBuffer;
    submitInfo.fence = fence;
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
    FILE* file = fopen("ray_tracing_output.ppm", "wb");
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

    // update sbt and trace again
    // since we still have the record array we used to create the pipeline
    // we just change the underlying data and update the record
    
    // change miss from black to white
    missLocalData.color[0] = 1.0f;
    missLocalData.color[1] = 1.0f;
    missLocalData.color[2] = 1.0f;

    // change closest hit from green to red
    closestLocalData.color[0] = 1.0f;
    closestLocalData.color[1] = 0.0f;
    closestLocalData.color[2] = 0.0f;

    PalShaderBindingTableRecordInfo updateRecords[2];
    updateRecords[0] = records[1]; // miss
    updateRecords[1] = records[2]; // closest hit

    // update records
    result = palUpdateShaderBindingTable(sbt, 2, updateRecords);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to update shader binding table");
        return PAL_FALSE;
    }

    // we dont need to rebuild the blas or tlas
    // we just delete and create the fence again for simplicity
    palDestroyFence(fence);
    fence = nullptr;

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

    result = palCmdBindPipeline(cmdBuffer, pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to bind pipeline");
        return PAL_FALSE;
    }
    
    result = palCmdBindDescriptorSet(cmdBuffer, 0, descriptorSet);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to bind descriptor set");
        return PAL_FALSE;
    }

    // the previous trace transitioned the buffer to transfer read
    // we need it back to shader write before transfer read
    oldUsageState = PAL_USAGE_STATE_TRANSFER_READ;
    newUsageState = PAL_USAGE_STATE_SHADER_WRITE;

    result = palCmdBufferBarrier(cmdBuffer, buffer, oldUsageState, newUsageState);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to set barrier");
        return PAL_FALSE;
    }

    result = palCmdTraceRays(cmdBuffer, sbt, 0, BUFFER_SIZE, BUFFER_SIZE, 1);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to issue trace command");
        return PAL_FALSE;
    }

    // set a barrier to transition to transfer read so we can read from it after shader has 
    // written to it
    oldUsageState = newUsageState;
    newUsageState = PAL_USAGE_STATE_TRANSFER_READ;

    result = palCmdBufferBarrier(cmdBuffer, buffer, oldUsageState, newUsageState);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to set barrier");
        return PAL_FALSE;
    }

    // now we copy from the GPU buffer into the staging buffer
    copyInfo.size = bufferBytes;
    result = palCmdCopyBuffer(cmdBuffer, stagingBuffer, buffer, &copyInfo);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to copy buffer");
        return PAL_FALSE;
    }

    result = palCmdEnd(cmdBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to end command buffer");
        return PAL_FALSE;
    }

    // submit the command buffer to the GPU
    submitInfo.cmdBuffer = cmdBuffer;
    submitInfo.fence = fence;
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

    // write to a ppm output file
    file = fopen("ray_tracing_output2.ppm", "wb");
    fprintf(file, "P6\n%d %d\n255\n", BUFFER_SIZE, BUFFER_SIZE);
    pixels = (float*)ptr;
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

    palFreeCommandBuffer(cmdBuffer);
    palDestroyCommandPool(cmdPool);
    palDestroyQueue(queue);
    palDestroyDevice(device);

    palShutdownGraphics();
    return PAL_TRUE;
}
