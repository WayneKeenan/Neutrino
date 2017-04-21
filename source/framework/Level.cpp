#include "Level.h"
#include "Log.h"
#include "ConfigFile.h"
#include "Assert.h"
#include "Types.h"
#include "Memory.h"
#include "File.h"
#include "GLUtils.h"
#include "Texture.h"
#include "Sprite.h"
#include "string.h"

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

			// Get the level name
			s_pFrameworkLevels[i]->_sLevelName = GetStringFromSetting(pLevel, "name");
			if (strcmp(s_pFrameworkLevels[i]->_sLevelName,"") == 0)
			{
				LOG_ERROR("Unable to parse level name from GameConfig.txt... Is the latest config file in the resources bundle?");
				return false;
			}

			// Get the filename of the tilemap binary file
			s_pFrameworkLevels[i]->_sTilemapFilename = GetStringFromSetting(pLevel, "tilemap");
			if (strcmp(s_pFrameworkLevels[i]->_sTilemapFilename, "") == 0)
			{
				LOG_ERROR("Unable to parse tilemap filename from GameConfig.txt... Is the latest config file in the resources bundle ?");
				return false;
			}

			// Load the Tilemap Data from the binary file in the resource bundle
			{
				s_pFrameworkLevels[i]->_pBackgroundTilemap = LoadTileMapData(s_pFrameworkLevels[i]->_sTilemapFilename, true);
				if (NULL == s_pFrameworkLevels[i]->_pBackgroundTilemap)
					return false;

				LOG_INFO("Level number: %d", i);
				LOG_INFO("- Name: %s", s_pFrameworkLevels[i]->_sLevelName);
				LOG_INFO("- Tilemap: %s", s_pFrameworkLevels[i]->_sTilemapFilename);
				LOG_INFO("- Tilemap width: %d", s_pFrameworkLevels[i]->_pBackgroundTilemap->_iLevelWidth);
				LOG_INFO("- Tilemap height: %d", s_pFrameworkLevels[i]->_pBackgroundTilemap->_iLevelHeight);
				LOG_INFO("- Tilemap Grid Size: %.2f", s_pFrameworkLevels[i]->_pBackgroundTilemap->_fGridSize);
			}


			// Find the texture ID for the tpage referenced by this tilemap
			{
				s_pFrameworkLevels[i]->_iTextureID = GetTextureID(s_pFrameworkLevels[i]->_pBackgroundTilemap->_sTextureFilename);
				if (s_pFrameworkLevels[i]->_iTextureID == iU32_BAD)
				{
					LOG_ERROR("Tilemap creation failed, unable to find the texture ID for the tilemap's texture");
					return false;
				}
			}

			// Build the static tilemap VBO for this tilemap
			{
				s_pFrameworkLevels[i]->_iTilemapSize = s_pFrameworkLevels[i]->_pBackgroundTilemap->_iLevelWidth * s_pFrameworkLevels[i]->_pBackgroundTilemap->_iLevelHeight;
				s_pFrameworkLevels[i]->_iStaticVBO_Index = GLUtils::CreateTilemapVBO(s_pFrameworkLevels[i]->_iTilemapSize);
				BuildSpriteArrayAndPopulateStaticVBO(s_pFrameworkLevels[i]->_iTextureID, s_pFrameworkLevels[i]->_pBackgroundTilemap, s_pFrameworkLevels[i]->_iStaticVBO_Index);
				LOG_INFO("Static VBO built and populated!");
			}
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


	void DrawTilemap()
	{
		// TODO: This needs to get whatever the current level tilemap is from somewhere. GameGlobals?
		const int iLevelDataIndex = 0;
		GLUtils::RenderTilemapVBO(s_pFrameworkLevels[iLevelDataIndex]->_iTilemapSize, s_pFrameworkLevels[iLevelDataIndex]->_iTextureID, s_pFrameworkLevels[iLevelDataIndex]->_iStaticVBO_Index);
	}
}
