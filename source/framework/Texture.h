#pragma once
#include "GLUtils.h"

namespace Neutrino {

	// Defines the coordinates of an individual sprite within a packed texture page.
	// Texture pages should be generated with TexturePacker (https://www.codeandweb.com/texturepacker)
	// See the pre-defined exporter formates for this app in external_dependencies/texturepacker_exporters/Neutrino
	// 
	// Params:
	// 		x & y: Pixel coordinates within the texture
	// 		fHalfWidth & fHalfHeight: Pixel Dimensions of the sprite / 2.0 - used as-is by the Sprite functions to generate rendered quad   
	// 
	// TO_DO:
	// 		Pre-calc the UV settings we'll need to copy into the sprite when rendering...
	//   
	typedef struct TPageSpriteInfo_t
	{
		uint16	iX;
		uint16	iY;
		float	fHalfWidth;
		float 	fHalfHeight;
	} TPageSpriteInfo_t;


	// Defines the parameters of an individual texture page
	// 
	// 		iTextureID: the generated GLuint when the texture is bound through OGL
	//  	iWidth: texture's width
	//  	iHeight: texture height
	//  	iMaxSprites: number of sprites contained in this texture page
	//  	aSpriteInfo: array containing the relevent coords for each sprite within the tpage
	//  
	// TO_DO:
	// 		The TPage_t should probably store the base pointers to the individual sprite arrays that'll be used to render any sprites
	//   	defined here.
	//  
	typedef struct TPage_t
	{
		GLuint iTextureID;
		uint16 iWidth;
		uint16 iHeight;
		uint16 iMaxSprites;
		TPageSpriteInfo_t* aSprintInfo;
	} TPage_t;


	// GetTextureID()
	// 		Returns the GLuint identifier for the loaded texture
	//   
	//  TO_DO:
	//  	This should be removed from accesible scope. The framework should iterate over each texture page
	//   	and call a DrawSprites function for it...
	GLuint GetTextureID(int iCount);



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
	bool LoadTexturesFromConfigFile();


	// TO_DO: we need clean up functions for all these loaded textures...
}