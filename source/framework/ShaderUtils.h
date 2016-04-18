#pragma once
#include "File.h"
#include "GLUtils.h"
#include <GL/gl.h>
#include "Memory.h"

#define MAX_SHADER_COUNT 5

namespace Neutrino {



	// LoadEngineShaders()
	//		Framework init function to load the standard shaders (predefined) and validate them. 
	//		Startup will fail if this function doesn't complete. 
	bool LoadEngineShaders();

	void SetActiveShader(const uint8 iIndex);
	inline GLint* GetActiveUniforms ();
}