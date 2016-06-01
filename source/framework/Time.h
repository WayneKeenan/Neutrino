#pragma once

#include "Types.h"
#include "sdl/sdl_wrapper.h"

namespace Neutrino
{
	// GetMSSinceStartup() 
	// 		returns the milliseconds passed since Framework init
	uint32 GetMSSinceStartup();

	// GetMSDelta() 
	// 		returns milliseconds passed since last Framework tick
	uint32 GetMSDelta();


	// GetGameMSDelta() 
	// 		returns milliseconds passed since last game tick. 
	// 		Game tick normally matches framework tick, but can be paused, spedup or slowed down
	float GetGameMSDelta();


	// GetGameMSElapsed()
	// 		returns milliseconds elapsed over total duration of game ticks
	// 		Note, if game tick is paused then MS elapsed does not increment
	uint32 GetGameMSElapsed();

	// GetUIMSDelta()
	// 		returns milliseconds passed since last UI tick
	// 		UI tick is kept separate from Game Tick to allow both to run under different conditions (paused or otherwise)
	float GetUIMSDelta();

	// GetFrameCount()
	// 		returns current frame number
	uint64 GetFrameCount();


	// TimeInit()
	// 		Setup timers, must be called during framework init
	void TimeInit();


	// TimeUpdate()
	// 		Should be called once per tick to make sure all timers are current
	void TimeUpdate();


	// GetTimeStamp()
	// 		Returns human readable string for the current datetime
	const char* GetTimeStamp();
}




