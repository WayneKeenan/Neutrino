#ifndef NEUTRINO_FRAMEWORK
#define NEUTRINO_FRAMEWORK

#include "sdl/sdl_wrapper.h"
#include "Log.h"
#include "Types.h"
#include "Assert.h"
#include "Memory.h"
#include "Time.h"



namespace Neutrino {
	
	// Add resource path
	
	bool CoreInit();
	bool CoreUpdate();
	bool CoreKill();
}


#endif
