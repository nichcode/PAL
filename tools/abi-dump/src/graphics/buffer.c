
#include "helpers.h"
#include "pal2/graphics/buffer.h"

bool bufferCopyInfoStruct(void)
{
    FieldInfo fields[] = {
        { "size", {0, 8}, FIELD(PalBufferCopyInfo, size) },
        { "dstOffset", {8, 8}, FIELD(PalBufferCopyInfo, dstOffset) },
        { "srcOffset", {16, 8}, FIELD(PalBufferCopyInfo, srcOffset) }
    };

    StructInfo info = {0};
    info.name = "PalBufferCopyInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalBufferCopyInfo);

    return dumpStruct(&info);
}

bool bufferCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "size", {0, 8}, FIELD(PalBufferCreateInfo, size) },
        { "usages", {8, 4}, FIELD(PalBufferCreateInfo, usages) },
        { "memoryUsage", {12, 4}, FIELD(PalBufferCreateInfo, memoryUsage) }
    };

    StructInfo info = {0};
    info.name = "PalBufferCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalBufferCreateInfo);

    return dumpStruct(&info);
}