#pragma once
#include "Types.h"


namespace Neutrino {

	// FileExists()
	// 		C Style stat check for existence of file in the user's filesystem. 
	//   	This does not check for existence of a file in the resource bundle (use PHYSFS_exists() instead)
	bool FileExists( const char * pFilename );

	// MountResources()
	// 		Mount the game's resource bundle from the given filepath. 
	//   	This should be mounted as early as possible in the bootstrapping of the engine
	bool MountResources( const char* pFilename );

	// GetFileSizeBytes()
	// 		This returns the file size in bytes of a given file in the RESOURCES bundle (not the filesystem)
	uint32 GetFileSizeBytes(const char * pFilename);

	// LoadResourceBytes()
	// 		'Load' a given file from the resource bundle, returning a pointer to alocated bytes
	//   	Note: no allocation tracking is undertaken by this function. It's the responsibility
	//    	of the calling function to free memory allocated by this load. 
	const char* LoadResourceBytes(const char* pFilename);
}