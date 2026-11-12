# Breaking Changes for the Next Major Version

## Event Module
- Rename `PAL_DEFAULT_QUEUE_EVENT_COUNT` to `PAL_DEFAULT_QUEUE_CAPACITY`. The value
is a capacity not a count of exiting events.

## Video Module
- Remove `PAL_VIDEO_FEATURE_MULTI_MONITORS` and reassign bit.
- Rename `palCreateCursorFrom()` to `palCreateSystemCursor()`

## Thread Module
- Rename `PaTlsDestructorFn` to `PalTlsDestructorFn`.

## System Module
- Rename `PAL_PLATFORM_API_TYPE_ANDRIOD` to `PAL_PLATFORM_API_TYPE_ANDROID`.
- Rename `PalPlatformInfo::totalMemory` to `PalPlatformInfo::totalDisk`.

# Graphics Module
- Rename `PAL_IMAGE_USAGE_COLOR_ATTACHEMENT` to `PAL_IMAGE_USAGE_COLOR_ATTACHMENT`.
- Rename `PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT` to `PAL_IMAGE_USAGE_DEPTH_ATTACHMENT`.
- Rename `PalCompositeAplha` to `PalCompositeAlpha`.
- Rename `PalSwapchainCreateInfo::compositeAlpha` to `PalSwapchainCreateInfo::compositeAlpha`.
- Rename `PalRenderingInfo::colorAttachentCount` to `PalRenderingInfo::colorAttachmentCount`.
- Rename `PalRenderingLayoutInfo::colorAttachentCount` to `PalRenderingLayoutInfo::colorAttachmentCount`.
- Rename `PalGraphicsBackendVtable1::createAccelerationstructure` to `PalGraphicsBackendVtable1::createAccelerationStructure`.
- Rename `PalGraphicsBackendVtable1::destroyAccelerationstructure` to `PalGraphicsBackendVtable1::destroyAccelerationStructure`.
- Rename `palCreateAccelerationstructure` to `palCreateAccelerationStructure`.
- Rename `PalDispatchIndirectData::groupCountXOrHeight` to `PalDispatchIndirectData::groupCountYOrHeight`.
- Rename `PalDispatchIndirectData::groupCountXOrDepth` to `PalDispatchIndirectData::groupCountZOrDepth`.
- Rename `PalBufferImageCopyInfo::ImageMipLevel` to `PalBufferImageCopyInfo::imageMipLevel`.
- Rename `PalBufferImageCopyInfo::ImageStartArrayLayer` to `PalBufferImageCopyInfo::imageStartArrayLayer`.
- Rename `PalBufferImageCopyInfo::ImageArrayLayerCount` to `PalBufferImageCopyInfo::imageArrayLayerCount`.
- Rename `PalImageSubresourceRange::layerArrayCount` to `PalImageSubresourceRange::arrayLayerCount`.