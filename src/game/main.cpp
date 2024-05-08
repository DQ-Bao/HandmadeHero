#include "main.h"

internal void InitializeArena(MemoryArena* arena, memidx size, u8* base)
{
    arena->Size = size;
    arena->Base = base;
    arena->Used = 0;
}

#define PushStruct(arena, type) (type*)PushSize_(arena, sizeof(type))
internal void* PushSize_(MemoryArena* arena, memidx size)
{
    Assert((arena->Used + size) <= arena->Size);
    void* result = arena->Base + arena->Used;
    arena->Used += size;
    return result;
}

#define PushArray(arena, count, type) (type*)PushArray_(arena, count, sizeof(type))
internal void* PushArray_(MemoryArena* arena, u32 count, memidx size)
{
    Assert((arena->Used + size * count) <= arena->Size);
    void* result = arena->Base + arena->Used;
    arena->Used += size * count;
    return result;
}

extern "C" DLL_EXPORT POUND_GAME_STARTUP(GameStartUp)
{
    GameState* state = (GameState*)memory->PermanentMemory;
    if (!memory->IsInitialized)
    {
        InitializeArena(&state->WorldArena, 
                        memory->PermanentSize - sizeof(GameState), 
                        (u8*)memory->PermanentMemory + sizeof(GameState));

        state->World = PushStruct(&state->WorldArena, World);
        World* world = state->World;
        world->TileMap = PushStruct(&state->WorldArena, TileMap);
        TileMap* tileMap = world->TileMap;

        tileMap->ChunkShift = 8;
        tileMap->ChunkMask = (1 << tileMap->ChunkShift) - 1;
        tileMap->ChunkSize = (1 << tileMap->ChunkShift);
        tileMap->TileWidthMeter = 1.4f;
        tileMap->TileHeightMeter = 1.4f;
        tileMap->TileWidthPixel = 60.0f;
        tileMap->TileHeightPixel = 60.0f;
        tileMap->ChunkCountY = 4;
        tileMap->ChunkCountX = 4;
        tileMap->TileChunks = PushArray(&state->WorldArena, 
                                        tileMap->ChunkCountX * tileMap->ChunkCountY, 
                                        TileChunk);

        for (u32 row = 0; row < tileMap->ChunkCountY; row++)
        {
            for (u32 col = 0; col < tileMap->ChunkCountX; col++)
            {
                tileMap->TileChunks[row * tileMap->ChunkCountX + col]
                    .Tiles = PushArray(&state->WorldArena, 
                                        tileMap->ChunkSize * tileMap->ChunkSize, 
                                        u32);
            }
        }

        tileMap->WidthMeterToPixel = tileMap->TileWidthPixel / tileMap->TileWidthMeter;
        tileMap->HeightMeterToPixel = tileMap->TileHeightPixel / tileMap->TileHeightMeter;

        for (u32 screenY = 0; screenY < 32; screenY++)
        {
            for (u32 screenX = 0; screenX < 32; screenX++)
            {
                for (u32 row = 0; row < 9; row++)
                {
                    for (u32 col = 0; col < 16; col++)
                    {
                        u32 x = screenX * 16 + col;
                        u32 y = screenY * 9 + row;
                        SetTileValue(world->TileMap, x, y, 
                                     (row == col) && (row % 2) ? 1 : 0);
                    }
                }
            }
        }

        state->PlayerPosition.TileX = 1;
        state->PlayerPosition.TileY = 4;
        state->PlayerPosition.TileMeterX = 0.7f;
        state->PlayerPosition.TileMeterY = 0.7f;
        state->PlayerSpeed = 2.0f;
        state->Render.XOffset = 0;
        state->Render.YOffset = 0;
        state->Sound.FrequencyChange = 0;
        state->Sound.VolumeChange = 0;

        memory->IsInitialized = true;

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

extern "C" DLL_EXPORT POUND_GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    GameState* state = (GameState*)memory->PermanentMemory;
    ProcessInput(input, state);
    GameOutputSound(output, soundUpdate);

    TileMap* map = state->World->TileMap;
    
    f32 worldLeft = 0.0f;
    f32 worldBottom = (f32)(buffer->Height - 1);

    f32 playerHeight = 1.4f;
    f32 playerWidth = 0.75f * playerHeight;

    f32 directionX = 0.0f;
    f32 directionY = 0.0f;
    if (input->KeyShift)
    {
        state->PlayerSpeed = 6.0f;
    }
    else
    {
        state->PlayerSpeed = 2.0f;
    }
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
        TileMapPosition playerPos = state->PlayerPosition;
        playerPos.TileMeterX = newPlayerX;
        playerPos.TileMeterY = newPlayerY;
        TileMapPosition playerLeft = playerPos;
        playerLeft.TileMeterX -= 0.5f * playerWidth;
        TileMapPosition playerRight = playerPos;
        playerRight.TileMeterX += 0.5f * playerWidth;
        if (IsTileMapPointEmpty(map, &playerPos) &&
            IsTileMapPointEmpty(map, &playerLeft) &&
            IsTileMapPointEmpty(map, &playerRight))
        {
            state->PlayerPosition = playerPos;
        }
    }

    f32 centerX = buffer->Width * 0.5f;
    f32 centerY = buffer->Height * 0.5f;

    for (i32 relRow = -6; relRow < 6; relRow++)
    {
        for (i32 relCol = -9; relCol < 9; relCol++)
        {
            u32 col = state->PlayerPosition.TileX + relCol;
            u32 row = state->PlayerPosition.TileY + relRow;
            f32 gray = 0.5f;
            ///\TODO Redo get tileValue because this is inefficient
            TileChunkPosition chunkPos = GetChunkPosition(map, col, row);
            TileChunk* chunk = GetChunk(map, chunkPos.ChunkX, chunkPos.ChunkY);
            if (chunk)
            {
                u32 tileValue = GetTileValue(map, chunk, chunkPos.ChunkTileX, chunkPos.ChunkTileY);
                if (tileValue == 1)
                {
                    gray = 1.0f;
                }
            }
            if (col == state->PlayerPosition.TileX && row == state->PlayerPosition.TileY)
            {
                gray = 0.0f;
            }
            f32 minX = centerX - state->PlayerPosition.TileMeterX * map->WidthMeterToPixel + (f32)relCol * map->TileWidthPixel;
            f32 maxY = centerY + state->PlayerPosition.TileMeterY * map->HeightMeterToPixel - (f32)relRow * map->TileHeightPixel;
            f32 maxX = minX + map->TileWidthPixel;
            f32 minY = maxY - map->TileHeightPixel;
            RenderRectangle(buffer, minX, minY, maxX, maxY, gray, gray, gray);
        }
    }

    f32 playerR = 0.5f;
    f32 playerG = 0.9f;
    f32 playerB = 0.2f;
    f32 playerWidthPixel = playerWidth * map->WidthMeterToPixel;
    f32 playerHeightPixel = playerHeight * map->HeightMeterToPixel;
    f32 playerLeft = centerX - (0.5f * playerWidthPixel);
    f32 playerTop = centerY - playerHeightPixel;
    RenderRectangle(buffer, playerLeft, playerTop, playerLeft + playerWidthPixel, playerTop + playerHeightPixel, 
                    playerR, playerG, playerB);
}
