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

struct GameState
{
	RenderUpdate Render;
	SoundUpdate Sound;
	i32 PlayerTileMapX;
	i32 PlayerTileMapY;
	f32 PlayerX;
	f32 PlayerY;
};

struct Position
{
	f32 WorldPixelX;
	f32 WorldPixelY;
	i32 WorldTileMapX;
	i32 WorldTileMapY;
	f32 TileMapPixelX;
	f32 TileMapPixelY;
	i32 TileMapTileX;
	i32 TileMapTileY;
	f32 TilePixelX;
	f32 TilePixelY;
};

struct TileMap
{
	u32* Tiles;
};

struct WorldMap
{
	f32 Left;
	f32 Top;
	f32 TileWidth;
	f32 TileHeight;
	i32 TileMapRowCount;
	i32 TileMapColCount;
	i32 RowCount;
	i32 ColCount;
	TileMap* TileMaps;
};
