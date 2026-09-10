

#include "helpers.h"

bool loggerStruct()
{
    FieldInfo fields[] = {
        {"callback", {0, 8}, FIELD(PalLogger, callback)},
        {"userData", {8, 8}, FIELD(PalLogger, userData)}
    };

    StructInfo info = {0};
    info.name = "PalLogger";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalLogger);

    return dumpStruct(&info);
}