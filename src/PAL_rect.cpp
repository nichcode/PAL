
#include "PAL_pch.h"
#include "PAL/PAL_rect.h"

b8 PAL_RectCollide(const PAL_Rect a, const PAL_Rect b)
{
    if (
        a.x < b.x + b.w  &&
        a.x + a.w > b.x  &&
        a.y < b.y + b.h  &&
        a.y + a.h > b.y
    ) { 
        return true; 
    }

    return false;
}