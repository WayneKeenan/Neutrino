#include <stdio.h>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include "framework/Framework.h"

int main(int, char**)
{
	if( !Neutrino::CoreInit("Neutrino") ) return 1;

	bool bRunning = true;
	while(bRunning)
	{
		Neutrino::CoreUpdate();

		SDL_Event e;
		while( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit
			if( e.type == SDL_QUIT )
			{
				bRunning = false;
			}	
		}
	}

	Neutrino::CoreKill();

    return 0;
}

