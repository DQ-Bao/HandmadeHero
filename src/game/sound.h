#pragma once
#include "platform.h"

struct GameSoundOutput
{
	void* Samples = 0;
	u32 Size = 0;
	u32 SampleCount = 0;
	u32 SampleRate = 48000;
	u16 AudioFormat = 1;
	u16 NumberOfChannels = 2;
	u16 BitsPerSample = 16;
};

struct WAVEFile
{
	u32 RIFFId;
	u32 RIFFSize;
	u32 RIFFFormat;
	u32 FmtId;
	u32 FmtSize;
	u16 AudioFormat;
	u16 NumberOfChannels;
	u32 SampleRate;
	u32 ByteRate;
	u16 BlockAlign;
	u16 BitsPerSample;
	u32 DataId;
	u32 DataSize;
	// Pad 4 bytes
	void* Data;
};

struct SoundUpdate
{
	f32 tSine = 0.0f;
	i8 FrequencyChange = 0;
	i8 VolumeChange = 0;
};

void GameOutputSound(GameSoundOutput* output, SoundUpdate* update);
