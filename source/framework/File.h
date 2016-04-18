#pragma once
#include "Types.h"

namespace Neutrino {

	bool FileExists( const char * pFilename );
	
	uint32 GetFileByteSize( const char * pFilename );
}