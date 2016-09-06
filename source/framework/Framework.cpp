#include "Framework.h"
#include "libconfig.h"

#include <stdio.h>

namespace Neutrino 
{

	NeutrinioPreferences_t* NeutrinoPreferences = NULL; 
	CGameGlobals* pGameGlobals;		// Remove this, just a test...

	static const char* const s_pOrganisation = "TripleEh";
	static const char* const s_pPrefsFilename = "PlayerPrefs.tdi";
	static const char* const s_pResourcesFilename = "NeutrinoData.tdi";


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////


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

				if( !config_lookup_int(&cfg, "internalheight", &NeutrinoPreferences->s_iInternalHeight))
				{
					config_destroy(&cfg);
					LOG_ERROR("Unable to parse internalhieght from Player Prefs file, exiting...");
					return false;
				}

				if( !config_lookup_int(&cfg, "internalwidth", &NeutrinoPreferences->s_iInternalWidth ))
				{
					config_destroy(&cfg);
					LOG_ERROR("Unable to parse internalwidth from Player Prefs file, exiting...");
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
					const char* _pPrefsText = "screenheight: 720\nscreenwidth: 1280\ninternalwidth: 320\ninternalheight: 180\n";
					fwrite(_pPrefsText, strlen(_pPrefsText), 1, pPlayerPrefsFile);
					fflush(pPlayerPrefsFile);
					fclose(pPlayerPrefsFile);
 
					NeutrinoPreferences->s_iScreenHeight = 1080;
					NeutrinoPreferences->s_iScreenWidth = 1920;
					NeutrinoPreferences->s_iInternalWidth = 320;
					NeutrinoPreferences->s_iInternalHeight = 180;
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
	

		// Create an SDL window with an OGL 3 Context and compile standard shaders
		// 
		{
			LOG_INFO("Screen dimensions: %d x %d", NeutrinoPreferences->s_iScreenWidth, NeutrinoPreferences->s_iScreenHeight);
			LOG_INFO("Internal dimensions: %d x %d", NeutrinoPreferences->s_iInternalWidth, NeutrinoPreferences->s_iInternalHeight);
			

			if( !SDLCreateWindowAndContext(NeutrinoPreferences->s_iScreenWidth, NeutrinoPreferences->s_iScreenHeight) )
				return false;
			
			if( !AttachShaders() )
				return false;

			GLUtils::SetViewport(	
									NeutrinoPreferences->s_iScreenWidth, 
									NeutrinoPreferences->s_iScreenHeight,
									NeutrinoPreferences->s_iInternalWidth, 
									NeutrinoPreferences->s_iInternalHeight 
								);
			GLUtils::GenerateMVCMatrices();
		}


		// Load all the textures defined in GameCofig.txt
		// There's a current assumption that we can probably load all our textures on init 
		// as there'll never be too many of them. Not likely to fill VRAM with these games...
		//  
		if(!LoadTexturesFromConfigFile())
		{
			LOG_ERROR("Framework was unable to load all textures, exiting...");
			return false;
		}


		// Create any Singletons we need
		//
		CGameGlobals::Create();
		pGameGlobals = CGameGlobals::InstancePtr(); // Remove this, just a test...


		// Enter Initial Gamestate
		GameStateInit();

		return true;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////


	bool CoreUpdate()
	{
		// Pump SDL Events
		//
		
		// Update clocks 
		TimeUpdate();

		// Reset active sprite count to zero
		ResetSpriteCount();

		// Process the active game state (this will process the game objects)
		GameStateUpdate();

		// Generate new Camera/World matrices for this frame
		GLUtils::GenerateMVCMatrices();


		// Generate some test sprites (TO BE REMOVED)
		TestSprite();


		// Set the active shader for this pass
		SetActiveShader(DEFAULT_SHADER);
		DrawSprites();

		// Let SDL do its magic...
		SDLPresent();
		return true;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////


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

			DeallocateSpriteArrays();
			DeallocateAllTextures();		// Also deletes all VBOs
			DetachShaders();
		}


		GameStateKill();
		SDLKill();
		
		LOG_INFO("Framework terminated (CoreKill) cleanly. Have a nice day!");
		return true;
	}

}
