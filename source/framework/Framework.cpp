#include "Framework.h"
#include <stdio.h>

#if defined DEBUG
	#include "Debug.h"
	#include "editor/CMapEditorIn.h"
#endif

#if defined _WIN32
	#define sprintf sprintf_s
#endif


#include "sdl/sdl_wrapper.h"
#include "Log.h"
#include "Types.h"
#include "Time.h"
#include "GL/glew.h"
#include "GLUtils.h"
#include "Sprite.h"
#include "File.h"
#include "ShaderUtils.h"
#include "GameStateManager.h"
#include "../game/global/CGameGlobals.h"
#include "Texture.h"
#include "Input.h"
#include "Level.h"
#include "Physics.h"
#include "IniFile.h"

namespace Neutrino 
{
	NeutrinoPreferences_t* s_pNeutrinoPreferences = NULL; 
	CGameGlobals* pGameGlobals;		

	static bool s_bRunningStatus = true;
	static uint8 s_iEditorModeFlag = 0x00;
	static uint8 s_iIsInMode = 0x00;

	static const int s_iFilepathLength = 4096;
	static const char* const s_pOrganisation = "TripleEh";
	static const char* const s_pPrefsFilename = "PlayerPrefs.tdi";
	static const char* const s_pPrefsIniFilename = "PlayerPrefs.ini";
	static const char* const s_pResourcesFilename = "NeutrinoData.tdi";

	static char s_pResourcesPath[s_iFilepathLength] = { '\0' };
	
#if defined DEBUG
	static CGameState* s_pEditorState = NULL;
#endif

	// TODO: Framework should have a set of camera functions
	static glm::vec3 s_pvCameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////


	bool CoreInit( const char* const pGameName )
	{
		if(	!SDLInit(s_pOrganisation, pGameName) ) return false;


		// Create a new prefs struct and populate the default paths
		s_pNeutrinoPreferences = NEWX(NeutrinoPreferences_t);
		s_pNeutrinoPreferences->_pPrefsPath = SDLGetPrefPath();
		s_pNeutrinoPreferences->_pResourcePath = SDLGetBasePath();
		LOG_INFO("Resource path: %s", s_pNeutrinoPreferences->_pResourcePath);
		LOG_INFO("Userdata path: %s", s_pNeutrinoPreferences->_pPrefsPath);

		char pPlayerPrefsIniFilename[4096] = { '\0' };
		sprintf(pPlayerPrefsIniFilename, "%s/%s", s_pNeutrinoPreferences->_pPrefsPath, s_pPrefsIniFilename);
	

		// This will get updated with player preferences.
		InputInitWithDefaults();
		
		if (!FileExists(pPlayerPrefsIniFilename))
		{
			// If no player preferences .ini file exists, create a default one. This is probably the first run.
			LOG_WARNING("No Player Prefs Ini file found, creating defaults");
			CreateDefaultPlayerPrefsIni(pPlayerPrefsIniFilename);
		}
		else 
		{
			// If one exists, parse it and set the preferences for the framework. 
			LOG_INFO("Loading %s", pPlayerPrefsIniFilename);
			if(!ParsePlayerPrefsIni(pPlayerPrefsIniFilename, s_pNeutrinoPreferences))
			{
				LOG_ERROR("Unable to parse %s. Delete this file and re-launch the game. A new set of player defaults will be created and the game should launch.", pPlayerPrefsIniFilename);
				return false;
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


		if(!LoadGameConfigIni())
		{
			LOG_ERROR("Unable to load the GameConfig.ini file, is the resource bundle missing or corrupt?");
			return false;
		}


		// Create an SDL window, with audio mixer and an OGL 3 Context and compile standard shaders
		// 
		{
			LOG_INFO("Screen dimensions: %d x %d", s_pNeutrinoPreferences->_iScreenWidth, s_pNeutrinoPreferences->_iScreenHeight);
			LOG_INFO("Internal dimensions: %d x %d", s_pNeutrinoPreferences->_iInternalWidth, s_pNeutrinoPreferences->_iInternalHeight);


			if(!SDLCreateWindowAndContext(s_pNeutrinoPreferences->_iScreenWidth, s_pNeutrinoPreferences->_iScreenHeight))
				return false;

			if(!SDLInitialiseAudio()) 
				return false;

			if(!AttachShaders())
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
			pGameGlobals = CGameGlobals::InstancePtr(); 
			pGameGlobals->SetPlayerPreferences(s_pNeutrinoPreferences);
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
		GLUtils::GenerateMVCMatrices(&s_pvCameraPosition);

		if(0 == s_iIsInMode)		
		{
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
		else  
		{
			SetActiveShader(DEFAULT_SHADER);
			DrawSprites(false);
		}

	
#if defined DEBUG
		DebugOverlayUpdate();
		SetActiveShader(DEFAULT_UNTEXTURED);
		DrawDebugSprites(false);
		PhysicsDrawWorld();

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
		UnloadPlayerPrefsIni();
		UnloadGameConfigIni();

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


