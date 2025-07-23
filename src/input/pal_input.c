
#include "pal_pch.h"
#include "pal_input_internal.h"

PalResult _PCALL palCreateInput(
    PalInputConfig* config,
    PalInput* outInput) {

    PalInput input;
    if (!config || !outInput) {
        return PAL_ERROR_NULL_POINTER;
    }

    if (config->allocator && (!config->allocator->alloc || !config->allocator->free)) {
        return PAL_ERROR_INVALID_ALLOCATOR;
    }

    input = palAllocate(config->allocator, sizeof(struct PalInput_T));
    if (!input) {
        return PAL_ERROR_OUT_OF_MEMORY;
    }

    palZeroMemory(input, sizeof(struct PalInput_T));
    if (config->allocator) {
        input->allocator = config->allocator;
    }

    if (config->eventDriver) {
        input->eventDriver = config->eventDriver;
    }

    PalResult result = palCreateInputData(input->allocator, &input->platformData);
    if (result != PAL_SUCCESS) {
        return result;
    }

    *outInput = input;
    return PAL_SUCCESS;
}

void _PCALL palDestroyInput(
    PalInput input) {

    if (!input) {
        return;
    }
    palDestroyInputData(input->allocator, input->platformData);
    palFree(input->allocator, input);
}