
#include "helpers.h"
#include "pal2/graphics/init.h"

bool graphicsDebuggerStruct(void)
{
    FieldInfo fields[] = {
        { "userData", {0, 8}, FIELD(PalGraphicsDebugger, userData) },
        { "callback", {8, 8}, FIELD(PalGraphicsDebugger, callback) },
        { "enableGPUValidation", {16, 4}, FIELD(PalGraphicsDebugger, enableGPUValidation) },
        { "denyGeneral", {20, 4}, FIELD(PalGraphicsDebugger, denyGeneral) },
        { "denyValidation", {24, 4}, FIELD(PalGraphicsDebugger, denyValidation) },
        { "denyPerformance", {28, 4}, FIELD(PalGraphicsDebugger, denyPerformance) },
        { "denyInfoSeverity", {32, 4}, FIELD(PalGraphicsDebugger, denyInfoSeverity) },
        { "denyWarningSeverity", {36, 4}, FIELD(PalGraphicsDebugger, denyWarningSeverity) },
        { "denyErrorSeverity", {40, 4}, FIELD(PalGraphicsDebugger, denyErrorSeverity) },
        { "reserved", {44, 4}, FIELD(PalGraphicsDebugger, reserved) }
    };

    StructInfo info = {0};
    info.name = "PalGraphicsDebugger";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 48;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGraphicsDebugger);

    return dumpStruct(&info);
}

bool graphicsBackendInfoStruct(void)
{
    FieldInfo fields[] = {
        { "vtable", {0, 8}, FIELD(PalGraphicsBackendInfo, vtable) },
        { "version", {8, 4}, FIELD(PalGraphicsBackendInfo, version) },
        { "reserved", {12, 4}, FIELD(PalGraphicsBackendInfo, reserved) }
    };

    StructInfo info = {0};
    info.name = "PalGraphicsBackendInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGraphicsBackendInfo);

    return dumpStruct(&info);
}