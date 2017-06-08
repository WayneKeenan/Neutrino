#include "IniFile.h"
#include "File.h"
#include "Log.h"
#include "Types.h"
#include "Input.h"

#define INI_IMPLEMENTATION
#include "Ini/Ini.h"

namespace Neutrino
{
	static ini_t* s_pPlayerPrefsIni = NULL;
	static const char* const s_sDefaultsSection = "defaults\0";
	static const char* const s_sInternalWidth = "internal_width\0";
	static const char* const s_sInternalHeight = "internal_height\0";
	static const char* const s_sViewportWidth = "viewport_width\0";
	static const char* const s_sViewportHeight = "viewport_height\0";


	bool CreateDefaultPlayerPrefsIni(const char* const pFilename)
	{
		s_pPlayerPrefsIni = ini_create(NULL);
		int iDefaultsSection = ini_section_add(s_pPlayerPrefsIni, s_sDefaultsSection, 0);
		
		if( iDefaultsSection < 0)
		{
			LOG_ERROR("Unable to create defaults Ini file. :/");
			return false;
		}

		char sStr[6] = { '\0' };
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

		int iInputCount = GetNumInputs();
		const char** pInputIDs = GetInputIDs();
		char sID[32] = { '\0' };
		char sVAL[8] = { '\0' };
		for (int i = 0; i < iInputCount; ++i)
		{
			memset(sID, 0, 32);
			memset(sVAL, 0, 8);
			snprintf(sID, 32, "%s", pInputIDs[i]);
			snprintf(sVAL, 8, "%d", GetInputMapping(i));
			ini_property_add(s_pPlayerPrefsIni, iDefaultsSection, sID, 0, sVAL, 0);
		}

		int iSize = ini_save(s_pPlayerPrefsIni, NULL, 0); // Find the size needed
		char* pData = (char*)malloc(iSize);
		iSize = ini_save(s_pPlayerPrefsIni, pData, iSize);
		ini_destroy(s_pPlayerPrefsIni);
		WriteToFile(pFilename, pData, iSize);
		free(pData);
		return true;
	}
}