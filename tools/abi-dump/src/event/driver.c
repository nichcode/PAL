
#include "helpers.h"
#include "pal2/event/driver.h"

bool eventDriverCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        {"allocator", {0, 8}, FIELD(PalEventDriverCreateInfo, allocator)},
        {"queue", {8, 8}, FIELD(PalEventDriverCreateInfo, queue)},
        {"callback", {16, 8}, FIELD(PalEventDriverCreateInfo, callback)},
        {"userData", {24, 8}, FIELD(PalEventDriverCreateInfo, userData)},
    };

    StructInfo info = {0};
    info.name = "PalEventDriverCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalEventDriverCreateInfo);

    return dumpStruct(&info);
}