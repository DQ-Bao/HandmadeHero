#pragma once
#include "platform.h"
#include "game.h"

struct GameState;

struct KeyState
{
	u32 NoTransitionCount;
	bool Down;
};

struct MouseState
{
	u32 NoTransitionCount;
	bool Down;
};

struct GameInput
{
	union
	{
		bool Keys[15];
		struct
		{
			bool KeyW;
			bool KeyA;
			bool KeyS;
			bool KeyD;
			bool KeyQ;
			bool KeyE;
			bool KeyUp;
			bool KeyDown;
			bool KeyLeft;
			bool KeyRight;
			bool KeySpace;
			bool KeyShift;
			bool KeyCtrl;
			bool KeyEsc;
			bool KeyEnter;
		};
	};
	bool MouseLeft;
	bool MouseRight;
	bool MouseMiddle;
	bool MouseSideX;
	bool MouseSideY;
	i32 MouseScroll;
	i32 MousePositionX;
	i32 MousePositionY;
	f32 DeltaTime;
};

void ProcessInput(GameInput* input, GameState* state);
