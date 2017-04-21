#include "Time.h"
#include "NeutrinoMath.h"
#include "Assert.h"
#include <time.h>

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4996 ) 
#endif
namespace Neutrino
{
	static uint32 s_iSystemMS_Elapsed;
	static uint64 s_iFrameCount;

	static float s_fSystemMS_Delta;
	static float s_fGameMS_Delta;
	static float s_fUIMS_Delta;
 
	static float s_fMaxDelta = 1.0f/15.0f;
	static float s_fMinDelta = 1.0f/200.0f;		// Really?

	static time_t _Rawtime;
	static const char* s_sTimeStamp;

	static bool s_bInitialized = false;
	static bool s_bGameTimerPaused = false;
	static bool s_bUITimerPaused = false;

	
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

		// _TODO: Take this out if you don't end up using the timestamp in the logfile output!
		time(&_Rawtime);
		s_sTimeStamp = ctime(&_Rawtime);

		uint32 iCurrentTime = SDL_GetTicks();

		// Calc the MS delta for this tick
		s_fSystemMS_Delta = clamp( (float)(iCurrentTime - s_iSystemMS_Elapsed) / 1000.0f, s_fMinDelta, s_fMaxDelta);
		s_iSystemMS_Elapsed = iCurrentTime;

		// Allow for independent pause of Game Time and UI Timer
		{
			if( s_bGameTimerPaused )
				s_fGameMS_Delta = 0.0f;
			else
				s_fGameMS_Delta = s_fSystemMS_Delta;


			if( s_bUITimerPaused )
				s_fUIMS_Delta = 0.0f;
			else
				s_fUIMS_Delta = s_fSystemMS_Delta;
		}

		++s_iFrameCount;
	}


	uint32 GetMSSinceStartup()
	{
		return s_iSystemMS_Elapsed;
	}

	uint32 GetCurrentMS()
	{
		return SDL_GetTicks();
	}

	float GetMSDelta()
	{
		return s_fSystemMS_Delta;
	}

	float GetGameMSDelta()
	{
		return s_fGameMS_Delta;
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

	void SetGameTimerIsPaused(const bool bState)
	{
		s_bGameTimerPaused = bState;
	}

	void SetUITimerIsPaused(const bool bState)
	{
		s_bUITimerPaused = bState;
	}
}
#ifdef _MSC_VER
#pragma warning( pop )
#endif