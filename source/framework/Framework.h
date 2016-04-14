#ifndef NEUTRINO_FRAMEWORK
#define NEUTRINO_FRAMEWORK

#include "sdl/sdl_wrapper.h"
#include "Log.h"
#include "Types.h"
#include "Assert.h"
#include "Memory.h"
#include "Time.h"

 

namespace Neutrino {
	
	typedef struct NeutrinioPreferences_t {
		int s_iScreenWidth;
		int s_iScreenHeight;
		const char* s_pResourcePath;
		const char* s_pPrefsPath;
	} NeutrinioPreferences_t;

	extern NeutrinioPreferences_t* NeutrinoPreferences;

	// Add resource path
	
	bool CoreInit(const char* const pGameName);
	bool CoreUpdate();
	bool CoreKill();
}


#endif