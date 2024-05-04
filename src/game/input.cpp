#include "input.h"

void ProcessInput(GameInput* input, RenderUpdate* render, SoundUpdate* sound)
{
	// buggy
	/*render->XOffset = input->MousePositionX;
	render->YOffset = input->MousePositionY;*/
	for (u32 i = 0; i < 15; i++)
	{
		if (input->Keys[i])
		{
			/*if (i == 0)
			{
				sound->VolumeChange = 1;
			}*/
			if (i == 1)
			{
				sound->FrequencyChange = -1;
			}
			/*else if (i == 2)
			{
				sound->VolumeChange = -1;
			}*/
			else if (i == 3)
			{
				sound->FrequencyChange = 1;
			}
		}
	}
}