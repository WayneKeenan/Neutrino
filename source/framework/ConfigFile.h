#pragma once 

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
}
