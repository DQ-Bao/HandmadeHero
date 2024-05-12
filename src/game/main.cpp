#include "main.h"
#include "utils/image.h"

internal void InitializeArena(MemoryArena* arena, memidx size, u8* base)
{
    arena->Size = size;
    arena->Base = base;
    arena->Used = 0;
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

        tileMap->ChunkShift = 4;
        tileMap->ChunkMask = (1 << tileMap->ChunkShift) - 1;
        tileMap->ChunkSize = (1 << tileMap->ChunkShift);
        tileMap->TileWidthMeter = 1.4f;
        tileMap->TileHeightMeter = 1.4f;
        tileMap->ChunkCountY = 128;
        tileMap->ChunkCountX = 128;
        tileMap->TileChunks = PushArray(&state->WorldArena, 
                                        tileMap->ChunkCountX * tileMap->ChunkCountY, 
                                        TileChunk);
        u32 screenX = 0;
        u32 screenY = 0;
        bool doorLeft = false;
        bool doorRight = false;
        bool doorTop = false;
        bool doorBottom = false;
        for (u32 screenIdx = 0; screenIdx < 100; screenIdx++)
        {
            u32 randomChoice = (screenIdx % 7) % 2;
            if (randomChoice == 0)
            {
                doorRight = true;
            }
            else
            {
                doorTop = true;
            }
            for (u32 row = 0; row < 9; row++)
            {
                for (u32 col = 0; col < 16; col++)
                {
                    u32 x = screenX * 16 + col;
                    u32 y = screenY * 9 + row;

                    u32 tileValue = TILE_EMPTY;
                    if ((col == 0 && (!doorLeft || row != 4)) ||
                        (col == 15 && (!doorRight || row != 4)) ||
                        (row == 0 && (!doorBottom || col != 8)) ||
                        (row == 8 && (!doorTop || col != 8)))
                    {
                        tileValue = TILE_WALL;
                    }

                    SetTileValue(&state->WorldArena, world->TileMap, x, y,
                                 tileValue);
                }
            }
            doorLeft = doorRight;
            doorBottom = doorTop;
            doorRight = false;
            doorTop = false;
            if (randomChoice == 0)
            {
                screenX++;
            }
            else
            {
                screenY++;
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
        char* filename = "assets/bitmap.bmp";
        state->Image = memory->PlatformLoadFile(filename);
    }
}

extern "C" DLL_EXPORT POUND_GAME_SHUTDOWN(GameShutdown)
{
    GameState* state = (GameState*)memory->PermanentMemory;
    memory->PlatformFreeFileMemory(state->Image.Data);
}

extern "C" DLL_EXPORT POUND_GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    GameState* state = (GameState*)memory->PermanentMemory;
    ProcessInput(input, state);
    GameOutputSound(output, soundUpdate);

    BmpHeader* header = (BmpHeader*)state->Image.Data;
    u8* imageData = (u8*)state->Image.Data + header->DataOffset;
    u32 imageBytesPerPixel = header->BitsPerPixel / 8;
    u32 imageColorMask = (1 << header->BitsPerPixel) - 1;

    TileMap* map = state->World->TileMap;
    
    f32 tileWidthPixel = 60.0f;
    f32 tileHeightPixel = 60.0f;
    f32 widthMeterToPixel = tileWidthPixel / map->TileWidthMeter;
    f32 heightMeterToPixel = tileHeightPixel / map->TileHeightMeter;

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

    RenderRectangle(buffer, 0.0f, 0.0f, (f32)buffer->Width, (f32)buffer->Height, 1.0f, 0.0f, 0.0f);
    RenderBitmap(buffer, imageData, imageBytesPerPixel, imageColorMask,
                 0.0f, 0.0f, (f32)header->BitmapWidth, (f32)header->BitmapHeight);

    f32 centerX = buffer->Width * 0.5f;
    f32 centerY = buffer->Height * 0.5f;

    for (i32 relRow = -10; relRow < 10; relRow++)
    {
        for (i32 relCol = -20; relCol < 20; relCol++)
        {
            u32 col = state->PlayerPosition.TileX + relCol;
            u32 row = state->PlayerPosition.TileY + relRow;
            f32 gray = 0.5f;
            u32 tileValue = TILE_INVALID;
            ///\TODO Redo get tileValue because this is inefficient
            TileChunkPosition chunkPos = GetChunkPosition(map, col, row);
            TileChunk* chunk = GetChunk(map, chunkPos.ChunkX, chunkPos.ChunkY);
            if (chunk)
            {
                tileValue = GetTileValue(map, chunk, chunkPos.ChunkTileX, chunkPos.ChunkTileY);
            }
            if (tileValue > TILE_INVALID)
            {
                if (tileValue == TILE_WALL)
                {
                    gray = 1.0f;
                }
                if (col == state->PlayerPosition.TileX && row == state->PlayerPosition.TileY)
                {
                    gray = 0.0f;
                }
                f32 minX = centerX - state->PlayerPosition.TileMeterX * widthMeterToPixel + (f32)relCol * tileWidthPixel;
                f32 maxY = centerY + state->PlayerPosition.TileMeterY * heightMeterToPixel - (f32)relRow * tileHeightPixel;
                f32 maxX = minX + tileWidthPixel;
                f32 minY = maxY - tileHeightPixel;
                RenderRectangle(buffer, minX, minY, maxX, maxY, gray, gray, gray);
            }
        }
    }

    f32 playerR = 0.5f;
    f32 playerG = 0.9f;
    f32 playerB = 0.2f;
    f32 playerWidthPixel = playerWidth * widthMeterToPixel;
    f32 playerHeightPixel = playerHeight * heightMeterToPixel;
    f32 playerLeft = centerX - (0.5f * playerWidthPixel);
    f32 playerTop = centerY - playerHeightPixel;
    RenderRectangle(buffer, playerLeft, playerTop, playerLeft + playerWidthPixel, playerTop + playerHeightPixel, 
                    playerR, playerG, playerB);
}
