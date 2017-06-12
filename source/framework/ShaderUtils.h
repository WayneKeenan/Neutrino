#pragma once
#include "GL/glew.h"
#include "Memory.h"
#include <glm/glm.hpp> 


namespace Neutrino {
	enum 
	{
		ATTRIB_VERTEX,
		ATTRIB_COLOR,
		ATTRIB_TEXTURE,
		NUM_ATTRIBUTES
	};

	enum eGameShaderUniforms
	{
		UNIFORM_TRANSLATE,
		UNIFORM_MATRIX,
		UNIFORM_TEXTURE,
		NUM_UNIFORMS
	};

	enum eCompositeShaderUniforms
	{
		UNIFORM_SCANLINE,
		UNIFORM_VSCANLINE,
		UNIFORM_PIXELBIAS,
		UNIFORM_BRIGHTNESS,
		UNIFORM_CONTRAST,
		UNIFORM_ADDITIVE,
		UNIFORM_TEXTUREWIDTH,
		UNIFORM_TEXTUREHEIGHT,
		UNIFORM_VIEWPORTWIDTH,
		UNIFORM_VIEWPORTHEIGHT,
		UNIFORM_THRESHOLD,
		UNIFORM_THRESBRIGHTSCALER,
		UNIFORM_BLOOMALPHA, 
		UNIFORM_BLOOMCONTRAST,
		UNIFORM_BLOOMBRIGHT,
		NUM_COMPOSITE_UNIFORMS
	};

	enum eShader
	{
		DEFAULT_SHADER,
		DEFAULT_UNTEXTURED,
		BLUR_HORIZ,
		BLUR_VERT,
		OUTPUT_CRT,
		THRESHOLD,
		ALPHA_SCALED,
#if defined DEBUG	
		// Physics world debug shaders aren't loaded in release builds. 
		DEBUG_BOX2D_POINT,
		DEBUG_BOX2D_LINE,
		DEBUG_BOX2D_TRIANGLE,
#endif
		NUM_SHADERS
	};

	// Shader defines for the Debug rendering of the physics world



	// The shaders used for one of the final composition have a set of custom 
	// settings that control various aspects of the rendering. These can be adjusted
	// in realtime from the debug overlay, saved and loaded in as a preference. 
	static const float s_fPPS_Version = 0.1f;
	typedef struct PostProcessSettings_t
	{
		float _fScanlineDark = 0.15f;
		float _fVScanlineDark = 0.5f;
		float _fPixelBias = 0.85f;
		float _fBrightness = 1.15f;
		float _fContrast = 2.35f;
		float _fAdditiveStrength = 0.25f;
		float _fThresholdBrightScaler = 0.85f;
		float _fThreshold = 0.65f;
		float _fBloomAlpha = 0.78f;
		float _fBloomContrast = 1.0f;
		float _fBloomBright = 0.75f;
		bool _bDoScanlines = true;
		bool _bDoBloom = true;
		GLuint _aUniforms[(int)eCompositeShaderUniforms::NUM_COMPOSITE_UNIFORMS];
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
#if defined DEBUG
	const char* const s_pDebugBox2DLineFragFilename = "Box2D_LineDebug.fsh";
	const char* const s_pDebugBox2DLineVertFilename = "Box2D_LineDebug.vsh";
	const char* const s_pDebugBox2DTriangleFragFilename = "Box2D_TriangleDebug.fsh";
	const char* const s_pDebugBox2DTriangleVertFilename = "Box2D_TriangleDebug.vsh";
	const char* const s_pDebugBox2DPointFragFilename = "Box2D_PointDebug.fsh";
	const char* const s_pDebugBox2DPointVertFilename = "Box2D_PointDebug.vsh";
	const int s_iNUM_BOX2D_DEBUG_SHADERS = 3;
#endif

	// LoadEngineShaders()
	//		Framework init function to load the standard shaders (predefined) and validate them. 
	//		Startup will fail if this function doesn't complete. 
	bool AttachShaders();

	// UnloadEngineShaders()
	//		Framework kill function to remove the loaded shaders
	void DetachShaders();


	// SetActiveShader
	// 		Set shader pair for current OGL state from list of standard shaders defined in enum above
	void SetActiveShader(eShader iIndex);

	// SetOutputShader
	// 		Final output of the lowres render target needs to have its own static camera matrix and custom
	// 		shader, so this function is used by GLUtils::FinishOffscreenRender to handle setup of the 
	// 		shader params. 
	void SetOutputScanlines(float* pCameraMatrix, const glm::vec2& vViewportDims, const glm::vec2& vInternalDims);

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
	void SetActiveShaderWithMatrix(eShader iIndex, float* pCameraMatrix);

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

#if defined DEBUG
	enum
	{
		ATTRIB_BOX2D_DEBUG_POSITION,
		ATTRIB_BOX2D_DEBUG_COLOR,
		ATTRIB_BOX2D_DEBUG_SIZE,
		NUM_BOX2D_DEBUG_ATTRIBUTES,
	};
	
	enum Box2DShader
	{
		BOX2D_POINT,
		BOX2D_LINE,
		BOX2D_TRIANGLE,
	};

	void SetBox2DShader(Box2DShader iIndex);
#endif
}
