
#include "bench.h"
#include "pal/pal_video.h"

static inline void createDestroy()
{
    PalResult result;
    PalWindow* window = nullptr;

    PalWindowCreateInfo createInfo = {0};
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = true;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title = "PAL Window";

    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create window %s", error);
        return;
    }

    palDestroyWindow(window);
}

void windowBench()
{
    // initialize PAL video
    PalResult result = palInitVideo(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video %s", error);
        return;
    }

    runBench(createDestroy, "PAL Window Benchmark", MAX_ITERATIONS);

    // shutdown PAL video
    palShutdownVideo();
}