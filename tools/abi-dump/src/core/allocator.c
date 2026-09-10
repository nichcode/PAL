
#include "helpers.h"

bool allocatorStruct()
{
    FieldInfo fields[] = {
        {"allocate", {0, 8}, FIELD(PalAllocator, allocate)},
        {"free", {8, 8}, FIELD(PalAllocator, free)},
        {"userData", {16, 8}, FIELD(PalAllocator, userData)}
    };

    StructInfo info = {0};
    info.name = "PalAllocator";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalAllocator);

    return dumpStruct(&info);
}