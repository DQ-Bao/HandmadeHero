#include "render.h"

void Render(GameBackBuffer* buffer, RenderUpdate* render)
{
    u8* row = (u8*)buffer->Memory;

    for (i32 y = 0; y < buffer->Height; y++)
    {
        u32* pixel = (u32*)row;
        for (i32 x = 0; x < buffer->Width; x++)
        {
            u8 r = (u8)(x + render->XOffset);
            u8 g = (u8)(y + render->YOffset);
            u8 b = 0;
            *pixel++ = b | (g << 8) | (r << 16);
        }
        row += buffer->RowStride;
    }
}