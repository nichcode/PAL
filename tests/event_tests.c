
#include "tests.h"
#include "pal2/pal_event.h"

#define EVENT_COUNT 32

typedef struct {
    uint32_t head;
    uint32_t tail;
    PalEvent data[EVENT_COUNT];
} QueueData;

typedef struct {
    PalDispatchMode dispatchMode;
    uint32_t counter;
} EventContext;

static void PAL_CALL eventPush(
    void* queue,
    PalEvent* event)
{
    PalEventQueue* eventQueue = queue;
    QueueData* data = eventQueue->userData;
    data->data[data->tail++ % EVENT_COUNT] = *event;
}

static PalBool PAL_CALL eventPoll(
    void* queue,
    PalEvent* outEvent)
{
    PalEventQueue* eventQueue = queue;
    QueueData* data = eventQueue->userData;
    if (data->head == data->tail) {
        return PAL_FALSE;
    }

    *outEvent = data->data[data->head++ % EVENT_COUNT];
    return PAL_TRUE;
}

static void PAL_CALL onEvent(
    void* userData,
    const PalEvent* event)
{
    EventContext* context = userData;
    context->counter++;
}

static const char* eventTypeToString(PalEventType type)
{
    switch (type) {
        case PAL_EVENT_TYPE_WINDOW_CLOSE:
            return "PAL_EVENT_TYPE_WINDOW_CLOSE";

        case PAL_EVENT_TYPE_WINDOW_SIZE:
            return "PAL_EVENT_TYPE_WINDOW_SIZE";

        case PAL_EVENT_TYPE_WINDOW_MOVE:
            return "PAL_EVENT_TYPE_WINDOW_MOVE";

        case PAL_EVENT_TYPE_WINDOW_STATE:
            return "PAL_EVENT_TYPE_WINDOW_STATE";

        case PAL_EVENT_TYPE_WINDOW_FOCUS:
            return "PAL_EVENT_TYPE_WINDOW_FOCUS";

        case PAL_EVENT_TYPE_WINDOW_VISIBILITY:
            return "PAL_EVENT_TYPE_WINDOW_VISIBILITY";

        case PAL_EVENT_TYPE_WINDOW_MODAL_BEGIN:
            return "PAL_EVENT_TYPE_WINDOW_MODAL_BEGIN";

        case PAL_EVENT_TYPE_WINDOW_MODAL_END:
            return "PAL_EVENT_TYPE_WINDOW_MODAL_END";

        case PAL_EVENT_TYPE_MONITOR_DPI_CHANGED:
            return "PAL_EVENT_TYPE_MONITOR_DPI_CHANGED";

        case PAL_EVENT_TYPE_MONITOR_LIST_CHANGED:
            return "PAL_EVENT_TYPE_MONITOR_LIST_CHANGED";

        case PAL_EVENT_TYPE_KEYDOWN:
            return "PAL_EVENT_TYPE_KEYDOWN";

        case PAL_EVENT_TYPE_KEYREPEAT:
            return "PAL_EVENT_TYPE_KEYREPEAT";

        case PAL_EVENT_TYPE_KEYUP:
            return "PAL_EVENT_TYPE_KEYUP";

        case PAL_EVENT_TYPE_MOUSE_BUTTONDOWN:
            return "PAL_EVENT_TYPE_MOUSE_BUTTONDOWN";

        case PAL_EVENT_TYPE_MOUSE_BUTTONUP:
            return "PAL_EVENT_TYPE_MOUSE_BUTTONUP";

        case PAL_EVENT_TYPE_MOUSE_MOVE:
            return "PAL_EVENT_TYPE_MOUSE_MOVE";

        case PAL_EVENT_TYPE_MOUSE_DELTA:
            return "PAL_EVENT_TYPE_MOUSE_DELTA";

        case PAL_EVENT_TYPE_MOUSE_WHEEL:
            return "PAL_EVENT_TYPE_MOUSE_WHEEL";

        case PAL_EVENT_TYPE_USER:
            return "PAL_EVENT_TYPE_USER";

        case PAL_EVENT_TYPE_KEYCHAR:
            return "PAL_EVENT_TYPE_KEYCHAR";

        case PAL_EVENT_TYPE_WINDOW_DECORATION_MODE:
            return "PAL_EVENT_TYPE_WINDOW_DECORATION_MODE";
    }
}

static void logEventDispatchModes(PalEventDriver* eventDriver)
{
    palLog(nullptr, "Event Type Dispatch Modes:");
    for (uint32_t e = 0; e < PAL_EVENT_TYPE_COUNT; e++) {
        PalDispatchMode mode = palGetEventDispatchMode(eventDriver, e);
        const char* modeStr = "None";

        switch (mode) {
            case PAL_DISPATCH_MODE_CALLBACK: {
                modeStr = "Callback";
                break;
            }

            case PAL_DISPATCH_MODE_POLL: {
                modeStr = "Poll";
                break;
            }
        }

        palLog(nullptr, " %s: %s", eventTypeToString(e), modeStr);
    }
}

static void pushAndPollEvents(
    PalEventDriver* eventDriver, 
    uint32_t* dispatchCounter)
{
    int32_t counter = 0;
    while (counter < EVENT_COUNT) {
        // push all types of event up to max
        for (int32_t i = 0; i < EVENT_COUNT; i++) {
            PalEventType type = i % PAL_EVENT_TYPE_COUNT;
            PalEvent event = {0};
            event.type = type;
            palPushEvent(eventDriver, &event);

            while (palPollEvent(eventDriver, &event)) {
                *dispatchCounter++;
            }
            counter++;
        }
    }
}

PalBool eventDriverTest()
{
    PalEventDriver* eventDriver = nullptr;
    PalEventQueue* queue = palAllocate(nullptr, sizeof(PalEventQueue), 0);
    QueueData* queueData = palAllocate(nullptr, sizeof(QueueData), 0);
    if (!queue || !queueData) {
        return PAL_FALSE;
    }

    memset(queueData, 0, sizeof(QueueData));
    queue->userData = queueData;
    queue->poll = eventPoll;
    queue->push = eventPush;

    EventContext context = {0};
    context.counter = 0;

    PalEventDriverCreateInfo createInfo = {0};
    createInfo.allocator = nullptr;
    createInfo.callback = onEvent;
    createInfo.queue = queue;
    createInfo.userData = &context;

    PalResult result = palCreateEventDriver(&createInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    palLog(nullptr, "Event Driver Created %p", (void*)eventDriver);

    context.dispatchMode = PAL_DISPATCH_MODE_CALLBACK;
    for (uint32_t e = 0; e < PAL_EVENT_TYPE_COUNT; e++) {
        palSetEventDispatchMode(eventDriver, e, context.dispatchMode);
    }

    logEventDispatchModes(eventDriver);
    pushAndPollEvents(eventDriver, &context.counter);
    palLog(nullptr, "Callback Dispatch Counter: %lu", context.counter);
    palLog(nullptr, "");

    context.dispatchMode = PAL_DISPATCH_MODE_POLL;
    for (uint32_t e = 0; e < PAL_EVENT_TYPE_COUNT; e++) {
        palSetEventDispatchMode(eventDriver, e, context.dispatchMode);
    }

    logEventDispatchModes(eventDriver);
    pushAndPollEvents(eventDriver, &context.counter);
    palLog(nullptr, "Poll Dispatch Counter: %lu", context.counter);

    palDestroyEventDriver(eventDriver);
    palFree(nullptr, queue);
    palFree(nullptr, queueData);
}

void registerEventTests()
{
    registerTest(eventDriverTest, "Event Driver Test");
}