#pragma once
#include "platform.h"
#include "render.h"
#include "sound.h"

#define POUND_PLATFORM_LOAD_FILE(name) void* name(char* fileName)
typedef POUND_PLATFORM_LOAD_FILE(PlatformLoadFile_t);
#define POUND_PLATFORM_FREE_FILE_MEMORY(name) void name(void* memory)
typedef POUND_PLATFORM_FREE_FILE_MEMORY(PlatformFreeFileMemory_t);

#define INVALID_TILE_VALUE 255

struct GameMemory
{
	bool IsInitialized;
	u64 PermanentSize;
	void* PermanentMemory; // NOTE: NEED to init memory to 0 at startup
	u64 TransientSize;
	void* TransientMemory; // NOTE: NEED to init memory to 0 at startup
	PlatformFreeFileMemory_t* PlatformFreeFileMemory;
	PlatformLoadFile_t* PlatformLoadFile;
};

// x go right, y go up
struct Position
{
	i32 WorldTileMapX;
	i32 WorldTileMapY;
	i32 TileMapTileX;
	i32 TileMapTileY;
	f32 TileMeterX;
	f32 TileMeterY;
};

struct TileMap
{
	u32* Tiles;
};

struct World
{
	f32 TileWidthMeter;
	f32 TileHeightMeter;
	f32 WidthMeterToPixel;
	f32 HeightMeterToPixel;
	f32 Left;
	f32 Top;
	f32 Bottom;
	f32 TileWidthPixel;
	f32 TileHeightPixel;
	i32 TileMapRowCount;
	i32 TileMapColCount;
	i32 RowCount;
	i32 ColCount;
	TileMap* TileMaps;
};

struct GameState
{
	RenderUpdate Render;
	SoundUpdate Sound;
	Position PlayerPosition;
	f32 PlayerSpeed;
};
