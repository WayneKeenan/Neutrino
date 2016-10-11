#pragma once

#include "Types.h"
#include "sdl/sdl_wrapper.h"


namespace Neutrino
{
	// GetMSSinceStartup() 
	// 		returns the milliseconds passed since Framework init
	uint32 GetMSSinceStartup();

	// GetCurrentMSSinceStartup()
	// 		returns SDL_GetTicks
	uint32	GetCurrentMS();

	// GetMSDelta() 
	// 		returns milliseconds passed since last Framework tick
	float GetMSDelta();


	// GetGameMSDelta() 
	// 		returns milliseconds passed since last game tick. 
	// 		Game tick normally matches framework tick, but can be paused, spedup or slowed down
	float GetGameMSDelta();


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


	// SetGameTimerIsPaused()
	// 		Will pause or unpause the GameMS timers so GetGameMSDelta can be locked to simulate pausing the game
	void SetGameTimerIsPaused( const bool bState );


	// SetUITimerIsPaused()
	// 		Will pause or unpause the UIMS timers so GetUIMSDelta can be locked to simulate pausing the UI 
	void SetUITimerIsPaused( const bool bState );



}




