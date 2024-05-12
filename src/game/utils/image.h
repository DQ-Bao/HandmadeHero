#pragma once
#include "platform.h"

#pragma pack(push, 1)
struct BmpHeader
{
	u16 Signature;
	u32 FileSize;
	u32 Reserved;
	u32 DataOffset;
	u32 InfoHeaderSize;
	u32 BitmapWidth;
	u32 BitmapHeight;
	u16 Planes;
	u16 BitsPerPixel;
	u32 Compression;
	u32 CompressedImageSize;
	u32 PixelPerMeterX;
	u32 PixelPerMeterY;
	u32 ColorUsed;
	u32 ImportantColorCount;
};
#pragma pack(pop)
