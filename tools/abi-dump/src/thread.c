
#include "helpers.h"
#include "pal2/pal_thread.h"

bool threadStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Thread ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    FieldInfo fields[] = {
        {"stackSize", {0, 8}, FIELD(PalThreadCreateInfo, stackSize)},
        {"allocator", {8, 8}, FIELD(PalThreadCreateInfo, allocator)},
        {"entry", {16, 8}, FIELD(PalThreadCreateInfo, entry)},
        {"arg", {24, 8}, FIELD(PalThreadCreateInfo, arg)}
    };

    StructInfo info = {0};
    info.name = "PalThreadCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalThreadCreateInfo);

    return dumpStruct(&info);
}