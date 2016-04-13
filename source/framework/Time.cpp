#include "Time.h"
#include "Clamp.h"

namespace Neutrino
{
	static float s_fSystemMS_Elapsed;
	static float s_fSystemMS_Delta;
	static float s_fGameMS_Elapsed;
	static float s_fGameMS_Delta;
	static float s_fUIMS_Delta;
	static uint32 s_iFrameCount;
 
	static float s_fMaxDelta = 1/15.0f;
	static float s_fMinDelta = 1/60.0f;		// Really?
	
	// 13.4.16 [GN]
	// 		Assume SDL will always be present, don't put an interface class in as middle man
	void TimeInit()
	{
		s_fSystemMS_Elapsed = SDL_GetTicks();
	}



	void TimeUpdate()
	{
		float fCurrentTime = SDL_GetTicks();
		s_fSystemMS_Delta = fCurrentTime - s_fSystemMS_Elapsed;
		s_fSystemMS_Elapsed = fCurrentTime;
		
		// 13.4.16 [GN]
		// 	TODO: Need to add Timestep multipliers for UI and Game Timers
		s_fGameMS_Delta = clamp(s_fSystemMS_Delta, s_fMinDelta, s_fMaxDelta);
		s_fGameMS_Elapsed += s_fGameMS_Delta;
		
		s_fUIMS_Delta = clamp(s_fSystemMS_Delta, s_fMinDelta, s_fMaxDelta);
	}


	float GetMSSinceStartup()
	{
		return s_fSystemMS_Elapsed;
	}

	float GetMSDelta()
	{
		return s_fSystemMS_Delta;
	}

	float GetGameMSDelta()
	{
		return s_fGameMS_Delta;
	}

	float GetGameMSElapsed()
	{
		return s_fGameMS_Elapsed;
	}

	float GetUIMSDelta()
	{
		return s_fUIMS_Delta;
	}

	uint32 GetFrameCount()
	{
		return s_iFrameCount;
	}
}
