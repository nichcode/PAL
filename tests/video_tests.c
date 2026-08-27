
#include "tests.h"
#include "pal2/pal_video.h"

#if PAL_HAS_VIDEO_MODULE

PalBool simpleWindowTest()
{
    PalWindow* window = nullptr;
    PalWindowCreateInfo createInfo = {0};

    PalResult result = palInitVideo(nullptr, nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    createInfo.width = 640;
    createInfo.height = 480;
    createInfo.title = "Simple Window";
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.show = PAL_TRUE;
    createInfo.state = PAL_WINDOW_STATE_NORMAL;

    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    palLog(nullptr, "Window Created %p", (void*)window);
    palDestroyWindow(window);
    palShutdownVideo();

    return PAL_TRUE;
}

#endif // PAL_HAS_VIDEO_MODULE

void registerVideoTests()
{
#if PAL_HAS_VIDEO_MODULE
    registerTest(simpleWindowTest, "Simple Window Test");
#endif // PAL_HAS_VIDEO_MODULE
}