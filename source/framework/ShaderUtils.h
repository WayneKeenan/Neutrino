#pragma once
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

	enum eCRTShaderUniforms
	{
		UNIFORM_SCANLINE,
		UNIFORM_VSCANLINE,
		UNIFORM_PIXELBIAS,
		UNIFORM_BRIGHTNESS,
		UNIFORM_CONTRAST, 
		UNIFORM_ADDITIVE,
		UNIFORM_THRESHOLD,
		UNIFORM_THRESBRIGHTSCALER,
		UNIFORM_BLOOMALPHA, 
		UNIFORM_BLOOMCONTRAST,
		UNIFORM_BLOOMBRIGHT,
		NUM_CRTUNIFORMS
	};

	enum eStandardShaders
	{
	 	DEFAULT_SHADER,
		DEFAULT_UNTEXTURED,
		BLUR_HORIZ,
		BLUR_VERT,
		OUTPUT_CRT,
		THRESHOLD,
		ALPHA_SCALED,
		NUM_SHADERS
	};

	// The scanline shader used for one of the final render passes has a set of custom 
	// settings that control the RGB split and scanline darkness. These can be adjusted
	// in realtime from the debug overlay, saved and loaded in as a preference. 

	static const float s_fPPS_Version = 0.1f;
	typedef struct PostProcessSettings_t
	{
		float _fScanlineDark = 0.2f;
		float _fVScanlineDark = 0.5f;
		float _fPixelBias = 0.3f;
		float _fBrightness = 2.5f;
		float _fContrast = 1.5f;
		float _fAdditiveStrength = 0.4f;
		float _fThresholdBrightScaler = 1.0f;
		float _fThreshold = 0.2f;
		float _fBloomAlpha = 0.5f;
		float _fBloomContrast = 1.0f;
		float _fBloomBright = 1.0f;
		bool _bDoScanlines = true;
		bool _bDoBloom = true;
		GLuint _aUniforms[(int)eCRTShaderUniforms::NUM_CRTUNIFORMS];
	} PostProcessSettings_t;


	// TO_DO: Shaders to load should be read from GameConfig.txt
	// TO_DO: imgui implementation should have it's shaders loaded in this batch as well...
	const char* const s_pDefaultShaderFragFilename = "default-shader.fsh";
	const char* const s_pDefaultShaderVertFilename = "default-shader.vsh";
	const char* const s_pDefaultUntexturedFragFilename = "default-untextured-shader.fsh";
	const char* const s_pDefaultUntexturedVertFilename = "default-untextured-shader.vsh";
	const char* const s_pBlurHorizShaderFragFilename = "blur-horiz-shader.fsh";
	const char* const s_pBlurHorizShaderVertFilename = "blur-horiz-shader.vsh";
	const char* const s_pBlurVertShaderFragFilename = "blur-vert-shader.fsh";
	const char* const s_pBlurVertShaderVertFilename = "blur-vert-shader.vsh";
	const char* const s_pOutputCRTShaderFragFilename = "simple-crt.fsh";
	const char* const s_pOutputCRTShaderVertFilename = "simple-crt.vsh";
	const char* const s_pThresholdFragFilename = "threshold.fsh";
	const char* const s_pThresholdVertFilename = "threshold.vsh";
	const char* const s_pAlphaScaledFragFilename = "default-alphascaled.fsh";
	const char* const s_pAlphaScaledVertFilename = "default-alphascaled.vsh";

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

	// SetOutputShader
	// 		Final output of the lowres render target needs to have its own static camera matrix and custom
	// 		shader, so this function is used by GLUtils::FinishOffscreenRender to handle setup of the 
	// 		shader params. 
	void SetOutputScanlines(float* pCameraMatrix);

	// SetOutputBloom
	//		Sets the shader its uniform parameters for compositing the bloom buffer to the final output
	void SetOutputBloom(float* pCameraMatrix);

	// SetOutputThreshold
	//		Sets the shader and its uniforms for rendering the low resolution render target to a buffer
	//		that will be used as the initial source for the bloom passes. 
	void SetOutputThreshold(float* pCameraMatrix);

	// SetActiveShaderWithMatrix
	//		Sets the shader pair from the list of standard shaders, but sets the shader's matrix uniform 
	//		to be the parameter passed, rather than the internal camera matrix. Used when rendering the
	//		final composite to the screen. 
	void SetActiveShaderWithMatrix(eStandardShaders iIndex, float* pCameraMatrix);

	// GetActiveUniforms
	// 		Returns the address of the current shader uniforms (see: struct _ShaderSettings_t in cpp file)
	GLint* GetActiveUniforms ();

	// GetCRTSettings
	//		Returns a pointer to the ScanlineSettings_t static that's passed to the CRT shader at runtime. 
	//		DebugOverlay provides a simple UI to adjust this in DEBUG builds.
	PostProcessSettings_t* GetCRTSettings();

	// LogShader()
	// 		Simple function to output compilation errors from shader source. IMGUI implementation will use this
	void LogShader(GLuint iID);


	// LogProgram()
	// 		Simple function to output linking errors for a shader programm. IMGUI implementation will use this
	void LogProgram ( GLuint iProg );
}
