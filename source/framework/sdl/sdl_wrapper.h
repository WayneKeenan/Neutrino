#ifndef SDL_WRAPPER
#define SDL_WRAPPER
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "../Assert.h"

namespace Neutrino
{
	// SDLInit():
	// 		Starts SDL and needed subsystems
	bool SDLInit(const char * const pOrgName, const char * const pGameName);

	// SDLKill():
	// 		Performs a clean shutdown of SDL and any open GL contexts
	bool SDLKill();


	// SDLCreateWindowAndContext()
	// 		Attempts to open an SDL window of given dimensions with an OGL3.1 context
	bool SDLCreateWindowAndContext(int iScreenWidth, int iScreenHeight); 	
	
	const char* SDLGetBasePath();
	const char* SDLGetPrefPath();

}

#endif
