#pragma once
#include "GLUtils.h"

namespace Neutrino {

	// LoadTexture()
	// 		Loads a texture from the resource file and binds it to an OGL texture
	bool LoadTexture( const char* pFilename );

	// GetTextureID()
	// 		Returns the GLuint identifier for the loaded texture
	GLuint GetTextureID(int iCount);

}