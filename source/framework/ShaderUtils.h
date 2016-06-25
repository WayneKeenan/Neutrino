#pragma once
#include "File.h"
#include "GLUtils.h"
#include <GL/gl.h>
#include "Memory.h"


namespace Neutrino {

	enum eStandardShaders
	{
	  DEFAULT_SHADER,
		DEFAULT_UNTEXTURED,
	  BLOOM_SHADER,
	  BLUR_HORIZ,
	  BLUR_VERTICAL,
	  NUM_SHADERS
	};

	const char* const s_pDefaultShaderFragFilename = "default-shader.fsh";
	const char* const s_pDefaultShaderVertFilename = "default-shader.vsh";
	const char* const s_pDefaultUntexturedFragFilename = "default-untextured-shader.fsh";
	const char* const s_pDefaultUntexturedVertFilename = "default-untextured-shader.vsh";
	const char* const s_pBloomShaderFragFilename = "bloom-shader.fsh";
	const char* const s_pBloomShaderVertFilename = "bloom-shader.vsh";
	const char* const s_pBlurHorizShaderFragFilename = "blur-horiz-shader.fsh";
	const char* const s_pBlurHorizShaderVertFilename = "blur-horiz-shader.vsh";
	const char* const s_pBlurVertShaderFragFilename = "blur-vert-shader.fsh";
	const char* const s_pBlurVertShaderVertFilename = "blur-vert-shader.vsh";


	// LoadEngineShaders()
	//		Framework init function to load the standard shaders (predefined) and validate them. 
	//		Startup will fail if this function doesn't complete. 
	bool LoadEngineShaders();


	// SetActiveShader
	// 		Set shader pair for current OGL state from list of standard shaders defined in enum above
	void SetActiveShader(eStandardShaders iIndex);

	// GetActiveUniforms
	// 		Returns the address of the current shader uniforms (see: struct _ShaderSettings_t in cpp file)
	inline GLint* GetActiveUniforms ();
}
