#include <stdio.h>
#include <iostream>
#include <string>
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "framework/Framework.h"

const int SCREEN_WIDTH 		= 1280;
const int SCREEN_HEIGHT 	= 720;

int main(int, char**)
{
	dmesg("Neutrino started in DEBUG MODE\n");
	

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	else
	{
		std::cout << "SDL Init OK!\n\n";
		Neutrino::CoreInit();
	}

	Neutrino::CoreKill();
	SDL_Quit();
	return 0;
}

