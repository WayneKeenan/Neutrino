#include "IniFile.h"
#include "File.h"
#include "Log.h"
#include "Input.h"
#include "Types.h"

#define INI_IMPLEMENTATION
#include "Ini/Ini.h"

namespace Neutrino
{
	static ini_t *s_pPlayerPrefsIni = NULL;
	static ini_t *s_pGameConfigIni = NULL;
	static const char *const s_sDefaultsSection = "defaults\0";
	static const char *const s_sInternalWidth = "internal_width\0";
	static const char *const s_sInternalHeight = "internal_height\0";
	static const char *const s_sViewportWidth = "viewport_width\0";
	static const char *const s_sViewportHeight = "viewport_height\0";
	static const char *const s_sMasterVolume = "master_volume\0";
	static const char *const s_sMusicVolume = "music_volume\0";
	static const char *const s_sSFXVolume = "sfx_volume\0";
	static const char *const s_sGameConfigFilename = "GameConfig.ini";

	static const char* s_aTextures[_iMAX_TEXTURES];
	static const char* s_aTilemaps[_iMAX_TEXTURES];
	static const char* s_aLevels[_iMAX_LEVELS];

	static int s_iNumTextures = 0;
	static int s_iNumLevels = 0;

	int GetNumLevels()
	{
		return s_iNumLevels;
	}

	int GetNumTextures()
	{
		return s_iNumTextures;
	}

	const char* GetTextureFilename(const int iIndex)
	{
		return s_aTextures[iIndex];
	}
	
	const char* GetTPageFilename(const int iIndex)
	{
		return s_aTilemaps[iIndex];
	}

	const char* GetLevel(const int iIndex)
	{
		return s_aLevels[iIndex];
	}

	bool CreateDefaultPlayerPrefsIni(const char *const pFilename)
	{
		s_pPlayerPrefsIni = ini_create(NULL);
		int iDefaultsSection = ini_section_add(s_pPlayerPrefsIni, s_sDefaultsSection, 0);

		if (iDefaultsSection < 0)
		{
			LOG_ERROR("Unable to create defaults Ini file. :/");
			return false;
		}

		char sStr[6] = {'\0'};
		snprintf(sStr, 6, "%d", _iDEFAULT_INTERNAL_WIDTH);
		ini_property_add(s_pPlayerPrefsIni, iDefaultsSection, s_sInternalWidth, 0, sStr, 0);
		memset(sStr, 0, 6);
		snprintf(sStr, 6, "%d", _iDEFAULT_INTERNAL_HEIGHT);
		ini_property_add(s_pPlayerPrefsIni, iDefaultsSection, s_sInternalHeight, 0, sStr, 0);

		memset(sStr, 0, 6);
		snprintf(sStr, 6, "%d", _iDEFAULT_VIEWPORT_WIDTH);
		ini_property_add(s_pPlayerPrefsIni, iDefaultsSection, s_sViewportWidth, 0, sStr, 0);

		memset(sStr, 0, 6);
		snprintf(sStr, 6, "%d", _iDEFAULT_VIEWPORT_HEIGHT);
		ini_property_add(s_pPlayerPrefsIni, iDefaultsSection, s_sViewportHeight, 0, sStr, 0);

		memset(sStr, 0, 6);
		snprintf(sStr, 6, "%.1f", 1.0f);
		ini_property_add(s_pPlayerPrefsIni, iDefaultsSection, s_sMasterVolume, 0, sStr, 0);
		ini_property_add(s_pPlayerPrefsIni, iDefaultsSection, s_sMusicVolume, 0, sStr, 0);
		ini_property_add(s_pPlayerPrefsIni, iDefaultsSection, s_sSFXVolume, 0, sStr, 0);

		int iInputCount = GetNumInputs();
		const char **pInputIDs = GetInputIDs();
		char sID[32] = {'\0'};
		char sVAL[8] = {'\0'};
		for (int i = 0; i < iInputCount; ++i)
		{
			memset(sID, 0, 32);
			memset(sVAL, 0, 8);
			snprintf(sID, 32, "%s", pInputIDs[i]);
			snprintf(sVAL, 8, "%d", GetInputMapping(i));
			ini_property_add(s_pPlayerPrefsIni, iDefaultsSection, sID, 0, sVAL, 0);
		}

		int iSize = ini_save(s_pPlayerPrefsIni, NULL, 0); // Find the size needed
		char *pData = (char *)malloc(iSize);
		iSize = ini_save(s_pPlayerPrefsIni, pData, iSize);
		ini_destroy(s_pPlayerPrefsIni);
		WriteToFile(pFilename, pData, iSize);
		free(pData);
		return true;
	}

