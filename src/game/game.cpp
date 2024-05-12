#include "game.h"
#include "intrinsics.h"

void GetLegitPosition(TileMap* map, TileMapPosition* pos)
{
    i32 tileXOffset = FloorF32ToI32(pos->TileMeterX / map->TileWidthMeter);
    i32 tileYOffset = FloorF32ToI32(pos->TileMeterY / map->TileHeightMeter);
    pos->TileX += tileXOffset;
    pos->TileY += tileYOffset;
    pos->TileMeterX -= tileXOffset * map->TileWidthMeter;
    pos->TileMeterY -= tileYOffset * map->TileHeightMeter;
    Assert(pos->TileMeterX >= 0 && pos->TileMeterX < map->TileWidthMeter);
    Assert(pos->TileMeterY >= 0 && pos->TileMeterY < map->TileHeightMeter);
}

bool IsTileMapPointEmpty(TileMap* map, TileMapPosition* pos)
{
    bool empty = false;

    ///\TODO Move this call out of this function because it change the parameter pos
    GetLegitPosition(map, pos);

    TileChunkPosition chunkPos = GetChunkPosition(map, pos->TileX, pos->TileY);
    TileChunk* chunk = GetChunk(map, chunkPos.ChunkX, chunkPos.ChunkY);
    if (chunk)
    {
        empty = GetTileValue(map, chunk, chunkPos.ChunkTileX, chunkPos.ChunkTileY) == TILE_EMPTY;
    }
    return empty;
}

void SetTileValue(MemoryArena* arena, TileMap* map, u32 col, u32 row, u32 value)
{
    TileChunkPosition chunkPos = GetChunkPosition(map, col, row);
    TileChunk* chunk = GetChunk(map, chunkPos.ChunkX, chunkPos.ChunkY);
    Assert(chunk);
    if (!chunk || !chunk->Tiles)
    {
        u32 chunkIdx = row * map->ChunkCountX + col;
        u32 tileCount = map->ChunkSize * map->ChunkSize;
        chunk->Tiles = PushArray(arena, tileCount, u32);
        for (u32 i = 0; i < tileCount; i++)
        {
            chunk->Tiles[i] = TILE_EMPTY;
        }
    }
    chunk->Tiles[chunkPos.ChunkTileY * map->ChunkSize + chunkPos.ChunkTileX] = value;
}
