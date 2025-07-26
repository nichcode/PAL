
#include "pal_pch.h"
#include "pal_input_internal.h"

// ==================================================
// Public API
// ==================================================

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

    PalResult result = palCreateInputData(input);
    if (result != PAL_SUCCESS) {
        return result;
    }

    palMapScancodes(input);
    palMapKeycodes(input);
    palMapScancodeNames(input);

    *outInput = input;
    return PAL_SUCCESS;
}

void _PCALL palDestroyInput(
    PalInput input) {

    if (!input) {
        return;
    }
    palDestroyInputData(input);
    palFree(input->allocator, input);
}

PalResult _PCALL palGetScancodeName(
    PalInput input,
    PalScancode scancode,
    const char** outName) {

    if (!input || !outName) {
        return PAL_ERROR_NULL_POINTER;
    }

    if (scancode < 1 || scancode > PAL_SCANCODE_MAX) {
        return PAL_ERROR_INVALID_SCANCODE;
    }
    *outName = input->scancodeNames[scancode];
    return PAL_SUCCESS;
}

PalResult _PCALL palGetMouseButtonName(
    PalInput input,
    PalMouseButton button,
    const char** outName) {

    if (!input || !outName) {
        return PAL_ERROR_NULL_POINTER;
    }

    if (button < 1 || button > PAL_MOUSE_BUTTON_MAX) {
        return PAL_ERROR_INVALID_MOUSE_BUTTON;
    }
    *outName = input->mouseButtonNames[button];
    return PAL_SUCCESS;
}

PalResult _PCALL palGetKeyboardState(
    PalInput input,
    PalKeyboardState* state) {
    
    if (!input || !state) {
        return PAL_ERROR_NULL_POINTER;
    }
    state->keys = input->keyState;
    state->scancodes = input->scancodeState;
    return PAL_SUCCESS;
}

PalResult _PCALL palGetMouseState(
    PalInput input,
    PalMouseState* state) {
    
    if (!input || !state) {
        return PAL_ERROR_NULL_POINTER;
    }

    state->x = input->mouseX;
    state->y = input->mouseY;
    state->dx = input->mouseDX;
    state->dy = input->mouseDY;
    state->wheelDeltaX = input->mouseWheelDeltaX;
    state->wheelDeltaY = input->mouseWheelDeltaY;
    state->buttons = input->mouseButtonState;
    return PAL_SUCCESS;
}