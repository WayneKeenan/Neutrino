#pragma once
#include "Types.h"

namespace Neutrino {

	bool FileExists( const char * pFilename );
	uint32 GetFileSizeBytes(const char * pFilename);
	const char* LoadResource(const char* pFilename);
}