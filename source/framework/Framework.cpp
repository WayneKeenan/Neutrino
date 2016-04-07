#include "Framework.h"

namespace Neutrino 
{

	bool CoreInit()
	{
		InitSDL();
		printf("Neutrino Framework Init...\n");
		return true;
	}



	bool CoreUpdate()
	{
		return true;
	}


	bool CoreKill()
	{
		printf("Terminating framework\n");
		return true;
	}
}
