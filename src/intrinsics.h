#pragma once
#include <math.h>

inline i32 RoundF32ToI32(f32 f)
{
    return (i32)(f + 0.5f);
}

inline u32 RoundF32ToU32(f32 f)
{
    return (u32)(f + 0.5f);
}

inline i32 FloorF32ToI32(f32 f)
{
    return (i32)floorf(f);
}
