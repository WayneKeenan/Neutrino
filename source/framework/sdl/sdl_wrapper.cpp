#include "sdl_wrapper.h"
#include "../Log.h"

namespace Neutrino
{
	SDL_Window* g_SDL_WindowHandle = NULL;
	SDL_GLContext g_SDL_GLContext;


	bool InitSDL()
	{
		ASSERT(false, "Testing");
		LOG_INFO("Init SDL Called");
		return true;
	}
}
