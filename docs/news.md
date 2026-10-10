# Breaking Changes for the Next Major Version

## Event Module
- Rename `PAL_DEFAULT_QUEUE_EVENT_COUNT` to `PAL_DEFAULT_QUEUE_CAPACITY`. The value
is a capacity not a count of exiting events.

## Video Module
- Remove `PAL_VIDEO_FEATURE_MULTI_MONITORS` and reassign bit.
- Rename `palCreateCursorFrom()` to `palCreateSystemCursor()`

## Thread Module
- Rename `PaTlsDestructorFn` to `PalTlsDestructorFn`.