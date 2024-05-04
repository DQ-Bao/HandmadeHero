#include "sound.h"
#include <math.h>

internal void PlaySineWave(GameSoundOutput* output, SoundUpdate* update)
{
    i16 volume = 3000;
    f32 freq = 256.0f;
    f32 inc = 2.0f * PI32 * freq / (f32)output->SampleRate;
    i16* p = (i16*)output->Samples;
    for (u32 i = 0; i < output->SampleCount; i++)
    {
        i16 sampleValue = (i16)((f32)volume * sinf(update->tSine));

        *p++ = sampleValue;
        *p++ = sampleValue;
        update->tSine += inc;
        if (update->tSine > 2 * PI32)
        {
            update->tSine -= 2 * PI32;
        }
    }
}

void GameOutputSound(GameSoundOutput* output, SoundUpdate* update)
{
}
