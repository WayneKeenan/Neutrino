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
	if( !Neutrino::CoreInit() ) return 1;

	Neutrino::CoreKill();
	SDL_Quit();
	return 0;
}

