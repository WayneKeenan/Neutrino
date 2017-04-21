#pragma once
#include "File.h"
#include "GL/glew.h"
#include "Memory.h"


namespace Neutrino {
	enum 
	{
		ATTRIB_VERTEX,
		ATTRIB_COLOR,
		ATTRIB_TEXTURE,
		NUM_ATTRIBUTES
	};

	enum eShaderUniforms
	{
		UNIFORM_TRANSLATE,
		UNIFORM_MATRIX,
		UNIFORM_TEXTURE,
		NUM_UNIFORMS
	};

	enum eStandardShaders
	{
	 	DEFAULT_SHADER,
		DEFAULT_UNTEXTURED,
		BLOOM_SHADER,
		BLUR_HORIZ,
		BLUR_VERTICAL,
		NUM_SHADERS
	};

	// TO_DO: Shaders to load should be read from GameConfig.txt
	// TO_DO: imgui implementation should have it's shaders loaded in this batch as well...
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
	bool AttachShaders();

	// UnloadEngineShaders()
	//		Framework kill function to remove the loaded shaders
	void DetachShaders();


	// SetActiveShader
	// 		Set shader pair for current OGL state from list of standard shaders defined in enum above
	void SetActiveShader(eStandardShaders iIndex);

	void SetOutputShader(float* pCameraMatrix);

	// GetActiveUniforms
	// 		Returns the address of the current shader uniforms (see: struct _ShaderSettings_t in cpp file)
	GLint* GetActiveUniforms ();


	// LogShader()
	// 		Simple function to output compilation errors from shader source. IMGUI implementation will use this
	void LogShader(GLuint iID);


	// LogProgram()
	// 		Simple function to output linking errors for a shader programm. IMGUI implementation will use this
	void LogProgram ( GLuint iProg );
}
