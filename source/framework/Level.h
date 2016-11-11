#pragma once 

namespace Neutrino {


	// Container for Level meta-data. Contents will be parsed to create a level during OnLevelEnter gamestate. 
	//
	// Params:
	// 	 pBackgroundMap: The tilemap for the level background. This should be created in TileD, and run through 
	// 	 tiled_preprocess.py, which can be found in the Neutrino_Assets repo. 
	//
	typedef struct LevelFileDetails_t
	{
		const char* pBackgroundMap;
	} LevelFileDetails_t;


	// ParseBackgroundMapFile()
	// 		Loads a .txt background map file and generates a static VBO holding the tilemap for the level. 
	// TODO:
	// 	  There wlil most likely be variants of these...
	// 	  How do we handle parallax? 
	// 	  What about levels with no background?
	// 	  See notes in your pad...
	bool ParseBackgroundMapFile(const char* pFilename);


	// LoadLeveldetailsFromConfigFile()
	//
	// 		Part of the CoreInit phase. Grab each of the level's various params/meta-data and store them 
	// 		in an array of LevelFileDetails_t so we can quickly build a level during gamestate transition.
	bool LoadLevelDetailsFromConfigFile();

}
