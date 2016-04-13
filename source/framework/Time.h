#pragma once

#include "Types.h"
#include "sdl/sdl_wrapper.h"

namespace Neutrino
{
	// GetMSSinceStartup() 
	// 		returns the milliseconds passed since Framework init
	float GetMSSinceStartup();

	// GetMSDelta() 
	// 		returns milliseconds passed since last Framework tick
	float GetMSDelta();


	// GetGameMSDelta() 
	// 		returns milliseconds passed since last game tick. 
	// 		Game tick normally matches framework tick, but can be paused, spedup or slowed down
	float GetGameMSDelta();


	// GetGameMSElapsed()
	// 		returns milliseconds elapsed over total duration of game ticks
	// 		Note, if game tick is paused then MS elapsed does not increment
	float GetGameMSElapsed();

	// GetUIMSDelta()
	// 		returns milliseconds passed since last UI tick
	// 		UI tick is kept separate from Game Tick to allow both to run under different conditions (paused or otherwise)
	float GetUIMSDelta();

	// GetFrameCount()
	// 		returns current frame number
	uint32 GetFrameCount();


	// TimeInit()
	// 		Setup timers, must be called during framework init
	void TimeInit();


	// TimeUpdate()
	// 		Should be called once per tick to make sure all timers are current
	void TimeUpdate();
}




