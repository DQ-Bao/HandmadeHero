#include "main.h"
#include "intrinsics.h"

extern "C" DLL_EXPORT POUND_GAME_STARTUP(GameStartUp)
{
    GameState* state = (GameState*)memory->PermanentMemory;
    if (!memory->IsInitialized)
    {
        state->PlayerPosition.WorldTileMapX = 0;
        state->PlayerPosition.WorldTileMapY = 0;
        state->PlayerPosition.TileMapTileX = 1;
        state->PlayerPosition.TileMapTileY = 4;
        state->PlayerPosition.TileMeterX = 0.7f;
        state->PlayerPosition.TileMeterY = 0.7f;
        state->PlayerSpeed = 2.0f;
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

inline u32 GetTileValue(World* world, TileMap* map, i32 col, i32 row)
{
    i32 idx = row * world->TileMapColCount + col;
    i32 size = world->TileMapRowCount * world->TileMapColCount;
    if ((idx < 0) || (idx >= size))
    {
        return INVALID_TILE_VALUE;
    }
    return map->Tiles[idx];
}

inline TileMap* GetTileMap(World* world, i32 col, i32 row)
{
    i32 idx = row * world->ColCount + col;
    i32 size = world->RowCount * world->ColCount;
    if ((idx < 0) || (idx >= size))
    {
        return 0;
    }
    return &world->TileMaps[idx];
}

internal void GetLegitPosition(World* world, Position* pos)
{
    i32 tileXOffset = FloorF32ToI32(pos->TileMeterX / world->TileWidthMeter);
    i32 tileYOffset = FloorF32ToI32(pos->TileMeterY / world->TileHeightMeter);
    pos->TileMapTileX += tileXOffset;
    pos->TileMapTileY += tileYOffset;
    pos->TileMeterX -= tileXOffset * world->TileWidthMeter;
    pos->TileMeterY -= tileYOffset * world->TileHeightMeter;
    Assert(pos->TileMeterX >= 0 && pos->TileMeterX < world->TileWidthMeter);
    Assert(pos->TileMeterY >= 0 && pos->TileMeterY < world->TileHeightMeter);
    if (pos->TileMapTileX < 0)
    {
        pos->TileMapTileX = world->TileMapColCount + pos->TileMapTileX;
        pos->WorldTileMapX--;
    }
    else if (pos->TileMapTileX >= world->TileMapColCount)
    {
        pos->TileMapTileX = pos->TileMapTileX - world->TileMapColCount;
        pos->WorldTileMapX++;
    }
    if (pos->TileMapTileY < 0)
    {
        pos->TileMapTileY = world->TileMapRowCount + pos->TileMapTileY;
        pos->WorldTileMapY--;
    }
    else if (pos->TileMapTileY >= world->TileMapRowCount)
    {
        pos->TileMapTileY = pos->TileMapTileY - world->TileMapRowCount;
        pos->WorldTileMapY++;
    }
}

internal bool IsWorldPointEmpty(World* world, Position* pos)
{
    bool empty = false;
    
    GetLegitPosition(world, pos);

    TileMap* map = GetTileMap(world, pos->WorldTileMapX, pos->WorldTileMapY);
    if (map)
    {
        if ((pos->TileMapTileX >= 0) && (pos->TileMapTileX < world->TileMapColCount) 
            && (pos->TileMapTileY >= 0) && (pos->TileMapTileY < world->TileMapRowCount))
        {
            empty = GetTileValue(world, map, pos->TileMapTileX, pos->TileMapTileY) == 0;
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
        { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
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

    World world = {};
    world.TileWidthMeter = 1.4f;
    world.TileHeightMeter = 1.4f;
    world.Left = 0.0f;
    world.Bottom = (f32)(buffer->Height - 1);
    world.TileWidthPixel = 60.0f;
    world.TileHeightPixel = 60.0f;
    world.TileMapRowCount = 9;
    world.TileMapColCount = 16;
    world.RowCount = 2;
    world.ColCount = 2;
    world.WidthMeterToPixel = world.TileWidthPixel / world.TileWidthMeter;
    world.HeightMeterToPixel = world.TileHeightPixel / world.TileHeightMeter;
    world.TileMaps = (TileMap*)maps;

    f32 playerR = 0.5f;
    f32 playerG = 0.9f;
    f32 playerB = 0.2f;
    f32 playerHeight = 1.4f;
    f32 playerWidth = 0.75f * playerHeight;

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
        directionY = -1.0f;
    }
    if (input->KeyW)
    {
        directionY = 1.0f;
    }
    if (directionX != 0 || directionY != 0)
    {
        f32 newPlayerX = state->PlayerPosition.TileMeterX + directionX * state->PlayerSpeed * input->DeltaTime;
        f32 newPlayerY = state->PlayerPosition.TileMeterY + directionY * state->PlayerSpeed * input->DeltaTime;
        Position playerPos = state->PlayerPosition;
        playerPos.TileMeterX = newPlayerX;
        playerPos.TileMeterY = newPlayerY;
        Position playerLeft = playerPos;
        playerLeft.TileMeterX -= 0.5f * playerWidth;
        Position playerRight = playerPos;
        playerRight.TileMeterX += 0.5f * playerWidth;
        if (IsWorldPointEmpty(&world, &playerPos) &&
            IsWorldPointEmpty(&world, &playerLeft) &&
            IsWorldPointEmpty(&world, &playerRight))
        {
            state->PlayerPosition = playerPos;
        }
    }

    TileMap* tileMap = GetTileMap(&world, state->PlayerPosition.WorldTileMapX, state->PlayerPosition.WorldTileMapY);
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
            if (col == state->PlayerPosition.TileMapTileX && row == state->PlayerPosition.TileMapTileY)
            {
                gray = 0.0f;
            }
            f32 minX = world.Left + (f32)col * world.TileWidthPixel;
            f32 maxY = world.Bottom - (f32)row * world.TileHeightPixel;
            f32 maxX = minX + world.TileWidthPixel;
            f32 minY = maxY - world.TileHeightPixel;
            RenderRectangle(buffer, minX, minY, maxX, maxY, gray, gray, gray);
        }
    }
    f32 playerWidthPixel = playerWidth * world.WidthMeterToPixel;
    f32 playerHeightPixel = playerHeight * world.HeightMeterToPixel;
    f32 playerLeft = world.Left + (state->PlayerPosition.TileMapTileX * world.TileWidthPixel + state->PlayerPosition.TileMeterX * world.WidthMeterToPixel) - (0.5f * playerWidthPixel);
    f32 playerTop = world.Bottom - (state->PlayerPosition.TileMapTileY * world.TileHeightPixel + state->PlayerPosition.TileMeterY * world.HeightMeterToPixel) - playerHeightPixel;
    RenderRectangle(buffer, playerLeft, playerTop, playerLeft + playerWidthPixel, playerTop + playerHeightPixel, 
                    playerR, playerG, playerB);
}
