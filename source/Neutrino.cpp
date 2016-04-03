#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>

int main(int, char**)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	else
	{
		std::cout << "SDL Init OK!\n\n";
	}
	SDL_Quit();
	return 0;
}

