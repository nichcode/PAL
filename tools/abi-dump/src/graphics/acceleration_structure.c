
#include "helpers.h"
#include "pal2/graphics/acceleration_structure.h"

bool accelerationStructureInstanceStruct(void)
{
    FieldInfo fields[] = {
        { "blas", {0, 8}, FIELD(PalAccelerationStructureInstance, blas) },
        { "flags", {8, 4}, FIELD(PalAccelerationStructureInstance, flags) },
        { "mask", {12, 4}, FIELD(PalAccelerationStructureInstance, mask) },
        { "instanceId", {16, 4}, FIELD(PalAccelerationStructureInstance, instanceId) },
        { "hitGroupOffset", {20, 4}, FIELD(PalAccelerationStructureInstance, hitGroupOffset) },
        { "transform", {24, 48}, FIELD(PalAccelerationStructureInstance, transform) }
    };

    StructInfo info = {0};
    info.name = "PalAccelerationStructureInstance";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 72;
    info.expected.padding = 0;
    info.actual = STRUCT(PalAccelerationStructureInstance);

    return dumpStruct(&info);
}

bool accelerationStructureBuildSizeStruct(void)
{
    FieldInfo fields[] = {
        { "accelerationStructureSize", {0, 8}, FIELD(PalAccelerationStructureBuildSize, accelerationStructureSize) },
        { "scratchBufferSize", {8, 8}, FIELD(PalAccelerationStructureBuildSize, scratchBufferSize) },
        { "updateScratchBufferSize", {16, 8}, FIELD(PalAccelerationStructureBuildSize, updateScratchBufferSize) }
    };

    StructInfo info = {0};
    info.name = "PalAccelerationStructureBuildSize";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalAccelerationStructureBuildSize);

    return dumpStruct(&info);
}

bool geometryDataTriangleStruct(void)
{
    FieldInfo fields[] = {
        { "vertexBufferAddress", {0, 8}, FIELD(PalGeometryDataTriangle, vertexBufferAddress) },
        { "indexBufferAddress", {8, 8}, FIELD(PalGeometryDataTriangle, indexBufferAddress) },
        { "transformBufferAddress", {16, 8}, FIELD(PalGeometryDataTriangle, transformBufferAddress) },
        { "vertexType", {24, 4}, FIELD(PalGeometryDataTriangle, vertexType) },
        { "indexType", {28, 4}, FIELD(PalGeometryDataTriangle, indexType) },
        { "vertexCount", {32, 4}, FIELD(PalGeometryDataTriangle, vertexCount) },
        { "vertexStride", {36, 4}, FIELD(PalGeometryDataTriangle, vertexStride) }
    };

    StructInfo info = {0};
    info.name = "PalGeometryDataTriangle";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 40;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGeometryDataTriangle);

    return dumpStruct(&info);
}

bool geometryDataAABBSStruct(void)
{
    FieldInfo fields[] = {
        { "bufferAddress", {0, 8}, FIELD(PalGeometryDataAABBS, bufferAddress) },
        { "stride", {8, 8}, FIELD(PalGeometryDataAABBS, stride) }
    };

    StructInfo info = {0};
    info.name = "PalGeometryDataAABBS";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGeometryDataAABBS);

    return dumpStruct(&info);
}

bool geometryStruct(void)
{
    FieldInfo fields[] = {
        { "data", {0, 8}, FIELD(PalGeometry, data) },
        { "primitiveCount", {8, 8}, FIELD(PalGeometry, primitiveCount) },
        { "flags", {16, 4}, FIELD(PalGeometry, flags) },
        { "type", {20, 4}, FIELD(PalGeometry, type) },
    };

    StructInfo info = {0};
    info.name = "PalGeometry";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGeometry);

    return dumpStruct(&info);
}

bool accelerationStructureBuildInfoStruct(void)
{
    FieldInfo fields[] = {
        { "dst", {0, 8}, FIELD(PalAccelerationStructureBuildInfo, dst) },
        { "src", {8, 8}, FIELD(PalAccelerationStructureBuildInfo, src) },
        { "geometries", {16, 8}, FIELD(PalAccelerationStructureBuildInfo, geometries) },
        { "scratchBufferAddress", {24, 8}, FIELD(PalAccelerationStructureBuildInfo, scratchBufferAddress) },
        { "instanceBufferAddress", {32, 8}, FIELD(PalAccelerationStructureBuildInfo, instanceBufferAddress) },
        { "buildHints", {40, 4}, FIELD(PalAccelerationStructureBuildInfo, buildHints) },
        { "type", {44, 4}, FIELD(PalAccelerationStructureBuildInfo, type) },
        { "buildMode", {48, 4}, FIELD(PalAccelerationStructureBuildInfo, buildMode) },
        { "count", {52, 4}, FIELD(PalAccelerationStructureBuildInfo, count) },
    };

    StructInfo info = {0};
    info.name = "PalAccelerationStructureBuildInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 56;
    info.expected.padding = 0;
    info.actual = STRUCT(PalAccelerationStructureBuildInfo);

    return dumpStruct(&info);
}

bool accelerationStructureCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "buffer", {0, 8}, FIELD(PalAccelerationStructureCreateInfo, buffer) },
        { "offset", {8, 8}, FIELD(PalAccelerationStructureCreateInfo, offset) },
        { "size", {16, 8}, FIELD(PalAccelerationStructureCreateInfo, size) },
        { "type", {24, 4}, FIELD(PalAccelerationStructureCreateInfo, type) },
        { "reserved", {28, 4}, FIELD(PalAccelerationStructureCreateInfo, reserved) }
    };

    StructInfo info = {0};
    info.name = "PalAccelerationStructureCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalAccelerationStructureCreateInfo);

    return dumpStruct(&info);
}