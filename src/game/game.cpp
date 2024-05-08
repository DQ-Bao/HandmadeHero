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
        empty = GetTileValue(map, chunk, chunkPos.ChunkTileX, chunkPos.ChunkTileY) == 0;
    }
    return empty;
}

void SetTileValue(TileMap* map, u32 col, u32 row, u32 value)
{
    TileChunkPosition chunkPos = GetChunkPosition(map, col, row);
    TileChunk* chunk = GetChunk(map, chunkPos.ChunkX, chunkPos.ChunkY);
    Assert(chunk);
    if (chunk)
    {
        chunk->Tiles[chunkPos.ChunkTileY * map->ChunkSize + chunkPos.ChunkTileX] = value;
    }
}
