
#include "helpers.h"
#include "pal2/graphics/memory.h"

bool memoryRequirementsStruct(void)
{
    FieldInfo fields[] = {
        { "size", {0, 8}, FIELD(PalMemoryRequirements, size) },
        { "alignment", {8, 8}, FIELD(PalMemoryRequirements, alignment) },
        { "memoryMask", {16, 8}, FIELD(PalMemoryRequirements, memoryMask) },
        { "supportedMemoryTypes", {24, 4}, FIELD(PalMemoryRequirements, supportedMemoryTypes) },
        { "reserved", {28, 4}, FIELD(PalMemoryRequirements, reserved) }
    };

    StructInfo info = {0};
    info.name = "PalMemoryRequirements";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalMemoryRequirements);

    return dumpStruct(&info);
}