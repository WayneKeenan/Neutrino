#ifndef NEUTRINO_FRAMEWORK
#define NEUTRINO_FRAMEWORK

#include "sdl/sdl_wrapper.h"
#include "Log.h"
#include "Types.h"
#include "Assert.h"
#include "Memory.h"
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
#include "ConfigFile.h"

namespace Neutrino {
	
	typedef struct NeutrinioPreferences_t {
		int s_iScreenWidth;					// Viewport Dimensions
		int s_iScreenHeight;
		int s_iInternalWidth;				// Internal coord dimensions
		int s_iInternalHeight;
		const char* s_pResourcePath;		// Packfile locations
		const char* s_pPrefsPath;			// Player prefs file location
		
	} NeutrinioPreferences_t;


	extern NeutrinioPreferences_t* NeutrinoPreferences;
	extern CGameGlobals* pGameGlobals;
	
	bool CoreInit(const char* const pGameName);
	bool CoreUpdate();
	bool CoreKill();
}


#endif
