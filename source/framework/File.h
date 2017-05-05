#pragma once
#include "Types.h"
#include "Level.h"
#include "ShaderUtils.h"

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

	// LoadTileMapData()
	//		Populates a TileMapData_t struct with data previously written to a file. No integrity checking. 	
	// 		This function is partially overloaded, it can load a file directly (for the map editor) or from 
	// 		the resource bundle for the game runtime. bFromResourceBundle toggles this behaviour.  
	// 		If bResourceBundle is true, FilePathAndName does not need to include the path, the filename in the 
	// 		resource bundle will be sufficient. 
	const TileMapData_t* LoadTileMapData(const char* sFilePathAndName, const bool bFromResourceBundle = false);

	//	LoadPostProcessSettings
	//		Loads and parses a binary post process settings file and returns a populated PostProcessSettings_t struct. 
	//		This struct needs to be freed by the caller. 
	PostProcessSettings_t* LoadPostProcessSettings(const char* sFilePathAndName, const bool bFromResourceBundle = false);

	// WriteToFile
	//		This will output the passed pBytes string to a file with zero checking. Used for writing Ini files 
	//		out during init and via editor modes. 
	void WriteToFile(const char* const pFilePathAndName, const char* const pBytes, const int iSize);

	// LoveFromFile
	//		Basic wrapper to load raw bytes from a file
	char* LoadFromFile(const char* const pFilename);

	//	GetFileSize
	//		Returns te file size in bytes of a file outside the resources bundle
	long int GetFileSize(const char* const pFilename);

#if defined DEBUG
	// SaveTileMapData()
	// 		Takes a TileMapData_t and writes it to a file at the given path. This is a destructive operation
	// 		although the saved data is reloaded and a simple comparison done to check the file's integrity. 
	// 		The function returns the result of that comparison
	bool SaveTileMapData(const TileMapData_t* pData);

	// SavePostProcessSettings()
	//		The Debug Overlay provides an interface to change all the shader uniforms related to the composite
	//		of the final image to screen. This function will save those params to a binary file
	bool SavePostProcessSettings(const PostProcessSettings_t* pData, const char* pFilepath);
#endif
}
