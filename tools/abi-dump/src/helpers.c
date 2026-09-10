
#include "helpers.h"

uint32_t g_DumpFlags = 0;
static const char* s_FailedString = "FAILED";
static const char* s_PassedString = "PASSED";

bool dumpStruct(const StructInfo* info)
{
    uint32_t fieldSize = 0;
    const uint32_t expectedSize = 14;
    const uint32_t actualSize = 8;
    char seperator[256];
    bool passed = PAL_TRUE;
    const char* result = s_PassedString;

    if (info->expected.size != info->actual.size       ||
        info->expected.align != info->actual.align     ||
        info->expected.padding != info->actual.padding) {
        passed = PAL_FALSE;
        result = s_FailedString;
    }

    for (uint32_t i = 0; i < info->fieldCount; i++) {
        FieldInfo* field = &info->fields[i];
        uint32_t size = (uint32_t)strlen(field->name);
        if (size > fieldSize) {
            fieldSize = size;
        }
    }

    fieldSize += 2;
    uint32_t seperatorSize = fieldSize + expectedSize + actualSize + 4; // add 4 spaces
    for (uint32_t i = 0; i < seperatorSize; i++) {
        seperator[i] = '=';
    }
    
    seperator[seperatorSize] = '\0';

    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "Struct: %s", info->name);
    }

    if (g_DumpFlags & ABI_DUMP_VERBOSE) {
        palLog(nullptr, "");
        palLog(nullptr, "Struct format: Property: (Expected, Actual)");
        palLog(nullptr, "Field format: (Offset, Size)");
        palLog(nullptr, "");

        palLog(nullptr, "Size:      (%04u, %04u)", info->expected.size, info->actual.size);
        palLog(nullptr, "Alignment: (%u, %u)", info->expected.align, info->actual.align);
        palLog(nullptr, "Padding:   (%02u, %02u)", info->expected.padding, info->actual.padding);
        palLog(nullptr, seperator);

        palLog(
            nullptr,
            "%-*s %-*s %-*s",
            fieldSize,
            "Field",
            expectedSize,
            "Expected",
            actualSize,
            "Actual");

        palLog(nullptr, seperator);
    }

    for (uint32_t i = 0; i < info->fieldCount; i++) {
        FieldInfo* field = &info->fields[i];

        if (field->expected.offset != field->actual.offset ||
            field->expected.size != field->actual.size) {
            passed = PAL_FALSE;
            result = s_FailedString;
        }

        if (g_DumpFlags & ABI_DUMP_VERBOSE) {
            palLog(
                nullptr,
                "%-*s (%04u, %03u)    (%04u, %03u)",
                fieldSize,
                field->name,
                field->expected.offset,
                field->expected.size,
                field->actual.offset,
                field->actual.size);
        }
    }
    if (g_DumpFlags & ABI_DUMP_VERBOSE) {
        palLog(nullptr, seperator);
    }

    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "Status: %s", result);
        palLog(nullptr, "");
    }

    return passed;
}