#pragma once 
#include <vector>
#include "Types.h"
#include "GLUtils.h"

namespace Neutrino {
	// Version number for the current TileMapData file format 
	static const float s_fTMD_Version = 0.2f;

	// TileMapData_t defines the file format for a level's background tile map. 
	// To create or amend a tile map, press F3 in debug builds to enter the Map editor. 
	//
	typedef struct TileMapData_t
	{
		float _fVersion = s_fTMD_Version;
		char* _sTextureFilename;
		char* _sFilepathBuf;
		char* _sFilenameBuf;
		float _fGridSize;
		uint16 _iLevelWidth;
		uint16 _iLevelHeight;
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
		uint8 _iStaticVBO_Index;
		uint32 _iTilemapSize;
		GLuint _iTextureID;
		const TileMapData_t* _pBackgroundTilemap;
	}FrameworkLevelData_t;


	// LoadLevelsFromConfig()
	//		Parses the GameConfig.txt file in the resources bundle for the generic level data
	//		to load on init. Stores this in an array of FrameworkLevelData_t that the game 
	//		logic can grab. 
	bool LoadLevelsFromConfig();

	// DeallocateLevels()
	//		Deallocates all the static arrays created during init
	bool DeallocateLevels();

	// DrawTilemap()
	// 		Passes all relevant tilemap info to the OGL renderer so it can be rendered this frame
	void DrawTilemap();
}
