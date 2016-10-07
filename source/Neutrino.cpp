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
		bRunning = Neutrino::CoreUpdate();
	}

	Neutrino::CoreKill();
	
    return 0;
}

