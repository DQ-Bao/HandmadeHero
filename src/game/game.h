#pragma once
#include "platform.h"
#include "sound.h"
#include "render.h"
#include "memory.h"
#define TILE_INVALID 0
#define TILE_EMPTY 1
#define TILE_WALL 2

struct TileChunkPosition
{
	u32 ChunkX;
	u32 ChunkY;
	u32 ChunkTileX;
	u32 ChunkTileY;
};

///\NOTE x go right, y go up
struct TileMapPosition
{
	///\NOTE Tile location, the high 24 bits are the chunk index, 
	///		 low 8 bits are tile index in the chunk
	u32 TileX;
	u32 TileY;

	///\NOTE Offset from the bottom left of the tile, in meters
	f32 TileMeterX;
	f32 TileMeterY;
};

struct TileChunk
{
	u32* Tiles;
};

struct TileMap
{
	u32 ChunkShift;
	u32 ChunkMask;
	u32 ChunkSize;
	f32 TileWidthMeter;
	f32 TileHeightMeter;
	u32 ChunkCountY;
	u32 ChunkCountX;
	TileChunk* TileChunks;
};

struct World
{
	TileMap* TileMap;
};

struct GameState
{
	MemoryArena WorldArena;
	World* World;
	RenderUpdate Render;
	SoundUpdate Sound;
	TileMapPosition PlayerPosition;
	f32 PlayerSpeed;
	File Image;
};

inline u32 GetTileValue(TileMap* map, TileChunk* chunk, u32 col, u32 row)
{
	Assert(chunk);
	Assert(col < map->ChunkSize);
	Assert(row < map->ChunkSize);
	u32 value = TILE_INVALID;
	if (chunk && chunk->Tiles)
	{
		i32 idx = row * map->ChunkSize + col;
		value = chunk->Tiles[idx];
	}
	return value;
}

inline TileChunk* GetChunk(TileMap* map, u32 col, u32 row)
{
	u32 idx = row * map->ChunkCountX + col;
	u32 size = map->ChunkCountY * map->ChunkCountX;
	if (idx >= size)
	{
		return 0;
	}
	return &map->TileChunks[idx];
}

inline TileChunkPosition GetChunkPosition(TileMap* map, u32 tileX, u32 tileY)
{
	TileChunkPosition chunk;
	chunk.ChunkX = tileX >> map->ChunkShift;
	chunk.ChunkY = tileY >> map->ChunkShift;
	chunk.ChunkTileX = tileX & map->ChunkMask;
	chunk.ChunkTileY = tileY & map->ChunkMask;
	return chunk;
}

void GetLegitPosition(TileMap* map, TileMapPosition* pos);
bool IsTileMapPointEmpty(TileMap* map, TileMapPosition* pos);
void SetTileValue(MemoryArena* arena, TileMap* map, u32 col, u32 row, u32 value);
