#pragma once
#include "GLUtils.h"

namespace Neutrino {

	// GetTextureID()
	// 		Returns the GLuint identifier for the loaded texture
	GLuint GetTextureID(int iCount);

	bool LoadTexturesFromConfigFile();

}