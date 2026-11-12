
#include "helpers.h"
#include "pal2/graphics/vtable2.h"

bool graphicsBackendVtable2Struct(void)
{
    FieldInfo fields[] = {
        { "vtable1", {0, 8}, FIELD(PalGraphicsBackendVtable2, vtable1) },
        { "canQueueShareOwnership", {8, 8}, FIELD(PalGraphicsBackendVtable2, canQueueShareOwnership) },
        { "canQueueUseUsageState", {16, 8}, FIELD(PalGraphicsBackendVtable2, canQueueUseUsageState) },
        { "canQueueUsePipelineStages", {24, 8}, FIELD(PalGraphicsBackendVtable2, canQueueUsePipelineStages) },
        { "cmdImageOwnershipTransfer", {32, 8}, FIELD(PalGraphicsBackendVtable2, cmdImageOwnershipTransfer) },
        { "cmdBufferOwnershipTransfer", {40, 8}, FIELD(PalGraphicsBackendVtable2, cmdBufferOwnershipTransfer) },
    };

    StructInfo info = {0};
    info.name = "PalGraphicsBackendVtable2";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 48;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGraphicsBackendVtable2);

    return dumpStruct(&info);
}