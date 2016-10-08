#include <stdio.h>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "framework/Log.h"
#include "framework/Framework.h"

int main(int, char**)
{
	if( !Neutrino::CoreInit("Neutrino") ) 
	{
		LOG_WARNING("Neutrino framework init failed. If you encountered parse errors please delete the PlayerPrefs.tdi file from the Userdata dir and try again. A default preferences file will be created. ");
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

