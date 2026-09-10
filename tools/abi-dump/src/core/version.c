
#include "helpers.h"

bool versionStruct()
{
    FieldInfo fields[] = {
        {"major", {0, 4}, FIELD(PalVersion, major)},
        {"minor", {4, 4}, FIELD(PalVersion, minor)},
        {"build", {8, 4}, FIELD(PalVersion, build)}
    };

    StructInfo info = {0};
    info.name = "PalVersion";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 12;
    info.expected.padding = 0;
    info.actual = STRUCT(PalVersion);

    return dumpStruct(&info);
}