	bool ParsePlayerPrefsIni(const char *const pFilename, NeutrinoPreferences_t *pPreferences)
	{
		char *pFileBytes = LoadFromFile(pFilename);
		ini_t *pIni = ini_load(pFileBytes, NULL);
		free(pFileBytes);

		bool bIniParsed = true;

		int iDefaults = ini_find_section(pIni, "defaults\0", 0);

		int iPropertyIndex = ini_find_property(pIni, iDefaults, s_sInternalWidth, 0);
		if (iPropertyIndex == INI_NOT_FOUND)
		{
			LOG_ERROR("Unable to find property index for defaults");
			return false;
		}
		pPreferences->_iInternalWidth = atoi(ini_property_value(pIni, iDefaults, iPropertyIndex));

		iPropertyIndex = ini_find_property(pIni, iDefaults, s_sInternalHeight, 0);
		if (iPropertyIndex == INI_NOT_FOUND)
		{
			LOG_ERROR("Unable to find property index for Internal Width");
			return false;
		}
		pPreferences->_iInternalHeight = atoi(ini_property_value(pIni, iDefaults, iPropertyIndex));

		iPropertyIndex = ini_find_property(pIni, iDefaults, s_sViewportWidth, 0);
		if (iPropertyIndex == INI_NOT_FOUND)
		{
			LOG_ERROR("Unable to find property index for Viewport Width");
			return false;
		}
		pPreferences->_iScreenWidth = atoi(ini_property_value(pIni, iDefaults, iPropertyIndex));

		iPropertyIndex = ini_find_property(pIni, iDefaults, s_sViewportHeight, 0);
		if (iPropertyIndex == INI_NOT_FOUND)
		{
			LOG_ERROR("Unable to find property index for Viewport Height");
			return false;
		}
		pPreferences->_iScreenHeight = atoi(ini_property_value(pIni, iDefaults, iPropertyIndex));

		iPropertyIndex = ini_find_property(pIni, iDefaults, s_sMasterVolume, 0);
		if (iPropertyIndex == INI_NOT_FOUND)
		{
			LOG_ERROR("Unable to find property index for Master Volume");
			return false;
		}
		pPreferences->_fMasterVolume = (float)atof(ini_property_value(pIni, iDefaults, iPropertyIndex));

		iPropertyIndex = ini_find_property(pIni, iDefaults, s_sMusicVolume, 0);
		if (iPropertyIndex == INI_NOT_FOUND)
		{
			LOG_ERROR("Unable to find property index for Music Volume");
			return false;
		}
		pPreferences->_fMusicVolume = (float)atof(ini_property_value(pIni, iDefaults, iPropertyIndex));

		iPropertyIndex = ini_find_property(pIni, iDefaults, s_sSFXVolume, 0);
		if (iPropertyIndex == INI_NOT_FOUND)
		{
			LOG_ERROR("Unable to find property index for SFX Volume");
			return false;
		}
		pPreferences->_fSampleVolume = (float)atof(ini_property_value(pIni, iDefaults, iPropertyIndex));

		int iInputCount = GetNumInputs();
		const char **pInputIDs = GetInputIDs();
		char sID[32] = {'\0'};
		for (int i = 0; i < iInputCount; ++i)
		{
			memset(sID, 0, 32);
			snprintf(sID, 32, "%s", pInputIDs[i]);
			iPropertyIndex = ini_find_property(pIni, iDefaults, sID, 0);
			if (iPropertyIndex == INI_NOT_FOUND)
			{
				LOG_ERROR("Unable to parse Ini file");
				return false;
			}
			int iMapping = atoi(ini_property_value(pIni, iDefaults, iPropertyIndex));
			SetInputMapping((Neutrino::eKeyboard_GameInputs)i, iMapping);
		}
		return true;
	}

