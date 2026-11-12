
#include "helpers.h"
#include "pal2/graphics/cmdbuffer.h"

bool commandBufferSubmitInfoStruct(void)
{
    FieldInfo fields[] = {
        { "waitValue", {0, 8}, FIELD(PalCommandBufferSubmitInfo, waitValue) },
        { "signalValue", {8, 8}, FIELD(PalCommandBufferSubmitInfo, signalValue) },
        { "cmdBuffer", {16, 8}, FIELD(PalCommandBufferSubmitInfo, cmdBuffer) },
        { "waitSemaphore", {24, 8}, FIELD(PalCommandBufferSubmitInfo, waitSemaphore) },
        { "signalSemaphore", {32, 8}, FIELD(PalCommandBufferSubmitInfo, signalSemaphore) },
        { "fence", {40, 8}, FIELD(PalCommandBufferSubmitInfo, fence) },
        { "waitStages", {48, 4}, FIELD(PalCommandBufferSubmitInfo, waitStages) },
        { "signalStages", {52, 4}, FIELD(PalCommandBufferSubmitInfo, signalStages) }
    };

    StructInfo info = {0};
    info.name = "PalCommandBufferSubmitInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 56;
    info.expected.padding = 0;
    info.actual = STRUCT(PalCommandBufferSubmitInfo);

    return dumpStruct(&info);
}