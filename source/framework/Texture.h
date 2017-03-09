#pragma once
#include "GLUtils.h"

namespace Neutrino {

	// Defines the coordinates of an individual sprite within a packed texture page.
	// Texture pages should be generated with TexturePacker (https://www.codeandweb.com/texturepacker)
	// See the pre-defined exporter format for this framework in ./external_dependencies/texturepacker_exporters/Neutrino
	// 
	// Params:
	// 		x & y: Pixel coordinates within the texture
	// 		_fHalfWidth & _fHalfHeight: Pixel Dimensions of the sprite / 2.0 - used as-is by the Sprite functions to generate rendered quad   
	//   	X_S* X_T*: pre-cacled UV coords for the sprite within the tpage
	//    
	typedef struct TPageSpriteInfo_t
	{
		float	_fX_S;		// Bot Left UV
		float	_fY_T;
		float _fX_SnS;	// Top Right UV
		float _fY_TnT;
		float	_fHalfWidth;
		float _fHalfHeight;
	} TPageSpriteInfo_t;


	// Defines the parameters of an individual texture page
	// 
	// 		_iTextureID: the generated GLuint when the texture is bound through OGL
	//  	_iWidth: texture's width
	//  	_iHeight: texture height
	//  	_iMaxSprites: number of sprites contained in this texture page
	//  	aSpriteInfo: array containing the relevent coords for each sprite within the tpage.
	//  
	typedef struct TPage_t
	{
		char* _sTextureFilename;
		char* _sTPageFilename;
		GLuint _iTextureID;
		uint16 _iWidth;
		uint16 _iHeight;
		uint16 _iMaxSprites;
		TPageSpriteInfo_t* aSprintInfo;
	} TPage_t;


	// LoadTexturesFromConfigFile()
	// 		
	//		The framework assumes textures come in two parts: 1) the binary image file, and 2) an associated tpage info 
	//  	file that lists all the sprites that have been packed into the image. 
	//   
	//		* The binary for the texture can be any format that SDL image supports, but PNG is probably best
	//   	* The tpage info file contains sprite dimensions and pixel offsets, see the top of this file for formatting information
	//    
	//    	This function looks for GameConfig.txt in the mounted resources bundle, and checks for texture definitions contained within.
	//     	When it finds one, it'll attempt to load the paired files, building the relevant TPage_t info (and TPageSpriteInfo_t array)
	//      before sending the image binary to OpenGL, for binding. 
	//      
	//      Any failure during each stage will terminate the framework. 
	//      
	//      The framework assumes that there won't be many textures and that it will be safe to load them all into memory during 
	//      CoreInit(). This may or may not be wise, so go easy on the 4096*4096*32 textures ;P
	//      
	//      This function will trigger GLUtils to create a set of VBOs for the texture. 
	//      
	bool LoadTexturesFromConfigFile();


	// GetSpriteInfo
	// 		Returns pointer to the correct TPageSpriteInfo_t structure for a given sprite index in the packed texture page
	//   	Sprite will call this to grab UV and Dimension info 
	const TPageSpriteInfo_t* GetSpriteInfo(const uint16 iTextureSet, const uint16 iSpriteCount);

	// GetTextureSet
	//		Will return the index of the packed texture page for the given ID. This index can be passed to GetSpriteInfo if 
	// 		you only know the texture's OGL id
	uint16 GetTextureSet( const GLuint iTextureID );

	// ClearAllTextures()
	// 		Free memory allocated for TPage_t arrays created during LoadTexturesFromConfigFile(), delete the texture
	//   	and call the GLUtils framework to delete the associated VBOS
	void DeallocateAllTextures();

	// GetLoadedTextureCount()
	//		Returns the number currently loaded textures
	uint8 GetLoadedTextureCount();

	// GetTpage()
	//		Returns the TPage_t at index. Used by the tilemap editor to dig into the loaded textures so it can 
	//		present the available tiles. 
	const TPage_t* GetTPage(const uint8 iTextureSet);
}
