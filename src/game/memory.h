#pragma once
#include "platform.h"

struct File
{
	u32 Size;
	void* Data;
};

#define POUND_PLATFORM_LOAD_FILE(name) File name(char* fileName)
typedef POUND_PLATFORM_LOAD_FILE(PlatformLoadFile_t);
#define POUND_PLATFORM_FREE_FILE_MEMORY(name) void name(void* memory)
typedef POUND_PLATFORM_FREE_FILE_MEMORY(PlatformFreeFileMemory_t);

struct GameMemory
{
	bool IsInitialized;
	u64 PermanentSize;
	void* PermanentMemory; ///\ATTENTION NEED to init memory to 0 at startup
	u64 TransientSize;
	void* TransientMemory; ///\ATTENTION NEED to init memory to 0 at startup
	PlatformFreeFileMemory_t* PlatformFreeFileMemory;
	PlatformLoadFile_t* PlatformLoadFile;
};

struct MemoryArena
{
	memidx Size;
	u8* Base;
	memidx Used;
};

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