	bool LoadGameConfigIni()
	{
		if (!ResourceFileExists(s_sGameConfigFilename))
		{
			LOG_ERROR("GameConfig.ini missing!");
			return false;
		}
		const char *pFileBytes = LoadResourceBytes(s_sGameConfigFilename);
		s_pGameConfigIni = ini_load(pFileBytes, NULL);
		DELETEX[] pFileBytes;
	
		int iSection = ini_find_section(s_pGameConfigIni, "textures\0", 0);
		int iCount = ini_property_count(s_pGameConfigIni, iSection);
		s_iNumTextures = iCount/2;	// (Spritemap and png file)

		char sID[32] = { '\0' };
		for (int i=0; i<iCount/2; ++i)
		{
			memset(sID, 0, 32);
			snprintf(sID, 32, "file%d", i+1);
			int iPropertyIndex = ini_find_property(s_pGameConfigIni, iSection, sID, 0);
			s_aTextures[i] = ini_property_value(s_pGameConfigIni, iSection, iPropertyIndex);
			
			memset(sID, 0, 32);
			snprintf(sID, 32, "info%d", i+1);
			iPropertyIndex = ini_find_property(s_pGameConfigIni, iSection, sID, 0);
			s_aTilemaps[i] = ini_property_value(s_pGameConfigIni, iSection, iPropertyIndex);
		}

		iSection = ini_find_section(s_pGameConfigIni, "levels\0", 0);
		iCount = ini_property_count(s_pGameConfigIni, iSection);
		s_iNumLevels = iCount/2; // (Name and tilemap)

		for (int i=0; i<iCount/2; ++i)
		{
			// TODO. This isn't parsing the name yet. In fact the levels should be a struct of data in their own file?'
			memset(sID, 0, 32);
			snprintf(sID, 32, "tilemap%d", i+1);
			int iPropertyIndex = ini_find_property(s_pGameConfigIni, iSection, sID, 0);
			s_aLevels[i] = ini_property_value(s_pGameConfigIni, iSection, iPropertyIndex);
		}
		return true;
	}

	void* LoadTPageIni(const char* const pFilename)
	{
		if (!ResourceFileExists(pFilename))
		{
			LOG_ERROR("Load TPage ini, unable to find %s in resources bundle", pFilename);
			return false;
		}

		const char* pFileBytes = LoadResourceBytes(pFilename);
		ini_t* pIni = ini_load(pFileBytes, NULL);
		DELETEX[] pFileBytes;

		if (nullptr == pIni)
			LOG_ERROR("Unable to load %s, possible parsing error in the ini file", pFilename);

		return (void*)pIni;
	}


	int GetTPageWidth(void* pIni)
	{
		int iSection = ini_find_section((ini_t*)pIni, "tpage\0", 0);
		if (iSection < 0)
		{
			LOG_ERROR("Unable to parse global section in TPage");
			return -1;
		}
		int iProperty = ini_find_property((ini_t*)pIni, iSection, "width\0", 0);
		if (iProperty < 0) 
		{
			LOG_ERROR("Unable to parse global section in TPage");
			return -1;
		}
		return atoi(ini_property_value((ini_t*)pIni, iSection, iProperty));
	}


	int GetTPageHeight(void* pIni)
	{
		int iSection = ini_find_section((ini_t*)pIni, "tpage\0", 0);
		if (iSection < 0)
		{
			LOG_ERROR("Unable to parse global section in TPage");
			return -1;
		}
		int iProperty = ini_find_property((ini_t*)pIni, iSection, "Height\0", 0);
		if (iProperty < 0)
		{
			LOG_ERROR("Unable to parse global section in TPage");
			return -1;
		}
		return atoi(ini_property_value((ini_t*)pIni, iSection, iProperty));
	}

	int GetTPageSpriteCount(void* pIni_t)
	{
		// This is minus two because Ini.h assumes there's a global section so 
		// indexes from 1. However, the TPage format's initial section starts as [tpage]
		// which counts as a new section, so we've got an additional section before
		// any sprites...
		return ini_section_count((ini_t*)pIni_t) - 2;
	}
	
	bool GetSpriteDetailsFromTPageIni(void* pIni_t, int iSprCount, int* iWidth, int* iHeight, int* iX, int* iY, const char** sFilename)
	{
		int iSectionCount = iSprCount + 2;	// See above
		int iProperty;
		*sFilename = ini_section_name((ini_t*)pIni_t, iSectionCount);
		iProperty = ini_find_property((ini_t*)pIni_t, iSectionCount, "x\0", 0);
		*iX = atoi(ini_property_value((ini_t*)pIni_t, iSectionCount, iProperty));
		iProperty = ini_find_property((ini_t*)pIni_t, iSectionCount, "y\0", 0);
		*iY = atoi(ini_property_value((ini_t*)pIni_t, iSectionCount, iProperty));
		iProperty = ini_find_property((ini_t*)pIni_t, iSectionCount, "width\0", 0);
		*iWidth = atoi(ini_property_value((ini_t*)pIni_t, iSectionCount, iProperty));
		iProperty = ini_find_property((ini_t*)pIni_t, iSectionCount, "width\0", 0);
		*iHeight = atoi(ini_property_value((ini_t*)pIni_t, iSectionCount, iProperty));
		return true;
	}

	bool UnloadPlayerPrefsIni()
	{
		ini_destroy(s_pPlayerPrefsIni);
		LOG_INFO("PlayerPrefs.ini Destroyed.");
		return true;
	}

	bool UnloadGameConfigIni()
	{
		ini_destroy(s_pGameConfigIni);
		LOG_INFO("GameConfig.ini Destroyed.");
		return true;
	}
}
