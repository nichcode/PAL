
#ifndef ABI_DUMP_GRAPHICS_H
#define ABI_DUMP_GRAPHICS_H

#include "helpers.h"

PalBool adapterInfoStruct();
PalBool imageCapabilitiesStruct();
PalBool resourceCapabilitiesStruct();
PalBool computeCapabilitiesStruct();
PalBool viewportCapabilitiesStruct();

PalBool adapterCapabilitiesStruct();
PalBool formatInfoStruct();
PalBool samplerAnisotropyCapabilitiesStruct();
PalBool multiViewCapabilitiesStruct();
PalBool multiViewportCapabilitiesStruct();

PalBool depthStencilCapabilitiesStruct();
PalBool fragmentShadingRateCapabilitiesStruct();
PalBool meshCapabilitiesStruct();
PalBool rayTracingCapabilitiesStruct();
PalBool descriptorIndexingCapabilitiesStruct();

PalBool memoryRequirementsStruct();
PalBool shaderEntryInfoStruct();
PalBool shaderCreateInfoStruct();
PalBool surfaceCapabilitiesStruct();
PalBool swapchainNextImageInfoStruct();

PalBool swapchainCreateInfoStruct();
PalBool imageInfoStruct();
PalBool imageSubresourceRangeStruct();
PalBool bufferImageCopyInfoStruct();
PalBool imageCopyInfoStruct();

PalBool imageCreateInfoStruct();
PalBool imageViewCreateInfoStruct();
PalBool samplerCreateInfoStruct();
PalBool imageStagingRequirementsStruct();
PalBool bufferCopyInfoStruct();

PalBool drawIndirectDataStruct();
PalBool drawIndexedIndirectDataStruct();
PalBool dispatchIndirectDataStruct();
PalBool accelerationStructureInstanceStruct();
PalBool accelerationStructureBuildSizeStruct();

PalBool geometryDataTriangleStruct();
PalBool geometryDataAABBSStruct();
PalBool geometryStruct();
PalBool accelerationStructureBuildInfoStruct();
PalBool accelerationStructureCreateInfoStruct();

PalBool vertexAttributeStruct();
PalBool vertexLayoutStruct();
PalBool rasterizerStateStruct();
PalBool multisampleStateStruct();
PalBool stencilOpStateStruct();

PalBool depthStencilStateStruct();
PalBool colorBlendAttachmentStruct();
PalBool fragmentShadingRateStateStruct();
PalBool pushConstantInfoStruct();
PalBool pipelineLayoutCreateInfoStruct();

PalBool graphicsPipelineCreateInfoStruct();
PalBool computePipelineCreateInfoStruct();
PalBool rayTracingShaderGroupCreateInfoStruct();
PalBool rayTracingPipelineCreateInfoStruct();
PalBool descriptorSetLayoutBindingStruct();

PalBool descriptorPoolBindingSizeStruct();
PalBool descriptorBufferInfoStruct();
PalBool descriptorImageViewInfoStruct();
PalBool descriptorSamplerInfoStruct();
PalBool descriptorTLASInfoStruct();

PalBool descriptorSetWriteInfoStruct();
PalBool descriptorSetLayoutCreateInfoStruct();
PalBool descriptorPoolCreateInfoStruct();
PalBool shaderBindingTableRecordInfoStruct();
PalBool shaderBindingTableCreateInfoStruct();

PalBool commandBufferSubmitInfoStruct();
PalBool clearValueStruct();
PalBool attachmentDescStruct();
PalBool viewportStruct();
PalBool rect2DStruct();

PalBool renderingInfoStruct();
PalBool renderingLayoutInfoStruct();
PalBool workGroupBuildDataStruct();
PalBool workGroupInfoStruct();
PalBool barrierInfoStruct();

PalBool graphicsBackendVtable1Struct();
PalBool graphicsBackendVtable2Struct();
PalBool graphicsDebuggerStruct();
PalBool graphicsBackendInfoStruct();

#endif // ABI_DUMP_GRAPHICS_H