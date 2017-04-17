#include "Level.h"
#include "Log.h"
#include "ConfigFile.h"
#include "Assert.h"
#include "Types.h"
#include "Memory.h"
#include "File.h"

namespace Neutrino {

	static FrameworkLevelData_t* s_pFrameworkLevels[iMAX_LEVELS];
	static uint16 s_iNumLevelsLoaded = 0;


	bool LoadLevelsFromConfig()
	{
		const config_setting_t* pLevels;
		const config_setting_t* pLevel; 
		int iNumLevels = 0;

		// Do basic sanity checks before we parse the data
		//
		{
			pLevels = GameConfigGetSetting("levels");
			if (NULL == pLevels)
			{
				LOG_ERROR("Unable to get the levels group setting from the config file!");
				return false;
			}

			iNumLevels = GetSettingLength(pLevels);
			if (0 == iNumLevels)
			{
				LOG_ERROR("Zero levels are defined in GameConfig.txt's level setting group!");
				return false;
			}

			if (iNumLevels > iMAX_LEVELS)
			{
				LOG_ERROR("There are more levels defined in GameConfig.txt than the framework supports. Adjust iMAX_LEVELS!");
				return false;
			}

			LOG_INFO("There are %d levels defined in GameConfig.txt", iNumLevels);
		}


		// Parse the level name and tilemap filenames from GameConfig.txt
		//
		for (uint16 i = 0; i < iNumLevels; ++i)
		{
			s_pFrameworkLevels[i] = NEWX(FrameworkLevelData_t);
			pLevel = GetSettingElement(pLevels, i);
			if (NULL == pLevel)
			{
				LOG_ERROR("Unable to get group: %d from the Levels array in GameConfig.txt LoadLevelsFromConfig failed...");
				return false;
			}

			s_pFrameworkLevels[i]->_sLevelName = GetStringFromSetting(pLevel, "name");
			if (s_pFrameworkLevels[i]->_sLevelName == "")
			{
				LOG_ERROR("Unable to parse level name from GameConfig.txt... Is the latest config file in the resources bundle?");
				return false;
			}

			s_pFrameworkLevels[i]->_sTilemapFilename = GetStringFromSetting(pLevel, "tilemap");
			if (s_pFrameworkLevels[i]->_sTilemapFilename == "")
			{
				LOG_ERROR("Unable to parse tilemap filename from GameConfig.txt... Is the latest config file in the resources bundle ?");
				return false;
			}

			// Load the Tilemap Data from the binary files in the resource bundle
			//
			{
				s_pFrameworkLevels[i]->_pBackgroundTilemap = LoadTileMapData(s_pFrameworkLevels[i]->_sTilemapFilename, true);
				if (NULL == s_pFrameworkLevels[i]->_pBackgroundTilemap)
				{
					LOG_ERROR("Unable to load level tilemap data.");
					return false;
				}
			}
		}
		



		// Build the tilemap VBO for each tilemap
		//
		{

		}

		// Finished!
 		return true;
	}


	bool DeallocateLevels()
	{
		for (int i = 0; i < s_iNumLevelsLoaded; ++i)
		{
			//DELETEX s_pFrameworkLevels[i]->_pBackgroundTilemap;
		}
		LOG_INFO("TODO: Delete the FrameworkLevels data!");
		return true;
	}
}
