#pragma once
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "../Assert.h"
#include "../Types.h"

namespace Neutrino
{
	// SDLInit():
	// 		Starts SDL and needed subsystems
	bool SDLInit(const char * const pOrgName, const char * const pGameName);

	// SDLKill():
	// 		Performs a clean shutdown of SDL and any open GL contexts
	bool SDLKill();

	// SDLCreateWindowAndContext():
	// 		Attempts to open an SDL window of given dimensions with an OGL3.1 context
	bool SDLCreateWindowAndContext(const int iScreenWidth, const int iScreenHeight); 	


	bool SDLInitialiseAudio();

	void SDLDeinitialiseAudio();


	// SDLPresent()
	// 		Swap buffer...
	void SDLPresent();

	// Populate the engine's resource and preferences paths:
	const char* SDLGetBasePath();
	const char* SDLGetPrefPath();


	// GLTextureFromSDLSurface()
	// 		uses SDL image to load a texture to an SDL surface and bind that to an OGL texture. 
	//   	bFiltered will set the GL filter hint to GL_LINEAR or default to GL_NEAREST
	bool GLTextureFromSDLSurface(GLuint* pTextureID, const SDL_Surface* pSurf, const bool bFiltered);


	// SDLProcessInput()
	// 		Polls all SDL input events. Passes events on to IMGUI ad should - eventually - set our input mappings
	bool SDLProcessInput(uint8* iEditorFlags);

}

