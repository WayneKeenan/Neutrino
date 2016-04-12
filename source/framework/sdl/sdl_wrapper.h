#ifndef SDL_WRAPPER
#define SDL_WRAPPER

#include <SDL.h>
#include <SDL_opengl.h>
#include "../Assert.h"

namespace Neutrino
{
	extern SDL_Window* g_SDL_WindowHandle;
	extern SDL_GLContext g_SDL_GLContext;

	bool InitSDL();
}

#endif
