#pragma once
#include "platform.h"

struct GameBackBuffer
{
    void* Memory;
    i32 Width;
    i32 Height;
    i32 BytesPerPixel;
    i32 RowStride;
};

struct RenderUpdate
{
    i32 XOffset = 0;
    i32 YOffset = 0;
};

void RenderGradient(GameBackBuffer* buffer, RenderUpdate* render);
void RenderRectangle(GameBackBuffer* buffer, 
                     f32 fMinX, f32 fMinY, f32 fMaxX, f32 fMaxY,
                     f32 red, f32 green, f32 blue);
