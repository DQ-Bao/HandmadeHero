#pragma once
#include "platform.h"
#include "render.h"
#include "sound.h"
#include "input.h"

#define POUND_PLATFORM_LOAD_FILE(name) void* name(char* fileName)
typedef POUND_PLATFORM_LOAD_FILE(PlatformLoadFile_t);
#define POUND_PLATFORM_FREE_FILE_MEMORY(name) void name(void* memory)
typedef POUND_PLATFORM_FREE_FILE_MEMORY(PlatformFreeFileMemory_t);

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
};

#define POUND_GAME_UPDATE_AND_RENDER(name) void name(GameMemory* memory, GameBackBuffer* buffer, GameSoundOutput* output, GameInput* input, RenderUpdate* renderUpdate, SoundUpdate* soundUpdate)
typedef POUND_GAME_UPDATE_AND_RENDER(GameUpdateAndRender_t);
POUND_GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub) {}

#define POUND_GAME_STARTUP(name) void name(GameMemory* memory, GameSoundOutput* sound)
typedef POUND_GAME_STARTUP(GameStartUp_t);
POUND_GAME_STARTUP(GameStartUpStub) {}

#define POUND_GAME_SHUTDOWN(name) void name()
typedef POUND_GAME_SHUTDOWN(GameShutdown_t);
POUND_GAME_SHUTDOWN(GameShutdownStub) {}

