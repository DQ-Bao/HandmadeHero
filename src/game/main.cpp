#include "main.h"

extern "C" DLL_EXPORT POUND_GAME_STARTUP(GameStartUp)
{
    GameState* state = (GameState*)memory->PermanentMemory;
    if (!memory->IsInitialized)
    {
        state->Render.XOffset = 0;
        state->Render.YOffset = 0;
        state->Sound.FrequencyChange = 0;
        state->Sound.VolumeChange = 0;

        char* fileName = "assets/nanamiring.wav";
        WAVEFile* wav = (WAVEFile*)memory->PlatformLoadFile(fileName);
        sound->AudioFormat = wav->AudioFormat;
        sound->NumberOfChannels = wav->NumberOfChannels;
        sound->SampleRate = wav->SampleRate;
        sound->BitsPerSample = wav->BitsPerSample;
        sound->Size = wav->DataSize;
        sound->Samples = (u8*)wav + 44;
    }
}

extern "C" DLL_EXPORT POUND_GAME_SHUTDOWN(GameShutdown)
{

}

extern "C" DLL_EXPORT POUND_GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    ProcessInput(input, renderUpdate, soundUpdate);
    GameOutputSound(output, soundUpdate);
    Render(buffer, renderUpdate);
}
