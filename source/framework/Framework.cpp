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
	NeutrinoPreferences_t* s_pNeutrinoPreferences = NULL; 
	CGameGlobals* pGameGlobals;		// Remove this, just a test...

	static const char* const s_pOrganisation = "TripleEh";
	static const char* const s_pPrefsFilename = "PlayerPrefs.tdi";
	static const char* const s_pResourcesFilename = "NeutrinoData.tdi";

	static char s_pResourcesPath[4096] = { '\0' };

	static bool s_bRunningStatus = true;
	static uint8 s_iEditorModeFlag = 0x00;
	static uint8 s_iIsInMode = 0x00;

#if defined DEBUG
	static CGameState* s_pEditorState = NULL;
#endif


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
			s_pNeutrinoPreferences = NEWX(NeutrinoPreferences_t);
			s_pNeutrinoPreferences->_pPrefsPath = SDLGetPrefPath();
			s_pNeutrinoPreferences->_pResourcePath = SDLGetBasePath();

			LOG_INFO("Resource path: %s", s_pNeutrinoPreferences->_pResourcePath);
			LOG_INFO("Userdata path: %s", s_pNeutrinoPreferences->_pPrefsPath);


			FILE* pPlayerPrefsFile;
			char pPlayerPrefsFilename[4096]={'\0'};
			sprintf(pPlayerPrefsFilename, "%s/%s", s_pNeutrinoPreferences->_pPrefsPath, s_pPrefsFilename);

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
				if(!config_read_file(&cfg, pPlayerPrefsFilename)) 
				{
					config_destroy(&cfg);
					LOG_ERROR("Unable to parse Player Prefs file, exiting...");
					return false;
				}

				if(!config_lookup_int(&cfg, "screenheight", &s_pNeutrinoPreferences->_iScreenHeight))
				{
					config_destroy(&cfg);
					LOG_ERROR("Unable to parse screenhieght from Player Prefs file, exiting...");
					return false;
				}

				if(!config_lookup_int(&cfg, "screenwidth", &s_pNeutrinoPreferences->_iScreenWidth ))
				{
					config_destroy(&cfg);
					LOG_ERROR("Unable to parse screenwidth from Player Prefs file, exiting...");
					return false;
				}

				if(!config_lookup_int(&cfg, "internalheight", &s_pNeutrinoPreferences->_iInternalHeight))
				{
					config_destroy(&cfg);
					LOG_ERROR("Unable to parse internalhieght from Player Prefs file, exiting...");
					return false;
				}

				if(!config_lookup_int(&cfg, "internalwidth", &s_pNeutrinoPreferences->_iInternalWidth ))
				{
					config_destroy(&cfg);
					LOG_ERROR("Unable to parse internalwidth from Player Prefs file, exiting...");
					return false;
				}


				//	Now parse the player's input preferences
				if(!InputInit(&cfg))
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
				if((pPlayerPrefsFile = fopen(pPlayerPrefsFilename, "w")))
