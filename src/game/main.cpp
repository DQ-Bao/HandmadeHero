#include "main.h"

extern "C" DLL_EXPORT POUND_GAME_STARTUP(GameStartUp)
{
    GameState* state = (GameState*)memory->PermanentMemory;
    if (!memory->IsInitialized)
    {
        state->PlayerTileMapX = 0;
        state->PlayerTileMapY = 0;
        state->PlayerX = 90.0f;
        state->PlayerY = 270.0f;
        state->Render.XOffset = 0;
        state->Render.YOffset = 0;
        state->Sound.FrequencyChange = 0;
        state->Sound.VolumeChange = 0;

        /*char* fileName = "assets/nanamiring.wav";
        WAVEFile* wav = (WAVEFile*)memory->PlatformLoadFile(fileName);
        sound->AudioFormat = wav->AudioFormat;
        sound->NumberOfChannels = wav->NumberOfChannels;
        sound->SampleRate = wav->SampleRate;
        sound->BitsPerSample = wav->BitsPerSample;
        sound->Size = wav->DataSize;
        sound->Samples = (u8*)wav + 44;*/
    }
}

extern "C" DLL_EXPORT POUND_GAME_SHUTDOWN(GameShutdown)
{

}

#include <math.h>
inline i32 FloorF32ToI32(f32 f)
{
    return (i32)floorf(f);
}

inline u32 GetTileValue(WorldMap* world, TileMap* map, i32 col, i32 row)
{
    i32 idx = row * world->TileMapColCount + col;
    i32 size = world->TileMapRowCount * world->TileMapColCount;
    if ((idx < 0) || (idx >= size))
    {
        return INVALID_TILE_VALUE;
    }
    return map->Tiles[idx];
}

inline TileMap* GetTileMap(WorldMap* world, i32 col, i32 row)
{
    i32 idx = row * world->ColCount + col;
    i32 size = world->RowCount * world->ColCount;
    if ((idx < 0) || (idx >= size))
    {
        return 0;
    }
    return &world->TileMaps[idx];
}

internal Position GetRealPosition(WorldMap* world, 
                                  i32 worldTileMapX, i32 worldTileMapY, 
                                  f32 worldPixelX, f32 worldPixelY) 
{
    Position pos = {};
    pos.WorldPixelX = worldPixelX;
    pos.WorldPixelY = worldPixelY;
    pos.WorldTileMapX = worldTileMapX;
    pos.WorldTileMapY = worldTileMapY;
    pos.TileMapPixelX = pos.WorldPixelX - world->Left;
    pos.TileMapPixelY = pos.WorldPixelY - world->Top;
    pos.TileMapTileX = FloorF32ToI32(pos.TileMapPixelX / world->TileWidth);
    pos.TileMapTileY = FloorF32ToI32(pos.TileMapPixelY / world->TileHeight);
    pos.TilePixelX = pos.TileMapPixelX - pos.TileMapTileX * world->TileWidth;
    pos.TilePixelY = pos.TileMapPixelY - pos.TileMapTileY * world->TileHeight;
    if (pos.TileMapTileX < 0)
    {
        pos.TileMapTileX = world->TileMapColCount + pos.TileMapTileX;
        pos.WorldPixelX = ((f32)pos.TileMapTileX * world->TileWidth) + world->Left + pos.TilePixelX;
        pos.WorldTileMapX--;
    }
    if (pos.TileMapTileX >= world->TileMapColCount)
    {
        pos.TileMapTileX = pos.TileMapTileX - world->TileMapColCount;
        pos.WorldPixelX = ((f32)pos.TileMapTileX * world->TileWidth) + world->Left + pos.TilePixelX;
        pos.WorldTileMapX++;
    }
    if (pos.TileMapTileY < 0)
    {
        pos.TileMapTileY = world->TileMapRowCount + pos.TileMapTileY;
        pos.WorldPixelY = ((f32)pos.TileMapTileY * world->TileHeight) + world->Top + pos.TilePixelY;
        pos.WorldTileMapY--;
    }
    if (pos.TileMapTileY >= world->TileMapRowCount)
    {
        pos.TileMapTileY = pos.TileMapTileY - world->TileMapRowCount;
        pos.WorldPixelY = ((f32)pos.TileMapTileY * world->TileHeight) + world->Top + pos.TilePixelY;
        pos.WorldTileMapY++;
    }
    return pos;
}

