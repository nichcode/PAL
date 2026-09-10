
#include "pal2/pal_core.h"
#include <stdbool.h>

#define MAX_LOGGERS 4

typedef struct LoggerContext
{
    bool triggered;
} LoggerContext;

static void PAL_CALL onLogger(
    void* userData,
    const char* msg)
{
    LoggerContext* context = userData;
    context->triggered = true;
}

int main(void)
{
    palLog(nullptr, "Running logger test...");

    LoggerContext contexts[MAX_LOGGERS];
    PalLogger loggers[MAX_LOGGERS];

    for (uint32_t i = 0; i < MAX_LOGGERS; i++) {
        contexts[i].triggered = PAL_FALSE;
        loggers[i].callback = onLogger;
        loggers[i].userData = &contexts[i];
    }

    for (uint32_t i = 0; i < MAX_LOGGERS; i++) {
        palLog(&loggers[i], "This is directed to a logger");
    }

    for (uint32_t i = 0; i < MAX_LOGGERS; i++) {
        if (!contexts[i].triggered) {
            palLog(nullptr, "A logger was not triggered");
            return -1;
        }
    }

    return 0;
}