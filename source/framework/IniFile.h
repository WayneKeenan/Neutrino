#pragma once
#include "Types.h"

namespace Neutrino
{
	bool CreateDefaultPlayerPrefsIni(const char* const pFilename);
	bool ParsePlayerPrefsIni(const char* const pFilename, NeutrinoPreferences_t* pPreferences);
}