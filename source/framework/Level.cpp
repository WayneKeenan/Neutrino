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

		//
		// Parse the tilemap data
		//
		{
			config_t cfg;
			config_init(&cfg);

			const char* pFileBytes = LoadResourceBytes(pFilename);
			ASSERT(NULL != pFileBytes, "Unable to load tilemap data file");


			if(!config_read_string(&cfg, pFileBytes))
			{
				const char* pErr = config_error_text(&cfg);
				config_destroy(&cfg);

				LOG_ERROR("Unable to parse tilemap data: \'%s\' - exiting...", pErr);
				return false;
			}

			int iMapWidth = 0;
			int iMapHeight = 0;
			int iTileW = 0;
			int iTileH = 0;

			if(!ConfigGetInt(&cfg, "map.width", &iMapWidth))
			{
				LOG_ERROR("Unable to find tilemap width");
				return false;
			}

			if(!ConfigGetInt(&cfg, "map.height", &iMapHeight))
			{
				LOG_ERROR("Unable to find tilemap height");
				return false;
			}

			if(!ConfigGetInt(&cfg, "map.tilewidth", &iTileW))
			{
				LOG_ERROR("Unable to find tilewidth");
				return false;
			}

			if(!ConfigGetInt(&cfg, "map.tileheight", &iTileH))
			{
				LOG_ERROR("Unable to find tileheight");
				return false;
			}

			LOG_INFO("Generating static tilemap from \'%s\'. Map is %d by %d, tiles are %d by %d pixels.", pFilename, iMapWidth, iMapHeight, iTileW, iTileH);
		}

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

			if(!GenerateTileMapFromFile(pFilename))
				return false;
		}

		return true;
	}


}
