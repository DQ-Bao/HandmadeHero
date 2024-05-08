#pragma once
#include "platform.h"

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
	f32 WidthMeterToPixel;
	f32 HeightMeterToPixel;
	f32 TileWidthPixel;
	f32 TileHeightPixel;
	u32 ChunkCountY;
	u32 ChunkCountX;
	TileChunk* TileChunks;
};

struct World
{
	TileMap* TileMap;
};

inline u32 GetTileValue(TileMap* map, TileChunk* chunk, u32 col, u32 row)
{
	Assert(chunk);
	Assert(col < map->ChunkSize);
	Assert(row < map->ChunkSize);
	i32 idx = row * map->ChunkSize + col;
	return chunk->Tiles[idx];
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
void SetTileValue(TileMap* map, u32 col, u32 row, u32 value);
