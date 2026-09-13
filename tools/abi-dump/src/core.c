
#include "helpers.h"

bool versionStruct(void)
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

bool allocatorStruct(void)
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

bool loggerStruct(void)
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

bool coreStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Core ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = allocatorStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = versionStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return loggerStruct();
}