internal bool IsWorldPointEmpty(WorldMap* world, i32 mapX, i32 mapY, f32 testX, f32 testY)
{
    bool empty = false;
    
    Position pos = GetRealPosition(world, mapX, mapY, testX, testY);

    TileMap* map = GetTileMap(world, pos.WorldTileMapX, pos.WorldTileMapY);
    if (map)
    {
        if ((pos.TileMapTileX >= 0) && (pos.TileMapTileX < world->TileMapColCount) 
            && (pos.TileMapTileY >= 0) && (pos.TileMapTileY < world->TileMapRowCount))
        {
            empty = GetTileValue(world, map, pos.TileMapTileX, pos.TileMapTileY) == 0;
        }
    }
    return empty;
}

extern "C" DLL_EXPORT POUND_GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    GameState* state = (GameState*)memory->PermanentMemory;
    ProcessInput(input, state);
    GameOutputSound(output, soundUpdate);
    u32 tiles00[9][16] = {
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1 },
        { 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1 },
        { 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1 },
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1 },
        { 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1 },
        { 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
    };
    u32 tiles10[9][16] = {
        { 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1 },
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    };
    u32 tiles11[9][16] = {
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1 },
        { 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1 },
        { 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1 },
        { 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1 },
        { 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1 },
        { 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    };
    u32 tiles01[9][16] = {
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1 },
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1 },
        { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1 },
        { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1 },
        { 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1 },
    };

    TileMap maps[2][2];

    maps[0][0].Tiles = (u32*)tiles00;
    maps[0][1].Tiles = (u32*)tiles01;
    maps[1][1].Tiles = (u32*)tiles11;
    maps[1][0].Tiles = (u32*)tiles10;

    WorldMap world = {};
    world.Left = 0.0f;
    world.Top = 0.0f;
    world.TileWidth = 60;
    world.TileHeight = 60;
    world.TileMapRowCount = 9;
    world.TileMapColCount = 16;
    world.RowCount = 2;
    world.ColCount = 2;
    world.TileMaps = (TileMap*)maps;

    f32 playerR = 0.5f;
    f32 playerG = 0.9f;
    f32 playerB = 0.2f;
    f32 playerWidth = 0.75f * world.TileWidth;
    f32 playerHeight = world.TileHeight;

    f32 directionX = 0.0f;
    f32 directionY = 0.0f;
    if (input->KeyA)
    {
        directionX = -1.0f;
    }
    if (input->KeyD)
    {
        directionX = 1.0f;
    }
    if (input->KeyS)
    {
        directionY = 1.0f;
    }
    if (input->KeyW)
    {
        directionY = -1.0f;
    }
    if (directionX != 0 || directionY != 0)
    {
        f32 speed = 128.0f;
        f32 newPlayerX = state->PlayerX + directionX * speed * input->DeltaTime;
        f32 newPlayerY = state->PlayerY + directionY * speed * input->DeltaTime;
        if (IsWorldPointEmpty(&world, state->PlayerTileMapX, state->PlayerTileMapY, newPlayerX, newPlayerY) &&
            IsWorldPointEmpty(&world, state->PlayerTileMapX, state->PlayerTileMapY, newPlayerX + 0.5f * playerWidth, newPlayerY) &&
            IsWorldPointEmpty(&world, state->PlayerTileMapX, state->PlayerTileMapY, newPlayerX - 0.5f * playerWidth, newPlayerY))
        {
            Position pos = GetRealPosition(&world, state->PlayerTileMapX, state->PlayerTileMapY, newPlayerX, newPlayerY);
            state->PlayerX = pos.WorldPixelX;
            state->PlayerY = pos.WorldPixelY;
            state->PlayerTileMapX = pos.WorldTileMapX;
            state->PlayerTileMapY = pos.WorldTileMapY;
        }
    }

    TileMap* tileMap = GetTileMap(&world, state->PlayerTileMapX, state->PlayerTileMapY);
    Assert(tileMap);

    for (i32 row = 0; row < world.TileMapRowCount; row++)
    {
        for (i32 col = 0; col < world.TileMapColCount; col++)
        {
            f32 gray = 0.5f;
            u32 tileValue = GetTileValue(&world, tileMap, col, row);
            if (tileValue == 1)
            {
                gray = 1.0f;
            }
            f32 minX = world.Left + col * world.TileWidth;
            f32 minY = world.Top + row * world.TileHeight;
            f32 maxX = minX + world.TileWidth;
            f32 maxY = minY + world.TileHeight;
            RenderRectangle(buffer, minX, minY, maxX, maxY, gray, gray, gray);
        }
    }
    
    f32 playerLeft = state->PlayerX - 0.5f * playerWidth;
    f32 playerTop = state->PlayerY - playerHeight;
    RenderRectangle(buffer, playerLeft, playerTop, playerLeft + playerWidth, playerTop + playerHeight, 
                    playerR, playerG, playerB);
}
