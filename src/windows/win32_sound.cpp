#include <Windows.h>
#include <xaudio2.h>
#include <math.h>
#include "win32_sound.h"

HANDLE bufferEndEvent;
IXAudio2SourceVoice* sourceVoice;
Win32VoiceCallback voiceCb;
void Win32InitXAudio2(Win32SourceVoiceSettings* settings)
{
    if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED)))
    {
        OutputDebugStringA("Failed to init COM\n");
        return;
    }
    IXAudio2* XAudio2Instance;
    if (FAILED(XAudio2Create(&XAudio2Instance, 0, XAUDIO2_DEFAULT_PROCESSOR)))
    {
        OutputDebugStringA("Failed to init XAudio2 engine\n");
        CoUninitialize();
        return;
    }

#ifdef HM_DEBUG
    XAUDIO2_DEBUG_CONFIGURATION debug = {};
    debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
    debug.BreakMask = XAUDIO2_LOG_ERRORS;
    debug.LogFileline = 1;
    debug.LogFunctionName = 1;
    debug.LogTiming = 1;
    XAudio2Instance->SetDebugConfiguration(&debug, 0);
#endif

    IXAudio2MasteringVoice* masterVoice;
    if (FAILED(XAudio2Instance->CreateMasteringVoice(&masterVoice)))
    {
        OutputDebugStringA("Failed to create mastering voice\n");
        XAudio2Instance->Release();
        CoUninitialize();
        return;
    }
    WAVEFORMATEX waveFormat = {};
    waveFormat.wFormatTag = settings->WaveFormat;
    waveFormat.nChannels = settings->Channels;
    waveFormat.nSamplesPerSec = settings->SampleRate;
    waveFormat.wBitsPerSample = settings->BitsPerSample;
    waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
    waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;
    waveFormat.cbSize = 0;

    if (FAILED(XAudio2Instance->CreateSourceVoice(&sourceVoice, &waveFormat, 0, XAUDIO2_MAX_FREQ_RATIO, &voiceCb, 0, 0)))
    {
    }
}

Win32VoiceCallback::Win32VoiceCallback()
{
    bufferEndEvent = CreateEventA(0, FALSE, FALSE, 0);
}

Win32VoiceCallback::~Win32VoiceCallback()
{
    CloseHandle(bufferEndEvent);
}

void Win32VoiceCallback::OnStreamEnd()
{
}

void Win32VoiceCallback::OnVoiceProcessingPassStart(UINT32 BytesRequired)
{
}

void Win32VoiceCallback::OnVoiceProcessingPassEnd()
{
}

void Win32VoiceCallback::OnBufferEnd(void* pBufferContext)
{
    if (!SetEvent(bufferEndEvent))
    {
        OutputDebugStringA("Failed to signal sound buffer end event\n");
    }
}

void Win32VoiceCallback::OnBufferStart(void* pBufferContext)
{
}

void Win32VoiceCallback::OnLoopEnd(void* pBufferContext)
{
}

void Win32VoiceCallback::OnVoiceError(void* pBufferContext, HRESULT Error)
{
}
