
#include "helpers.h"
#include "pal2/graphics/vertex.h"

bool vertexAttributeStruct(void)
{
    FieldInfo fields[] = {
        { "semanticID", {0, 4}, FIELD(PalVertexAttribute, semanticID) },
        { "type", {4, 4}, FIELD(PalVertexAttribute, type) }
    };

    StructInfo info = {0};
    info.name = "PalVertexAttribute";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 8;
    info.expected.padding = 0;
    info.actual = STRUCT(PalVertexAttribute);

    return dumpStruct(&info);
}

bool vertexLayoutStruct(void)
{
    FieldInfo fields[] = {
        { "attributes", {0, 8}, FIELD(PalVertexLayout, attributes) },
        { "attributeCount", {8, 4}, FIELD(PalVertexLayout, attributeCount) },
        { "type", {12, 4}, FIELD(PalVertexLayout, type) },
        { "binding", {16, 4}, FIELD(PalVertexLayout, binding) },
        { "reserved", {20, 4}, FIELD(PalVertexLayout, reserved) }
    };

    StructInfo info = {0};
    info.name = "PalVertexLayout";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalVertexLayout);

    return dumpStruct(&info);
}