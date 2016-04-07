#include "sdl_wrapper.h"

namespace Neutrino
{
	SDL_Window* g_SDL_WindowHandle = NULL;
	SDL_GLContext g_SDL_GLContext;


	bool InitSDL()
	{
		printf("Init SDL Called...\n");
		return true;
	}
}
