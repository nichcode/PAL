
#include "helpers.h"
#include "pal2/graphics/sbt.h"

bool shaderBindingTableRecordInfoStruct(void)
{
    FieldInfo fields[] = {
        { "localData", {0, 8}, FIELD(PalShaderBindingTableRecordInfo, localData) },
        { "groupIndex", {8, 4}, FIELD(PalShaderBindingTableRecordInfo, groupIndex) },
        { "localDataSize", {12, 4}, FIELD(PalShaderBindingTableRecordInfo, localDataSize) }
    };

    StructInfo info = {0};
    info.name = "PalShaderBindingTableRecordInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalShaderBindingTableRecordInfo);

    return dumpStruct(&info);
}

bool shaderBindingTableCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "records", {0, 8}, FIELD(PalShaderBindingTableCreateInfo, records) },
        { "rayTracingPipeline", {8, 8}, FIELD(PalShaderBindingTableCreateInfo, rayTracingPipeline) },
        { "recordCount", {16, 4}, FIELD(PalShaderBindingTableCreateInfo, recordCount) },
        { "reserved", {20, 4}, FIELD(PalShaderBindingTableCreateInfo, reserved) }
    };

    StructInfo info = {0};
    info.name = "PalShaderBindingTableCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalShaderBindingTableCreateInfo);

    return dumpStruct(&info);
}