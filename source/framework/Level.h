#pragma once 

namespace Neutrino {

	// ParseBackgroundMapFile()
	// 		Loads a .txt background map file and generates a static VBO holding the tilemap for the level. 
	// TODO:
	// 	  There wlil most likely be variants of these...
	// 	  How do we handle parallax? 
	// 	  What about levels with no background?
	// 	  See notes in your pad...
	bool GenerateTileMapFromFile(const char* pFilename);


	// LoadLeveldetailsFromConfigFile()
	//
	// 		Part of the CoreInit phase. Grab each of the level's various params/meta-data and store them 
	// 		in an array of LevelFileDetails_t so we can quickly build a level during gamestate transition.
	bool LoadLevelDetailsFromConfigFile();

	void DeallocateLevels();
}