#endif
				{

					s_pNeutrinoPreferences->_iScreenWidth = iDEFAULT_VIEWPORT_WIDTH;
					s_pNeutrinoPreferences->_iScreenHeight = iDEFAULT_VIEWPORT_HEIGHT;
					s_pNeutrinoPreferences->_iInternalWidth = iDEFAULT_INTERNAL_WIDTH;
					s_pNeutrinoPreferences->_iInternalHeight = iDEFAULT_INTERNAL_HEIGHT;

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
			sprintf(s_pResourcesPath, "%s%s", s_pNeutrinoPreferences->_pResourcePath, s_pResourcesFilename);
			if (!MountResources(s_pResourcesPath))
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
			LOG_INFO("Screen dimensions: %d x %d", s_pNeutrinoPreferences->_iScreenWidth, s_pNeutrinoPreferences->_iScreenHeight);
			LOG_INFO("Internal dimensions: %d x %d", s_pNeutrinoPreferences->_iInternalWidth, s_pNeutrinoPreferences->_iInternalHeight);


			if( !SDLCreateWindowAndContext(s_pNeutrinoPreferences->_iScreenWidth, s_pNeutrinoPreferences->_iScreenHeight) )
				return false;

			if( !AttachShaders() )
				return false;
		}

		// Set our internal / external dimensions
		{
			GLUtils::SetViewport(s_pNeutrinoPreferences->_iScreenWidth, s_pNeutrinoPreferences->_iScreenHeight);
			GLUtils::SetDimensions(s_pNeutrinoPreferences->_iScreenWidth, s_pNeutrinoPreferences->_iScreenHeight, s_pNeutrinoPreferences->_iInternalWidth, s_pNeutrinoPreferences->_iInternalHeight);
			GLUtils::GenerateMVCMatrices(&s_pvCameraPosition);

			const glm::vec2 vDims = GLUtils::GetInternalPixelScale();
			s_pNeutrinoPreferences->_InternalPixelWidth = vDims.x;
			s_pNeutrinoPreferences->_InternalPixelHeight = vDims.y;
		}

		// Allocate FBOs and the render textures
		if (!GLUtils::AllocateFBOs())
		{
			LOG_ERROR("Unable to allocate FBOs / Render Textures, exiting...");
			return false;
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

		// Load the generic level data defined in GameConfig.txt
		// Levels structures are game specific, but the framework generically supports tilemaps
		// audio data, and [TODO] that we can parse/load on init.
		if (!LoadLevelsFromConfig())
		{
#if defined DEBUG
			LOG_ERROR("Unable to load level data, assuming you want to go into editor modes though...");
#else
			LOG_ERROR("Unable to load the level data, exiting...");
			return false;
#endif
		}

#if defined DEBUG
		// If we're a DEBUG build, allocate some sprite arrays for untextured sprites. The 
		// editor modes will use these. 
		AllocateUntexturedSpriteArrays();
		GLUtils::AllocateDebugVBOs();
#endif


		// Create GameGlobals and set the dimensions parsed from the preferences file. 
		{
			CGameGlobals::Create();
			pGameGlobals = CGameGlobals::InstancePtr(); // TODO: Remove this, just a test...
		}

		// Enter Initial Gamestate
		GameStateInit();

		// Init finished successfully 
		return s_bRunningStatus;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CoreUpdate()
	{
		TimeUpdate();																														// Update the internal clocks

#if defined DEBUG
		s_pvCameraPosition = *GetFlyCamOffset(); // + vGameCameraPosition;			// TODO: remove this, should the framework even know about this?
#endif

		s_bRunningStatus = SDLProcessInput(&s_iEditorModeFlag);									// Poll input events, pass controls to IMGUI and capture Quit state TODO: make status a param to the function
		ResetSpriteCount();																											// Must be called each tick, resets base pointers for all the sprites
		GameStateUpdate();																											// Process whatever is the active game state
							
		GLUtils::ClearBuffers();																								// This doesn't clear the FBOs!

		if(0 == s_iIsInMode)  // We're in the normal game mode, do the full render path
		{
			// Clamp Camera Movement to a Texel
			{
// 				int fXPixels = (int)roundf(s_pvCameraPosition.x / GLUtils::GetInternalPixelScale().x);
// 				int fYPixels = (int)roundf(s_pvCameraPosition.y / GLUtils::GetInternalPixelScale().y);
// 				s_pvCameraPosition.x = (float)fXPixels * GLUtils::GetInternalPixelScale().x;
// 				s_pvCameraPosition.y = (float)fYPixels * GLUtils::GetInternalPixelScale().y;
				GLUtils::GenerateMVCMatrices(&s_pvCameraPosition);
			}

			// Set shader for rendering to a texture
			SetActiveShader(DEFAULT_SHADER);

			// Render our elements to an offscreen texture and present it to the final viewport
			{
				GLUtils::StartOffscreenRender();
				DrawTilemap();
				TestSprite();
				DrawSprites(true);
				GLUtils::FinishOffScreenRender();
			}
		}
		else  // We're in an editor mode, so do a simplified render path
		{
			
			GLUtils::GenerateMVCMatrices(&s_pvCameraPosition);
			SetActiveShader(DEFAULT_SHADER);
			DrawSprites(false);
		}

	
#if defined DEBUG
		DebugOverlayUpdate();
		SetActiveShader(DEFAULT_UNTEXTURED);
		DrawDebugSprites(false);

		// Process editor toggling
		if(s_iEditorModeFlag & _SPLINE_ED) LOG_INFO("Spline Editor Not Implemented");
		if(s_iEditorModeFlag & _PARTICLE_ED) LOG_INFO("Particle Editor Not Implemented");
		if(s_iEditorModeFlag & _MAP_ED) EnterEditorMode();
#endif

		SDLPresent();
		return s_bRunningStatus;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////


	bool CoreKill()
	{
		sprintf(s_pResourcesPath, "%s%s", s_pNeutrinoPreferences->_pResourcePath, s_pResourcesFilename);

		if (!UnmountResources(s_pResourcesPath))
			LOG_ERROR("Unable to unmount resources file: %s", s_pResourcesPath);

		// Remove any singletons we created...
		CGameGlobals::Destroy();
		DELETEX(s_pNeutrinoPreferences);

		DeallocateLevels();
		DeallocateSpriteArrays();
		DeallocateTextures();			
		GLUtils::DeallocateDynamicVBOs();
		GLUtils::DeallocateTilemapVBOs();	// TODO: this needs to be replaced with "DeallocateLevels"
		GLUtils::DeallocateFBOs();
		DetachShaders();
		InputKill();
		
#if defined DEBUG
		DebugOverlayKill();
		GLUtils::DeallocateDebugVBOs();
#endif

		GameStateKill();
		SDLKill();

		LOG_INFO("Framework terminated (CoreKill) cleanly. Have a nice day!");
		return true;
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////



#if defined DEBUG
	void EnterEditorMode()
	{

		// Can we even do this?
		if (!GameStateAttemptForceKill())
		{
			LOG_WARNING("Attempt to enter editor mode failed, current game mode does not support immediate exit");
			return;
		}

		// Which mode do we want?

		// Tile Map Editor
		if (s_iEditorModeFlag & _MAP_ED)
		{
			if (!(s_iIsInMode & _MAP_ED))
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
		if (NULL != s_pEditorState && s_iEditorModeFlag != 0x00)
			GameStateChange(s_pEditorState);
		else
			GameStateInit();
	}
#endif
}


