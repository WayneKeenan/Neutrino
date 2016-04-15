#include "Framework.h"
#include "libconfig.h"
#include <sys/stat.h>
#include <stdio.h>

namespace Neutrino 
{

	NeutrinioPreferences_t* NeutrinoPreferences = NULL; 

	static const char* const s_pOrganisation = "TripleEh";
	static const char* const s_pPrefsFilename = "PlayerPrefs.tdi";

	bool CoreInit( const char* const pGameName )
	{
		if(	!SDLInit(s_pOrganisation, pGameName) ) return false;

		// Init timing and logging facility
		//
		{
			TimeInit();
			GetSystemLog();
		}

		
		// Get PlayerPrefs.tdi for this game, parse it and populate engine preferences
		//
		{
			NeutrinoPreferences = NEWX(NeutrinioPreferences_t);
			NeutrinoPreferences->s_pPrefsPath = SDLGetPrefPath();
			NeutrinoPreferences->s_pResourcePath = SDLGetBasePath();

			LOG_INFO("Resource path: %s", NeutrinoPreferences->s_pResourcePath);
			LOG_INFO("Userdata path: %s", NeutrinoPreferences->s_pPrefsPath);


			FILE* pPlayerPrefsFile;
			char pPlayerPrefsFilename[4096]={'\0'};
			sprintf(pPlayerPrefsFilename, "%s/%s", NeutrinoPreferences->s_pPrefsPath, s_pPrefsFilename);

			struct stat fileBuf;
			if( stat(pPlayerPrefsFilename, &fileBuf) == 0)
			{
				// Parse existing player preferences file...
				//
				config_t cfg;
				config_init(&cfg);

				if(! config_read_file(&cfg, pPlayerPrefsFilename)) 
				{
					config_destroy(&cfg);
					LOG_ERROR("Unable to parse Player Prefs file, exiting...");
					return false;
				}

				if( !config_lookup_int(&cfg, "screenheight", &NeutrinoPreferences->s_iScreenHeight))
					LOG_ERROR("Unable to parse screenhieght from Player Prefs file, exiting...");

				if( !config_lookup_int(&cfg, "screenwidth", &NeutrinoPreferences->s_iScreenWidth ))
					LOG_ERROR("Unable to parse screenwidth from Player Prefs file, exiting...");
			}
			else
			{
				// write out a default player prefs file since this is likely the first run
				//
				LOG_WARNING("No player prefs file found, creating defaults...");
				if( (pPlayerPrefsFile = fopen(pPlayerPrefsFilename, "w")) )
				{
					const char* _pPrefsText = "screenheight: 720\nscreenwidth: 1280\n";
					fwrite(_pPrefsText, strlen(_pPrefsText), 1, pPlayerPrefsFile);
					fflush(pPlayerPrefsFile);
 
					NeutrinoPreferences->s_iScreenHeight = 720;
					NeutrinoPreferences->s_iScreenWidth = 1280;
				}
				else
				{
					LOG_ERROR("Unable to write a default preferences file. Exiting...");
					return false;
				}
			}
		}



		// Create an SDL window with an OGL 3.1 Context
		// 
		{
			LOG_INFO("Screen dimensions: %d x %d", NeutrinoPreferences->s_iScreenWidth, NeutrinoPreferences->s_iScreenHeight);
			SDLCreateWindowAndContext(NeutrinoPreferences->s_iScreenWidth, NeutrinoPreferences->s_iScreenHeight);
		}

		return true;
	}



	bool CoreUpdate()
	{
		TimeUpdate();
		TestRender();
		SDLPresent();
		return true;
	}


	bool CoreKill()
	{
		DELETEX(NeutrinoPreferences);
		SDLKill();
		LOG_INFO("Framework terminated (CoreKill) cleanly. Have a nice day!");
		return true;
	}

}
