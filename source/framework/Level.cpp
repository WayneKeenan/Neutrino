#include "Level.h"
#include "libconfig.h"
#include "Log.h"
#include "File.h"
#include "Memory.h"
#include "ConfigFile.h"
#include "Assert.h"

namespace Neutrino {

	static LevelFileDetails_t* s_aLevelFileDetails;

	bool GenerateTileMapFromFile(const char* pFilename)
	{
		return true;
	}



	bool LoadLevelDetailsFromConfigFile()
	{
		const config_setting_t* pLevels = GameConfigGetList("levels");
		if( NULL == pLevels )
		{
			LOG_ERROR("LoadLevelDetailsFromConfigFile: failed to find levels list");
			return false;
		}

		int iLvlCount = config_setting_length(pLevels);
		LOG_INFO("There are %d levels defined in the game config...", iLvlCount);


		for (int i = 0; i < iLvlCount; ++i)
		{
			const config_setting_t* pElem = GameConfigGetListElement(pLevels, i);
			ASSERT(pElem, "LoadLevelDetailsFromConfigFile: failure to get levels list item %d", i);
		
			const char* pFilename = GameConfigGetStringFromSetting(pElem, "background_map");
			ASSERT(pFilename, "LoadLevelDetailsFromConfigFile: failure to parse background_map filename");

			GenerateTileMapFromFile(pFilename);
		}

		return true;
	}

	void DeallocateLevels()
	{
		DELETEX [] s_aLevelFileDetails;
		LOG_INFO("Levels deallocated.");
	}

}
