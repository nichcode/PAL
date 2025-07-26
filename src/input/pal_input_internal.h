
#ifndef _PAL_INPUT_INTERNAL_H
#define _PAL_INPUT_INTERNAL_H

#include "pal/pal_input.h"

struct PalInput_T {
    PalAllocator* allocator;
    PalEventDriver eventDriver;

    PalScancode scancodes[512];
    PalKey keycodes[256];
    bool scancodeState[PAL_SCANCODE_MAX];
    bool keyState[PAL_KEY_MAX];
    const char* scancodeNames[PAL_SCANCODE_MAX];

    int mouseX;
    int mouseY;
    int mouseDX;
    int mouseDY;
    int mouseWheelDeltaX;
    int mouseWheelDeltaY;
    bool mouseButtonState[PAL_MOUSE_BUTTON_MAX];
    const char* mouseButtonNames[PAL_MOUSE_BUTTON_MAX];
    
    void* platformData;
};

PalResult palCreateInputData(PalInput input);
void palDestroyInputData(PalInput input);

void palMapScancodes(PalInput input);
void palMapKeycodes(PalInput input);
void palMapScancodeNames(PalInput input);

void palMapMouseButtonCodes(PalInput input);
void palMapMouseButtonNames(PalInput input);

#endif // _PAL_INPUT_INTERNAL_H