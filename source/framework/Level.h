#pragma once 
#include <vector>
#include "Types.h"
#include "GLUtils.h"

namespace Neutrino {
	// Version number for the current TileMapData file format 
	static const float s_fTMD_Version= 0.1f;

	// TileMapData_t defines the file format for a level's background tile map. 
	// To create or amend a tile map, press F3 in debug builds to enter the Map editor. 
	//
	typedef struct TileMapData_t
	{
		float _fVersion = s_fTMD_Version;
		char* _sTextureFilename;
		char* _sFilepathBuf;
		char* _sFilenameBuf;
		uint16 _LevelWidth;
		uint16 _LevelHeight;
		int16* _aTileMap;
	}TileMapData_t;

	// Level structures are game specific, but as the framework provides a bunch of core
	// types this provides a convenient wrapper that can be embedded into a game specific
	// structure. 
	// 
	// TODO: 
	//	- Add audio banks
	//	- Add any physics information for background tilemaps
	//	
	typedef struct FrameworkLevelData_t
	{
		const char* _sLevelName;						// invalid when game config file is released. 
		const char* _sTilemapFilename;
		const TileMapData_t* _pBackgroundTilemap;
		GLuint*	_iStaticVBO_ID;
	}FrameworkLevelData_t;


	// LoadLevelsFromConfig()
	//		Parses the GameConfig.txt file in the resources bundle for the generic level data
	//		to load on init. Stores this in an array of FrameworkLevelData_t that the game 
	//		logic can grab. 
	bool LoadLevelsFromConfig();

	// DeallocateLevels()
	//		Deallocates all the static arrays created during init
	bool DeallocateLevels();
}
