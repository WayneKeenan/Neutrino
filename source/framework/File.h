#pragma once
#include "Types.h"


namespace Neutrino {

	bool FileExists( const char * pFilename );
	bool MountResources( const char* pFilename );
	uint32 GetFileSizeBytes(const char * pFilename);
	const char* LoadResourceBytes(const char* pFilename);
}