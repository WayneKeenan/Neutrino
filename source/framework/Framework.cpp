#include "Framework.h"
#include "GLUtils.h"
#include "libconfig.h"

#include <stdio.h>

namespace Neutrino 
{

	NeutrinioPreferences_t* NeutrinoPreferences = NULL; 
	CGameGlobals* pGameGlobals;		// Remove this, just a test...

	static const char* const s_pOrganisation = "TripleEh";
	static const char* const s_pPrefsFilename = "PlayerPrefs.tdi";
	static const char* const s_pResourcesFilename = "NeutrinoData.tdi";

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

			if( FileExists(pPlayerPrefsFilename) )
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
				{
					config_destroy(&cfg);
					LOG_ERROR("Unable to parse screenhieght from Player Prefs file, exiting...");
					return false;
				}

				if( !config_lookup_int(&cfg, "screenwidth", &NeutrinoPreferences->s_iScreenWidth ))
				{
					config_destroy(&cfg);
					LOG_ERROR("Unable to parse screenwidth from Player Prefs file, exiting...");
					return false;
				}

				// Think we've got what we need from the config file for now
				config_destroy(&cfg);
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
					fclose(pPlayerPrefsFile);
 
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



		// Mount the resources pack file. Must do this prior to OGL setup as shaders will need to be loaded
		// from it.
		{
			char pResourcesFilename[4096]={'\0'};
			sprintf(pResourcesFilename, "%s%s", NeutrinoPreferences->s_pResourcePath, s_pResourcesFilename);
			if (!MountResources(pResourcesFilename))
			{
				LOG_ERROR("Unable to mount resources file, exiting.");
				return false;
			} 
		}
	

		// Create an SDL window with an OGL 3.1 Context and compile standard shaders
		// 
		{
			LOG_INFO("Screen dimensions: %d x %d", NeutrinoPreferences->s_iScreenWidth, NeutrinoPreferences->s_iScreenHeight);
			
			if( !SDLCreateWindowAndContext(NeutrinoPreferences->s_iScreenWidth, NeutrinoPreferences->s_iScreenHeight) )
				return false;
			
			if( !LoadEngineShaders() )
				return false;

			GLUtils::SetViewport(NeutrinoPreferences->s_iScreenWidth, NeutrinoPreferences->s_iScreenHeight);
			GLUtils::CreateVBO();
		}

		// Create any Singletons we need
		//
		CGameGlobals::Create();
		pGameGlobals = CGameGlobals::InstancePtr(); // Remove this, just a test...


		// Enter Initial Gamestate
		GameStateInit();

		return true;
	}



	bool CoreUpdate()
	{
		TimeUpdate();
		GameStateUpdate();
		GLUtils::TestRender();
		SDLPresent();
		return true;
	}


	bool CoreKill()
	{
		// Unmount resources bundle
		{
			char pResourcesFilename[4096]={'\0'};
			sprintf(pResourcesFilename, "%s%s", NeutrinoPreferences->s_pResourcePath, s_pResourcesFilename);

			if (!UnmountResources(pResourcesFilename))
				LOG_ERROR("Unable to unmount resources file: %s", pResourcesFilename);
	
			// Remove any singletons we created...
			CGameGlobals::Destroy();
			DELETEX(NeutrinoPreferences);
		}


		GameStateKill();
		SDLKill();
		
		LOG_INFO("Framework terminated (CoreKill) cleanly. Have a nice day!");
		return true;
	}

}
