#pragma once 
#include "libconfig.h"
#include "Types.h"

// Libconfig is used for all configuration files supported by this framework. There's an argument to 
// rip all this out and use JSON, but JSON smells and this library is easily installed through 
// a package manager on most linux systems (and Windows, I looked)
//
// Individual systems will directly use Libconfig to load files and parse data (such as texture pages 
// pulling out sprite data) but the framework itself looks for GameConfig.txt, which lists where
// such files can be found in the resource bundle. 
//
// This file contains wrapper functions to libconfig to simplify access to this data. 
//
// GameConfig.txt should be loaded as part of CoreInit, once the resource bundle has been mounted. It
// stays resident until the framework terminates. 
//
// 
namespace Neutrino
{

	// LoadConfigFile()
	// 		Part of CoreInit, loads the libconfig format config file into memory and stores it for other 
	// 		parts of the game to grab what they need as and when.
	bool LoadConfigFile();

	// GameConfigGetInt()
	// 		Utility function, get an int value from the game config file, and store in iValueStore. Calling 
	// 		function should check the return status for success/fail
	bool GameConfigGetInt( const char* pParam, int* iValueStore );

	// ConfigGetInt()
	// 		As above, but wrapper to get an int safely from an externally loaded config file
	bool ConfigGetInt( config_t* pCfg, const char* pParam, int* iValueStore );

	// GameConfigGetString()
	// 		Utility function, get a string value from the game config file.  Returns NULL pointer on failure
	const char* GameConfigGetString( const char* pParam); 

	// ConfigGetString()
	// 		Wrapper to get a string from an externally loaded libconfig file. Returns NULL pointer on failure
	const char* ConfigGetString( config_t* pCfg, const char* pParam );

	// ConfigGetList()
	// 		Wrapper, get the root element of a list from an externally loaded config file. Returns NULL on failure
	const config_setting_t* ConfigGetList(const config_t* pCfg, const char* pParam);

	// GameConfigGetSetting()
	// 		Search for the list of elements identified by pParam. Returns the config_setting_t structure.
	// 		Returns NULL on failure. 
	const config_setting_t* GameConfigGetSetting(const char* pParam);

	// GameConfigGetStringFromSetting()
	// 		Get a string element from a config_setting_t (list element) structure. Returns NULL pointer on failure. Returns NULL pointer on failure
	const char* GetStringFromSetting(const config_setting_t* pSetting, const char* pParam );

	// GetSettingElement()
	// 		Get the iCount element from a list. Will return NULL on failure. 
	const config_setting_t* GetSettingElement(const config_setting_t* pSetting, int iCount);

	// GetSettingLength()
	//		Returns the length of a group, or list config setting, or 0 for other element types
	inline const int GetSettingLength(const config_setting_t* pSetting) { return config_setting_length(pSetting); };

}
