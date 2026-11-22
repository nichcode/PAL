
#ifndef ABI_DUMP_VIDEO_H
#define ABI_DUMP_VIDEO_H

#include "helpers.h"

PalBool monitorInfoStruct();
PalBool monitorModeStruct();
PalBool flashInfoStruct();
PalBool iconCreateInfoStruct();
PalBool cursorCreateInfoStruct();

PalBool windowHandleInfoStruct();
PalBool windowCreateInfoStruct();
PalBool videoDriverStruct();

#endif // ABI_DUMP_VIDEO_H