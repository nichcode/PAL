
#ifndef ABI_DUMP_GRAPHICS_H
#define ABI_DUMP_GRAPHICS_H

#include "helpers.h"

bool adapterInfoStruct();
bool imageCapabilitiesStruct();
bool resourceCapabilitiesStruct();
bool computeCapabilitiesStruct();
bool viewportCapabilitiesStruct();

bool adapterCapabilitiesStruct();
bool formatInfoStruct();
bool samplerAnisotropyCapabilitiesStruct();
bool multiViewCapabilitiesStruct();
bool multiViewportCapabilitiesStruct();

bool depthStencilCapabilitiesStruct();
bool fragmentShadingRateCapabilitiesStruct();
bool meshCapabilitiesStruct();
bool rayTracingCapabilitiesStruct();
bool descriptorIndexingCapabilitiesStruct();

bool memoryRequirementsStruct();
bool shaderEntryInfoStruct();
bool shaderCreateInfoStruct();
bool surfaceCapabilitiesStruct();
bool swapchainNextImageInfoStruct();

bool swapchainCreateInfoStruct();
bool imageInfoStruct();
bool imageSubresourceRangeStruct();
bool bufferImageCopyInfoStruct();
bool imageCopyInfoStruct();

bool imageCreateInfoStruct();
bool imageViewCreateInfoStruct();
bool samplerCreateInfoStruct();
bool imageStagingRequirementsStruct();
bool bufferCopyInfoStruct();

bool drawIndirectDataStruct();
bool drawIndexedIndirectDataStruct();
bool dispatchIndirectDataStruct();
bool accelerationStructureInstanceStruct();
bool accelerationStructureBuildSizeStruct();

bool geometryDataTriangleStruct();
bool geometryDataAABBSStruct();
bool geometryStruct();
bool accelerationStructureBuildInfoStruct();
bool accelerationStructureCreateInfoStruct();

bool vertexAttributeStruct();
bool vertexLayoutStruct();
bool rasterizerStateStruct();
bool multisampleStateStruct();
bool stencilOpStateStruct();

bool depthStencilStateStruct();
bool colorBlendAttachmentStruct();
bool fragmentShadingRateStateStruct();
bool pushConstantInfoStruct();
bool pipelineLayoutCreateInfoStruct();

bool graphicsPipelineCreateInfoStruct();
bool computePipelineCreateInfoStruct();
bool rayTracingShaderGroupCreateInfoStruct();
bool rayTracingPipelineCreateInfoStruct();
bool descriptorSetLayoutBindingStruct();

bool descriptorPoolBindingSizeStruct();
bool descriptorBufferInfoStruct();
bool descriptorImageViewInfoStruct();
bool descriptorSamplerInfoStruct();
bool descriptorTLASInfoStruct();

bool descriptorSetWriteInfoStruct();
bool descriptorSetLayoutCreateInfoStruct();
bool descriptorPoolCreateInfoStruct();
bool shaderBindingTableRecordInfoStruct();
bool shaderBindingTableCreateInfoStruct();

bool commandBufferSubmitInfoStruct();
bool clearValueStruct();
bool attachmentDescStruct();
bool viewportStruct();
bool rect2DStruct();

bool renderingInfoStruct();
bool renderingLayoutInfoStruct();
bool workGroupBuildDataStruct();
bool workGroupInfoStruct();
bool barrierInfoStruct();

bool graphicsBackendVtable1Struct();
bool graphicsBackendVtable2Struct();
bool graphicsDebuggerStruct();
bool graphicsBackendInfoStruct();

#endif // ABI_DUMP_GRAPHICS_H