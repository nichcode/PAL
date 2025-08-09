
#include "tests.h"
#include "pal/pal_input.h"

void inputTest() {

    palLog("");
    palLog("===========================================");
    palLog("Input Test");
    palLog("===========================================");
    palLog("");

    PalResult result;
    PalInputSystem* input = nullptr;

    PalInputSystemCreateInfo createInfo;
    createInfo.allocator = nullptr; // for default.
    createInfo.eventDriver = nullptr; // for default.
    result = palCreateInputSystem(&createInfo, &input);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    palDestroyInputSystem(input);
}