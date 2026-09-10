
#include "pal2/pal_core.h"
#include <stdbool.h>

typedef struct Timer
{
    uint64_t frequency;
    uint64_t startTime;
} Timer;

static inline double getTime(Timer* timer)
{
    uint64_t now = palGetPerformanceCounter();
    return (double)(now - timer->startTime) / (double)timer->frequency;
}

int main(void)
{
    palLog(nullptr, "Running time test...");

    Timer timer;
    timer.frequency = palGetPerformanceFrequency();
    timer.startTime = palGetPerformanceCounter();

    double lastTime = getTime(&timer);
    double totalTime = 0.0;
    int32_t frameCount = 0;

    while (totalTime < 5.0) {
        double now = getTime(&timer);
        totalTime = now - lastTime;
        frameCount++;
    }

    if (!frameCount) {
        return -1;
    }

    return 0;
}