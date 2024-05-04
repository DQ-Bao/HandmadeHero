#pragma once
#include "platform.h"

struct GameBackBuffer
{
    void* Memory;
    i32 Width;
    i32 Height;
    i32 RowStride;
};

struct RenderUpdate
{
    i32 XOffset = 0;
    i32 YOffset = 0;
};

void Render(GameBackBuffer* buffer, RenderUpdate* render);
