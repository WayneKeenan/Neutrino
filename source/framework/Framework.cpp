#include "Framework.h"
#include "libconfig.h"
#include <stdio.h>

#if defined DEBUG
	#include "Debug.h"
	#include "editor/CMapEditorIn.h"
#endif


#if defined _WIN32
	#define sprintf sprintf_s
#endif
namespace Neutrino 
{

	NeutrinioPreferences_t* NeutrinoPreferences = NULL; 
	CGameGlobals* pGameGlobals;		// Remove this, just a test...

	static const char* const s_pOrganisation = "TripleEh";
	static const char* const s_pPrefsFilename = "PlayerPrefs.tdi";
	static const char* const s_pResourcesFilename = "NeutrinoData.tdi";

	static bool s_bRunningStatus = true;
	static uint8 s_iEditorModeFlag = 0x00;
	static uint8 s_iIsInMode = 0x00;


	// TODO: This is temporary, should be retrieved from the active game state.
	//       Should framework have a set of camera functionality?
	static glm::vec3 s_pvCameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////


	bool CoreInit( const char* const pGameName )
	{
		if(	!SDLInit(s_pOrganisation, pGameName) ) return false;

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


				//
				// Parse the dimensions for the game window and internal render buffer
				// (This is for pixel games, so assumption is we'll always render to a smaller
				// backbuffer and then present that scaled up in the final displayport)
				//     
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


				//
				//	Now parse the player's input preferences
				if( !InputInit(&cfg) )
				{
					config_destroy(&cfg);	
					return false;
				}
				LOG_INFO("Input bindings parsed and set to the following:\n\n%s", GetInputMappingsString());				

				// Think we've got what we need from the config file for now
				config_destroy(&cfg);
			}
			else
			{
				LOG_WARNING("No player prefs file found, creating defaults...");

				// This will create a default keyboard input mapping set (Mapped to SDL Keycodes)
				InputInitWithDefaults();

				// write out a default player prefs file since this is looks like the first run
				//
#if defined _WIN32
				fopen_s(&pPlayerPrefsFile, pPlayerPrefsFilename, "w");
				if(pPlayerPrefsFile)
#else
				if( (pPlayerPrefsFile = fopen(pPlayerPrefsFilename, "w")) )
#endif
				{

					NeutrinoPreferences->s_iScreenWidth = iDEFAULT_VIEWPORT_WIDTH;
					NeutrinoPreferences->s_iScreenHeight = iDEFAULT_VIEWPORT_HEIGHT;
					NeutrinoPreferences->s_iInternalWidth = iDEFAULT_INTERNAL_WIDTH;
					NeutrinoPreferences->s_iInternalHeight = iDEFAULT_INTERNAL_HEIGHT;

					const char* pInputMappingsText = GetInputMappingsString();
					const char* pPrefsText = "screenheight: 1080\nscreenwidth: 1920\ninternalwidth: 320\ninternalheight: 180\n";

					fprintf(pPlayerPrefsFile, "%s", pPrefsText);
					fprintf(pPlayerPrefsFile, "%s", pInputMappingsText);

					fflush(pPlayerPrefsFile);
					fclose(pPlayerPrefsFile);

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


		// Now the resources file is mounted, load the Game Config file and keep it open for other 
		// parts of the game to quickly grab whatever it is they need. 
		if(!LoadConfigFile())
		{
			LOG_ERROR("Unable to load Game Config file, exiting.");
			return false;
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
			GLUtils::GenerateMVCMatrices(&s_pvCameraPosition);
		}

		// Init timing and logging facility
		//
		{
			TimeInit();
			GetSystemLog();
		}

		// Load all the textures defined in GameCofig.txt
		// There's a current assumption that we can probably load all our textures on init 
		// as there'll never be too many of them. Not likely to fill VRAM with these games...
		if(!LoadTexturesFromConfigFile())
		{
			LOG_ERROR("Framework was unable to load all textures, exiting...");
			return false;
		}

#if defined DEBUG
		// If we're a DEBUG build, allocate some sprite arrays for untextured sprites. The 
		// editor modes will use these. 
		AllocateUntexturedSpriteArrays();
		GLUtils::CreateDebugVBOs();
#endif


		// Create any Singletons we need
		//
		CGameGlobals::Create();
		pGameGlobals = CGameGlobals::InstancePtr(); // Remove this, just a test...



		// Enter Initial Gamestate
		GameStateInit();


		return s_bRunningStatus;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	static CGameState* s_pEditorState = NULL;

	bool CoreUpdate()
	{
		// TODO: Remove this, temporary calc of the camera position to support the flycam
#if defined DEBUG
		s_pvCameraPosition = *GetFlyCamOffset(); // + vGameCameraPosition;
#endif
		// Update clocks 
		TimeUpdate();

		// Poll input events, pass controls to IMGUI and capture Quit state
		s_bRunningStatus = SDLProcessInput(&s_iEditorModeFlag);

		// Reset active sprite count to zero
		ResetSpriteCount();

		// Process the active game state
		GameStateUpdate();

		// Generate new Camera/World matrices for this frame
		// TODO: Need to get Game Camera Position and add it to this for MCV generation
		GLUtils::GenerateMVCMatrices(&s_pvCameraPosition);

		// Set the active shader for this pass
		SetActiveShader(DEFAULT_SHADER);

		// Draw everything
		DrawSprites();

#if defined DEBUG
		// Output the debug mode overlay
		DebugOverlayUpdate();

		SetActiveShader(DEFAULT_UNTEXTURED);

		// Draw everything
		DrawDebugSprites();

		// If we're to enter an editor mode FORCEKILL current state, and innit the editor mode. 
		// Note: not all states will let themselves be forcekilled so this attempt may fail. 

		// Spline Editor
		if(s_iEditorModeFlag & _SPLINE_ED)
			LOG_INFO("Spline Editor Not Implemented");


		// Particle Editor
		if( s_iEditorModeFlag & _PARTICLE_ED )
			LOG_INFO("Particle Editor Not Implemented");

		// Tile Map Editor
		if(s_iEditorModeFlag & _MAP_ED)
			EnterEditorMode();
#endif



		// Let SDL do its magic...
		SDLPresent();

		return s_bRunningStatus;
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

		InputKill();

#if defined DEBUG
		DebugOverlayKill();
		DeallocateUntexturedSpriteArrays();
		GLUtils::DeallocateDebugVBOs();
#endif
		GameStateKill();
		SDLKill();

		LOG_INFO("Framework terminated (CoreKill) cleanly. Have a nice day!");
		return true;
	}


	void EnterEditorMode()
	{
		// Can we even do this?
		if( !GameStateAttemptForceKill() )
		{
			LOG_WARNING("Attempt to enter editor mode failed, current game mode does not support immediate exit");
			return; 
		}

		// Which mode do we want?

		// Tile Map Editor
		if(s_iEditorModeFlag & _MAP_ED)
		{
			if( !(s_iIsInMode & _MAP_ED) )
			{
				// Ok, we're good to go and can enter the new mode...
				s_iIsInMode |= _MAP_ED;
				s_pEditorState = NEWX(CMapEditorIn);
			}
			else
			{
				s_iIsInMode ^= _MAP_ED;
				s_pEditorState = NULL;
			}
		}
	
		// Change the game mode, either to our editor state, or back to the splash screen...
		if( NULL != s_pEditorState && s_iEditorModeFlag != 0x00)
			GameStateChange(s_pEditorState);
		else
			GameStateInit();
	}
}
