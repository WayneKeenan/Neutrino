#pragma once 
#include "libconfig.h"

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
namespace Neutrino
{

	// LoadConfigFile()
	// 		Part of CoreInit, loads the libconfig format config file into memory and stores it for other 
	// 		parts of the game to grab what they need as and when.
	bool LoadConfigFile();

	// GameConfigGetInt()
	// 		Utility function, get an int value from the game config file.
	bool GameConfigGetInt( const char* pParam, int* iValueStore );

	// GameConfigGetString()
	// 		Utility function, get a string value from the game config file. 
	const char* GameConfigGetString( const char* pParam); 

	const char* ConfigGetString( config_t* pCfg, const char* pParam );

	bool ConfigGetInt( config_t* pCfg, const char* pParam, int* iValueStore );

	// GameConfigGetStringFromSetting()
	// 		Get a string element from a config_setting_t (list element) structure
	const char* GameConfigGetStringFromSetting(const config_setting_t* pSetting, const char* pParam );

	// GameConfigGetList()
	// 		Search for the list of elements identified by pParam. Returns the config_setting_t structure.
	const config_setting_t* GameConfigGetList(const char* pParam);

	// GameConfigGetListElement()
	// 		Get the iCount element from a list.
	const config_setting_t* GameConfigGetListElement(const config_setting_t* pList, int iCount);

}
