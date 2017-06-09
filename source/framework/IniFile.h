#pragma once
#include "Types.h"

namespace Neutrino
{
	// CreateDefaultPlayerPrefsIni
	//		On first load the player won't have a pre-existing preferences file. This function creates a new .ini 
	//		for the player, with the defaults defined int Types.h'
	bool CreateDefaultPlayerPrefsIni(const char* const pFilename);

	// ParsePlayerPrefsIni
	//		This function will load and parse an existing preferences file and populate the relevant settings
	// 		in the NeutrinoPreferences_t struct, that the framework uses.
	bool ParsePlayerPrefsIni(const char* const pFilename, NeutrinoPreferences_t* pPreferences);

	// LoadGameConfigIni
	//		This loads GameConfig.ini from the resources bundle and stores the relevant info for later retrival.
	bool LoadGameConfigIni();

	// LoadGameConfigIni
	//		This loads GameConfig.ini from the resources bundle and stores the relevant info for later retrival.
	void* LoadTPageIni(const char* const pFilename);

	// GetNumTextures
	//		Returns the number of textures defined in GameConfig.ini
	int GetNumTextures();

	// GetNumLevels
	//		Returns the number of levels defined in GameCofig.ini
	int GetNumLevels();

	int GetTPageWidth(void* pIni);

	int GetTPageHeight(void* pIni);

	int GetTPageSpriteCount(void* pIni_t);

	bool GetSpriteDetailsFromTPageIni(void* pIni_t, int iSprCount, int iWidth, int iHeight, int iX, int iY, const char* sFilename);

	// GetTextureFilename()
	//		Returns the nth filename defined in GameConfig.ini
	const char* GetTextureFilename(const int iIndex);

	//	GetTPageFilename
	//		Returns the nth TPage filename defined in GameConfig.ini
	const char* GetTPageFilename(const int iIndex);

	//	GetNumLevels
	//		Returns the nth level defined in GameConfig.ini
	const char* GetLevel(const int iIndex);

	// UnloadGameConfigIni
	//		Calls the ini parser destroy to remove the stored PlayerPrefs file. 
	bool UnloadPlayerPrefsIni();

	// UnloadGameConfigIni
	//		Calls the ini parser destroy to remove the stored GameConfig file. 
	bool UnloadGameConfigIni();
}

