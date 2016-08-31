#pragma once
#include "GLUtils.h"

namespace Neutrino {

	typedef struct TPageSpriteInfo_t
	{
		uint16	iX;
		uint16	iY;
		float	fHalfWidth;
		float 	fHalfHeight;
	} TPageSpriteInfo_t;


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
	GLuint GetTextureID(int iCount);

	bool LoadTexturesFromConfigFile();
}