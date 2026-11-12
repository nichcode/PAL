
#include "helpers.h"
#include "pal2/graphics/cmd.h"

bool drawIndirectDataStruct(void)
{
    FieldInfo fields[] = {
        { "vertexCount", {0, 4}, FIELD(PalDrawIndirectData, vertexCount) },
        { "instanceCount", {4, 4}, FIELD(PalDrawIndirectData, instanceCount) },
        { "firstVertex", {8, 4}, FIELD(PalDrawIndirectData, firstVertex) },
        { "firstInstance", {12, 4}, FIELD(PalDrawIndirectData, firstInstance) }
    };

    StructInfo info = {0};
    info.name = "PalDrawIndirectData";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDrawIndirectData);

    return dumpStruct(&info);
}

bool drawIndexedIndirectDataStruct(void)
{
    FieldInfo fields[] = {
        { "indexCount", {0, 4}, FIELD(PalDrawIndexedIndirectData, indexCount) },
        { "instanceCount", {4, 4}, FIELD(PalDrawIndexedIndirectData, instanceCount) },
        { "firstIndex", {8, 4}, FIELD(PalDrawIndexedIndirectData, firstIndex) },
        { "vertexOffset", {12, 4}, FIELD(PalDrawIndexedIndirectData, vertexOffset) },
        { "firstInstance", {16, 4}, FIELD(PalDrawIndexedIndirectData, firstInstance) }
    };

    StructInfo info = {0};
    info.name = "PalDrawIndexedIndirectData";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 20;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDrawIndexedIndirectData);

    return dumpStruct(&info);
}

bool dispatchIndirectDataStruct(void)
{
    FieldInfo fields[] = {
        { "groupCountXOrWidth", {0, 4}, FIELD(PalDispatchIndirectData, groupCountXOrWidth) },
        { "groupCountXOrHeight", {4, 4}, FIELD(PalDispatchIndirectData, groupCountXOrHeight) },
        { "groupCountXOrDepth", {8, 4}, FIELD(PalDispatchIndirectData, groupCountXOrDepth) }
    };

    StructInfo info = {0};
    info.name = "PalDispatchIndirectData";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 12;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDispatchIndirectData);

    return dumpStruct(&info);
}

bool clearValueStruct(void)
{
    FieldInfo fields[] = {
        { "color", {0, 16}, FIELD(PalClearValue, color) },
        { "depth", {16, 4}, FIELD(PalClearValue, depth) },
        { "stencil", {20, 4}, FIELD(PalClearValue, stencil) }
    };

    StructInfo info = {0};
    info.name = "PalClearValue";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalClearValue);

    return dumpStruct(&info);
}

bool attachmentDescStruct(void)
{
    FieldInfo fields[] = {
        { "imageView", {0, 8}, FIELD(PalAttachmentDesc, imageView) },
        { "resolveImageView", {8, 8}, FIELD(PalAttachmentDesc, resolveImageView) },
        { "loadOp", {16, 4}, FIELD(PalAttachmentDesc, loadOp) },
        { "storeOp", {20, 4}, FIELD(PalAttachmentDesc, storeOp) },
        { "stencilLoadOp", {24, 4}, FIELD(PalAttachmentDesc, stencilLoadOp) },
        { "stencilStoreOp", {28, 4}, FIELD(PalAttachmentDesc, stencilStoreOp) },
        { "resolveMode", {32, 4}, FIELD(PalAttachmentDesc, resolveMode) },
        { "stencilResolveMode", {36, 4}, FIELD(PalAttachmentDesc, stencilResolveMode) },
        { "clearValue", {40, 24}, FIELD(PalAttachmentDesc, clearValue) },
    };

    StructInfo info = {0};
    info.name = "PalAttachmentDesc";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 64;
    info.expected.padding = 0;
    info.actual = STRUCT(PalAttachmentDesc);

    return dumpStruct(&info);
}

