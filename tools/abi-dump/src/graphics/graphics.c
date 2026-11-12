
#include "graphics.h"

bool graphicsStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Graphics ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = adapterInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = resourceCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = computeCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = viewportCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = adapterCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = formatInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = samplerAnisotropyCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = multiViewCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = multiViewportCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = depthStencilCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = fragmentShadingRateCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = meshCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = rayTracingCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorIndexingCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = memoryRequirementsStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = shaderEntryInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = shaderCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = surfaceCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = swapchainNextImageInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = swapchainCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageSubresourceRangeStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = bufferImageCopyInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageCopyInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageViewCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = samplerCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageStagingRequirementsStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = bufferCopyInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = drawIndirectDataStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = drawIndexedIndirectDataStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = dispatchIndirectDataStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = accelerationStructureInstanceStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = accelerationStructureBuildSizeStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = geometryDataTriangleStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = geometryDataAABBSStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = geometryStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = accelerationStructureBuildInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = accelerationStructureCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = vertexAttributeStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = vertexLayoutStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = rasterizerStateStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = multisampleStateStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = stencilOpStateStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = depthStencilStateStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = colorBlendAttachmentStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = fragmentShadingRateStateStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = pushConstantInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = pipelineLayoutCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = graphicsPipelineCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = computePipelineCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = rayTracingShaderGroupCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = rayTracingPipelineCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorSetLayoutBindingStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorPoolBindingSizeStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorBufferInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorImageViewInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorSamplerInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorTLASInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorSetWriteInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorSetLayoutCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorPoolCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = shaderBindingTableRecordInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = shaderBindingTableCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = commandBufferSubmitInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = clearValueStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = attachmentDescStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = viewportStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = rect2DStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = renderingInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = renderingLayoutInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = workGroupBuildDataStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = workGroupInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = barrierInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = graphicsBackendVtable1Struct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = graphicsBackendVtable2Struct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = graphicsDebuggerStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return graphicsBackendInfoStruct();
}