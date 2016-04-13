#include "sdl_wrapper.h"
#include "../Log.h"

namespace Neutrino
{
	SDL_Window* g_SDL_WindowHandle = NULL;
	SDL_GLContext g_SDL_GLContext;

	static const char* s_pBasePath;
	static const char* s_pPrefsPath;

	bool SDLInit(const char* const pOrgName, const char * const pGameName)
	{
		if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_GAMECONTROLLER) != 0)
		{
			fprintf(stderr, "Unable to initialise SDL systems");
			return false;
		}

		s_pPrefsPath = SDL_GetPrefPath( pOrgName, pGameName);
		s_pBasePath = SDL_GetBasePath();
		return true;
	}


	bool SDLKill()
	{
		SDL_Quit();
		return true;
	}

	const char* SDLGetPrefPath()
	{
		return s_pPrefsPath;
	}

	const char* SDLGetBasePath()
	{
		return s_pBasePath;
	}

}