bool viewportStruct(void)
{
    FieldInfo fields[] = {
        { "x", {0, 4}, FIELD(PalViewport, x) },
        { "y", {4, 4}, FIELD(PalViewport, y) },
        { "width", {8, 4}, FIELD(PalViewport, width) },
        { "height", {12, 4}, FIELD(PalViewport, height) },
        { "minDepth", {16, 4}, FIELD(PalViewport, minDepth) },
        { "maxDepth", {20, 4}, FIELD(PalViewport, maxDepth) }
    };

    StructInfo info = {0};
    info.name = "PalViewport";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalViewport);

    return dumpStruct(&info);
}

bool rect2DStruct(void)
{
    FieldInfo fields[] = {
        { "x", {0, 4}, FIELD(PalRect2D, x) },
        { "y", {4, 4}, FIELD(PalRect2D, y) },
        { "width", {8, 4}, FIELD(PalRect2D, width) },
        { "height", {12, 4}, FIELD(PalRect2D, height) },
    };

    StructInfo info = {0};
    info.name = "PalRect2D";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalRect2D);

    return dumpStruct(&info);
}

bool renderingInfoStruct(void)
{
    FieldInfo fields[] = {
        { "colorAttachments", {0, 8}, FIELD(PalRenderingInfo, colorAttachments) },
        { "depthStencilAttachment", {8, 8}, FIELD(PalRenderingInfo, depthStencilAttachment) },
        { "fragmentShadingRateImageView", {16, 8}, FIELD(PalRenderingInfo, fragmentShadingRateImageView) },
        { "renderArea", {24, 16}, FIELD(PalRenderingInfo, renderArea) },
        { "flags", {40, 4}, FIELD(PalRenderingInfo, flags) },
        { "fragmentShadingRateTexelWidth", {44, 4}, FIELD(PalRenderingInfo, fragmentShadingRateTexelWidth) },
        { "fragmentShadingRateTexelHeight", {48, 4}, FIELD(PalRenderingInfo, fragmentShadingRateTexelHeight) },
        { "viewCount", {52, 4}, FIELD(PalRenderingInfo, viewCount) },
        { "arrayLayerCount", {56, 4}, FIELD(PalRenderingInfo, arrayLayerCount) },
        { "colorAttachentCount", {60, 4}, FIELD(PalRenderingInfo, colorAttachentCount) }
    };

    StructInfo info = {0};
    info.name = "PalRenderingInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 64;
    info.expected.padding = 0;
    info.actual = STRUCT(PalRenderingInfo);

    return dumpStruct(&info);
}

bool workGroupBuildDataStruct(void)
{
    FieldInfo fields[] = {
        { "workCount", {0, 12}, FIELD(PalWorkGroupBuildData, workCount) },
        { "workGroupSize", {12, 12}, FIELD(PalWorkGroupBuildData, workGroupSize) },
        { "workGroupCount", {24, 12}, FIELD(PalWorkGroupBuildData, workGroupCount) }
    };

    StructInfo info = {0};
    info.name = "PalWorkGroupBuildData";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 36;
    info.expected.padding = 0;
    info.actual = STRUCT(PalWorkGroupBuildData);

    return dumpStruct(&info);
}

bool workGroupInfoStruct(void)
{
    FieldInfo fields[] = {
        { "workGroupBase", {0, 12}, FIELD(PalWorkGroupInfo, workGroupBase) },
        { "workGroupCount", {12, 12}, FIELD(PalWorkGroupInfo, workGroupCount) }
    };

    StructInfo info = {0};
    info.name = "PalWorkGroupInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalWorkGroupInfo);

    return dumpStruct(&info);
}

bool barrierInfoStruct(void)
{
    FieldInfo fields[] = {
        { "oldState", {0, 4}, FIELD(PalBarrierInfo, oldState) },
        { "newState", {4, 4}, FIELD(PalBarrierInfo, newState) },
        { "srcStages", {8, 4}, FIELD(PalBarrierInfo, srcStages) },
        { "dstStages", {12, 4}, FIELD(PalBarrierInfo, dstStages) }
    };

    StructInfo info = {0};
    info.name = "PalBarrierInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalBarrierInfo);

    return dumpStruct(&info);
}