#include "Framework.h"

namespace Neutrino 
{

	bool CoreInit()
	{
		if(	!InitSDL() ) return false;
		TimeInit();
		GetSystemLog();
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
