#include "sdl_wrapper.h"
#include "../Log.h"

namespace Neutrino
{
	SDL_Window* g_SDL_WindowHandle = NULL;
	SDL_GLContext g_SDL_GLContext;


	bool InitSDL()
	{
		if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_GAMECONTROLLER) != 0)
		{
			fprintf(stderr, "Unable to initialise SDL systems");
			return false;
		}
		return true;
	}
}
