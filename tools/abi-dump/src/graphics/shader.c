
#include "helpers.h"
#include "pal2/graphics/shader.h"

bool meshCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
        { "maxOutputPrimitives", {0, 4}, FIELD(PalMeshShaderCapabilities, maxOutputPrimitives) },
        { "maxOutputVertices", {4, 4}, FIELD(PalMeshShaderCapabilities, maxOutputVertices) },
        { "maxWorkGroupInvocations", {8, 4}, FIELD(PalMeshShaderCapabilities, maxWorkGroupInvocations) },
        { "maxTaskWorkGroupInvocations", {12, 4}, FIELD(PalMeshShaderCapabilities, maxTaskWorkGroupInvocations) },
        { "maxWorkGroupCount", {16, 12}, FIELD(PalMeshShaderCapabilities, maxWorkGroupCount) },
        { "maxTaskWorkGroupCount", {28, 12}, FIELD(PalMeshShaderCapabilities, maxTaskWorkGroupCount) }
    };

    StructInfo info = {0};
    info.name = "PalMeshShaderCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 40;
    info.expected.padding = 0;
    info.actual = STRUCT(PalMeshShaderCapabilities);

    return dumpStruct(&info);
}

bool shaderEntryInfoStruct(void)
{
    FieldInfo fields[] = {
        { "entryName", {0, 8}, FIELD(PalShaderEntryInfo, entryName) },
        { "stage", {8, 4}, FIELD(PalShaderEntryInfo, stage) },
        { "patchControlPoints", {12, 4}, FIELD(PalShaderEntryInfo, patchControlPoints) }
    };

    StructInfo info = {0};
    info.name = "PalShaderEntryInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalShaderEntryInfo);

    return dumpStruct(&info);
}

bool shaderCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "bcode", {0, 8}, FIELD(PalShaderCreateInfo, code) },
        { "entries", {8, 8}, FIELD(PalShaderCreateInfo, entries) },
        { "codeSize", {16, 4}, FIELD(PalShaderCreateInfo, codeSize) },
        { "entryCount", {20, 4}, FIELD(PalShaderCreateInfo, entryCount) }
    };

    StructInfo info = {0};
    info.name = "PalShaderCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalShaderCreateInfo);

    return dumpStruct(&info);
}