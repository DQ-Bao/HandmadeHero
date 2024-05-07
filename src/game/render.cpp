#include "render.h"
#include "intrinsics.h"

void RenderGradient(GameBackBuffer* buffer, RenderUpdate* render)
{
    u8* row = (u8*)buffer->Memory;

    for (i32 y = 0; y < buffer->Height; y++)
    {
        u32* pixel = (u32*)row;
        for (i32 x = 0; x < buffer->Width; x++)
        {
            u8 r = (u8)(x - render->XOffset);
            u8 g = (u8)(y - render->YOffset);
            u8 b = 0;
            *pixel++ = b | (g << 8) | (r << 16);
        }
        row += buffer->RowStride;
    }
}

void RenderRectangle(GameBackBuffer* buffer, 
                     f32 fMinX, f32 fMinY, f32 fMaxX, f32 fMaxY, 
                     f32 red, f32 green, f32 blue)
{
    i32 minX = RoundF32ToI32(fMinX);
    i32 minY = RoundF32ToI32(fMinY);
    i32 maxX = RoundF32ToI32(fMaxX);
    i32 maxY = RoundF32ToI32(fMaxY);
    if (minX < 0)
    {
        minX = 0;
    }
    if (minY < 0)
    {
        minY = 0;
    }
    if (maxX > buffer->Width)
    {
        maxX = buffer->Width;
    }
    if (maxY > buffer->Height)
    {
        maxY = buffer->Height;
    }
    u32 color = (RoundF32ToU32(red * 255.0f) << 16) | 
                (RoundF32ToU32(green * 255.0f) << 8) | 
                (RoundF32ToU32(blue * 255.0f));
    u8* row = (u8*)buffer->Memory + minX * buffer->BytesPerPixel + minY * buffer->RowStride;
    for (i32 y = minY; y < maxY; y++)
    {
        u32* pixel = (u32*)row;
        for (i32 x = minX; x < maxX; x++)
        {
            *pixel++ = color;
        }
        row += buffer->RowStride;
    }
}
