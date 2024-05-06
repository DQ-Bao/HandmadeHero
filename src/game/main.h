#pragma once
#include "platform.h"
#include "render.h"
#include "sound.h"
#include "input.h"
#include "game.h"

#define POUND_GAME_UPDATE_AND_RENDER(name) void name(GameMemory* memory, GameBackBuffer* buffer, GameSoundOutput* output, GameInput* input, RenderUpdate* renderUpdate, SoundUpdate* soundUpdate)
typedef POUND_GAME_UPDATE_AND_RENDER(GameUpdateAndRender_t);

#ifndef GAME_UPDATE_AND_RENDER_STUB
#define GAME_UPDATE_AND_RENDER_STUB
POUND_GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub) {}
#endif

#define POUND_GAME_STARTUP(name) void name(GameMemory* memory, GameSoundOutput* sound)
typedef POUND_GAME_STARTUP(GameStartUp_t);

#ifndef GAME_START_UP_STUB
#define GAME_START_UP_STUB
POUND_GAME_STARTUP(GameStartUpStub) {}
#endif

#define POUND_GAME_SHUTDOWN(name) void name()
typedef POUND_GAME_SHUTDOWN(GameShutdown_t);

#ifndef GAME_SHUTDOWN
#define GAME_SHUTDOWN
POUND_GAME_SHUTDOWN(GameShutdownStub) {}
#endif
