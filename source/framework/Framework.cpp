#include "Framework.h"

namespace Neutrino 
{

	NeutrinioPreferences_t* NeutrinoPreferences = NULL; 

	static const char* const s_pOrganisation = "TripleEh\0";

	bool CoreInit( const char* const pGameName )
	{
		if(	!SDLInit(s_pOrganisation, pGameName) ) return false;

		// Init timing and logging facility
		//
		{
			TimeInit();
			GetSystemLog();
		}

		
		// Get prefs.ini for this game, parse it and populate engine preferences
		//
		{
			NeutrinoPreferences = NEWX(NeutrinioPreferences_t);
			NeutrinoPreferences->s_pPrefsPath = SDLGetPrefPath();
			NeutrinoPreferences->s_pResourcePath = SDLGetBasePath();

			LOG_INFO("Found resource path: %s", NeutrinoPreferences->s_pResourcePath);
			LOG_INFO("Found userdata path: %s", NeutrinoPreferences->s_pPrefsPath);
		}

		return true;
	}



	bool CoreUpdate()
	{
		TimeUpdate();
		return true;
	}


	bool CoreKill()
	{
		DELETEX(NeutrinoPreferences);
		SDLKill();
		LOG_WARNING("Framework terminated (CoreKill)");
		return true;
	}
}
