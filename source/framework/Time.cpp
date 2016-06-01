#include "Time.h"
#include "Clamp.h"
#include "Assert.h"
#include <time.h>

namespace Neutrino
{
	static uint32 s_iSystemMS_Elapsed;
	static uint32 s_iGameMS_Elapsed;
	static uint32 s_iSystemMS_Delta;
	static uint64 s_iFrameCount;

	static float s_fGameMS_Delta;
	static float s_fUIMS_Delta;
 
	static uint32 s_iMaxDelta = 15 * 1000;
	static uint32 s_iMinDelta = 60 * 1000;		// Really?

	static time_t _Rawtime;
	static const char* s_sTimeStamp;

	static bool s_bInitialized = false;
	
	// 13.4.16 [GN]
	// 		Assume SDL will always be present, don't put an interface class in as middle man
	void TimeInit()
	{
		time(&_Rawtime);
		s_sTimeStamp = ctime(&_Rawtime);
		s_iSystemMS_Elapsed = SDL_GetTicks();
		s_bInitialized = true;
	}


	void TimeUpdate()
	{
		ASSERT(s_bInitialized, "TimeUpdate called before TimeInit()!");

		time(&_Rawtime);
		s_sTimeStamp = ctime(&_Rawtime);

		uint32 iCurrentTime = SDL_GetTicks();
		s_iSystemMS_Delta = iCurrentTime - s_iSystemMS_Elapsed;
		s_iSystemMS_Elapsed = iCurrentTime;

		// 13.4.16 [GN]
		// 	_TODO: Need to add Timestep multipliers for UI and Game Timers
		//
		//	_TODO: These need to be converted into a float that we can multiply against in code...
		//s_fGameMS_Delta = clamp(s_fSystemMS_Delta, s_fMinDelta, s_fMaxDelta);
		//s_iGameMS_Elapsed += s_fGameMS_Delta;		

		
		//s_fUIMS_Delta = clamp(s_fSystemMS_Delta, s_fMinDelta, s_fMaxDelta);
	}


	uint32 GetMSSinceStartup()
	{
		return s_iSystemMS_Elapsed;
	}

	uint32 GetMSDelta()
	{
		return s_iSystemMS_Delta;
	}

	float GetGameMSDelta()
	{
		return s_fGameMS_Delta;
	}

	uint32 GetGameMSElapsed()
	{
		return s_iGameMS_Elapsed;
	}

	float GetUIMSDelta()
	{
		return s_fUIMS_Delta;
	}

	uint64 GetFrameCount()
	{
		return s_iFrameCount;
	}

	const char* GetTimeStamp()
	{

		return s_sTimeStamp;
	}
}
