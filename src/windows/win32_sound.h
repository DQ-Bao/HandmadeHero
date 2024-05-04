#pragma once
#include "platform.h"

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

struct Win32SoundUpdate
{
	f32 FrequencyRatio = 1.0f;
	f32 VolumeLevel = 1.0f;
};

class Win32VoiceCallback : public IXAudio2VoiceCallback
{
public:
	Win32VoiceCallback();
	~Win32VoiceCallback();

	void OnStreamEnd() override;
	void OnVoiceProcessingPassStart(UINT32 BytesRequired) override;
	void OnVoiceProcessingPassEnd() override;
	void OnBufferEnd(void* pBufferContext) override;
	void OnBufferStart(void* pBufferContext) override;
	void OnLoopEnd(void* pBufferContext) override;
	void OnVoiceError(void* pBufferContext, HRESULT Error) override;
};

struct Win32SourceVoiceSettings
{
	u16 WaveFormat;
	u16 Channels;
	u32 SampleRate;
	u16 BitsPerSample;
};

extern HANDLE bufferEndEvent;
extern IXAudio2SourceVoice* sourceVoice;
void Win32InitXAudio2(Win32SourceVoiceSettings* settings);
