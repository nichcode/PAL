
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
