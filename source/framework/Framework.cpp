#include "Framework.h"

namespace Neutrino 
{

	bool CoreInit()
	{
		LOG_INFO("Neutrino Framework, Bootstrapping...");
		InitSDL();
		return true;
	}



	bool CoreUpdate()
	{
		return true;
	}


	bool CoreKill()
	{
		LOG_INFO("Terminating framework");
		return true;
	}
}
