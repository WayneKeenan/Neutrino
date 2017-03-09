#pragma once 
#include <vector>
#include "Types.h"

namespace Neutrino {
	// ----------------------------------------------------------------------------------------
	// Levels are game specific, the functions contained in this header handle the parsing 
	// & loading of data from runtime directories and/or the resource bundle. 
	// ----------------------------------------------------------------------------------------

	// Version number for the current TileMapData file format 
	//
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

}
