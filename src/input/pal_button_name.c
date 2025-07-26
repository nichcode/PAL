
#include "pal_pch.h"
#include "pal_input_internal.h"

void palMapMouseButtonNames(PalInput input) {

    input->mouseButtonNames[PAL_MOUSE_BUTTON_LEFT] = "Mouse Button Left";
    input->mouseButtonNames[PAL_MOUSE_BUTTON_RIGHT] = "Mouse Button Right";
    input->mouseButtonNames[PAL_MOUSE_BUTTON_MIDDLE] = "Mouse Button Middle";
    input->mouseButtonNames[PAL_MOUSE_BUTTON_X1] = "Mouse Button X1";
    input->mouseButtonNames[PAL_MOUSE_BUTTON_X2] = "Mouse Button X2";
}