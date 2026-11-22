#include "pal2/core/time.h"
#include "pal2/core/log.h"

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

    Timer timer = {0};
    timer.frequency = palGetPerformanceFrequency();
    if (timer.frequency == 0) {
        palLog(nullptr, "Performance frequency is 0");
        return -1;
    }

    timer.startTime = palGetPerformanceCounter();

    double lastTime = getTime(&timer);
    double totalTime = 0.0;
    int32_t frameCount = 0;

    while (totalTime < 0.05) {
        double now = getTime(&timer);
        if (now < lastTime) {
            palLog(nullptr, "Performance counter moved backwards");
            return -1;
        }
        totalTime = now - lastTime;
        frameCount++;
    }

    if (!frameCount) {
        return -1;
    }

    return 0;
}