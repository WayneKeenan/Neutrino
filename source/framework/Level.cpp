#include "Level.h"
#include "libconfig.h"
#include "Log.h"
#include "File.h"
#include "Memory.h"
#include "ConfigFile.h"
#include "Assert.h"

namespace Neutrino {

	bool GenerateTileMapFromFile(const char* pFilename)
	{
		if (!ResourceFileExists(pFilename)) { LOG_ERROR("GenerateTileMapFromFile: Unable to find %s, exiting...", pFilename); return false; }

		return true;
	}



	bool LoadLevelDetailsFromConfigFile()
	{
		return true;
	}


}
