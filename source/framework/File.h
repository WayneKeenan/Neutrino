#pragma once
#include "Types.h"
#include "Level.h"


namespace Neutrino {

	// FileExists()
	// 		C Style stat check for existence of file in the user's filesystem. 
	//   	This does not check for existence of a file in the resource bundle (use ResourceFileExists instead)
	bool FileExists( const char * pFilename );

	// ResourceFileExists()
	// 		Check for existence of file in the resource bundle 
	bool ResourceFileExists( const char * pFilename );

	// MountResources()
	// 		Mount the game's resource bundle from the given filepath. 
	//   	This should be mounted as early as possible in the bootstrapping of the engine
	bool MountResources( const char* pFilename );


	// UnountResources()
	// 		Attempt to remove pFilename (previously mounted bundle) from the PHYSFS search path. 
	bool UnmountResources( const char* pFilename );


	// GetFileSizeBytes()
	// 		This returns the file size in bytes of a given file in the RESOURCES bundle (not the filesystem)
	uint32 GetFileSizeBytes(const char * pFilename);

	// LoadResourceBytes()
	// 		'Load' a given file from the resource bundle, returning a pointer to alocated bytes
	//   	Note: no allocation tracking is undertaken by this function. It's the responsibility
	//    	of the calling function to free memory allocated by this load. 
	const char* LoadResourceBytes(const char* pFilename);

#if defined DEBUG
	// SaveTileMapData()
	// 		Takes a TileMapData_t and writes it to a file at the given path. This is a destructive operation
	// 		although the saved data is reloaded and a simple comparison done to check the file's integrity. 
	// 		The function returns the result of that comparison
	bool SaveTileMapData(const TileMapData_t* pData);
#endif

	// LoadTileMapData()
	//		Populates a TileMapData_t struct with data previously written to a file. No integrity checking. 	
	const TileMapData_t* LoadTileMapData(const char* sFilePathAndName, const bool bFromResourceBundle = false);
}
