#include <stdio.h>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "framework/Log.h"
#include "framework/Framework.h"

#if defined (WIN32)
#undef main
#include <tchar.h>
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int , char**) 
#endif
{
	if( !Neutrino::CoreInit("Neutrino") ) 
	{
		LOG_WARNING("Neutrino framework init failed. Please check the log file for information regarding the problem.");
		return 1;
	}

	bool bRunning = true;
	while(bRunning)
	{
		bRunning = Neutrino::CoreUpdate();
	}

	Neutrino::CoreKill();

	return 0;
}

