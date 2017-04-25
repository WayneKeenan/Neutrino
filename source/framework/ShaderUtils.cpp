#include "ShaderUtils.h"
#include "Log.h"
#include "Assert.h"
#include "File.h"
#include "GLUtils.h"


namespace Neutrino {

	struct ShaderSettings_t
	{
		GLint _Uniforms[NUM_UNIFORMS];
		GLint _ProgramID;
	};


	static ShaderSettings_t* s_pActiveShader = NULL;
	static ScanlineSettings_t* s_pCRTSettings = NULL; 
	static ShaderSettings_t* s_aLoadedShaders = NEWX ShaderSettings_t[NUM_SHADERS];

	static uint8 s_iNumShadersLoaded = 0;


	void LogShader(GLuint iID)
	{
		GLint iLogLength;
		glGetShaderiv(iID, GL_INFO_LOG_LENGTH, &iLogLength);
		ASSERT_GL_ERROR;
		if (iLogLength > 1)
		{
			GLchar *pLog = (GLchar *)malloc(iLogLength);
			glGetShaderInfoLog(iID, iLogLength, &iLogLength, pLog);
			ASSERT_GL_ERROR;
			LOG_INFO("Shader compile log: %s", pLog);
			free(pLog);
		}
	}


	void LogProgram(GLuint iProg)
	{
		GLint iLogLength;
		glGetProgramiv(iProg, GL_INFO_LOG_LENGTH, &iLogLength);
		ASSERT_GL_ERROR;
		if (iLogLength > 1)
		{
			GLchar *pLog = (GLchar *)malloc(iLogLength);
			glGetProgramInfoLog(iProg, iLogLength, &iLogLength, pLog);
			ASSERT_GL_ERROR;
			LOG_INFO("Program compile log: %s", pLog);
			free(pLog);
		}
	}


	static bool LinkShader(GLuint iProg)
	{
		GLint iStatus;
		glLinkProgram(iProg);
		ASSERT_GL_ERROR;

#if defined(DEBUG)
		LogProgram(iProg);
#endif
		glGetProgramiv(iProg, GL_LINK_STATUS, &iStatus);
		ASSERT_GL_ERROR;
		if (iStatus != GL_TRUE)
		{
			glDeleteProgram(iProg);
			return false;
		}

		return true;
	}



	static bool CompileShader(GLuint iShader, const char* pShaderSource, const int iBytes)
	{
		GLint iStatus;

		glShaderSource(iShader, 1, &pShaderSource, &iBytes);
		ASSERT_GL_ERROR;
		glCompileShader(iShader);
		ASSERT_GL_ERROR;

#if defined(DEBUG)
		LogShader(iShader);
#endif

		glGetShaderiv(iShader, GL_COMPILE_STATUS, &iStatus);
		ASSERT_GL_ERROR;
		if (iStatus != GL_TRUE)
		{
			glDeleteShader(iShader);
			ASSERT_GL_ERROR;
			return false;
		}

		return true;
	}



	static bool LoadShader(const char* pFragFilename, const char* pVertFilename)
	{
		ASSERT(s_iNumShadersLoaded < NUM_SHADERS, "Attempting to load more shaders than currently defined");

		GLuint iVertShader = 0;
		GLuint iFragShader = 0;
		int iFragSourceSize = 0;
		int iVertSourceSize = 0;
		const char* FragSource = NULL;
		const char* VertSource = NULL;

		// Load both components of he shader
		{
			iFragSourceSize = GetFileSizeBytes(pFragFilename);
			iVertSourceSize = GetFileSizeBytes(pVertFilename);

			ASSERT(iFragSourceSize > 0, "GetFileSizeBytes returned 0 when attempting to load: %s", pFragFilename);
			ASSERT(iVertSourceSize > 0, "GetFileSizeBytes returned 0 when attempting to load: %s", pVertFilename);


			FragSource = LoadResourceBytes(pFragFilename);
			ASSERT(NULL != FragSource, "LoadResourceBytes for %s returned null", pFragFilename);
			VertSource = LoadResourceBytes(pVertFilename);
			ASSERT(NULL != FragSource, "LoadResourceBytes for %s returned null", pVertFilename);
		}


		// Generate the program and shader objects we need
		{
			s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID = 0;
			s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID = glCreateProgram();

			if (s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID == 0)
			{
				LOG_ERROR("LoadShader: %s -- glCreateProgram failed...");
				return false;
			}

			iFragShader = glCreateShader(GL_FRAGMENT_SHADER);
			ASSERT_GL_ERROR;
			if (iFragShader == 0)
			{
				LOG_ERROR("LoadShader: %s -- glCreateShader failed for GL_FRAGMENT_SHADER...");
				return false;
			}

			iVertShader = glCreateShader(GL_VERTEX_SHADER);
			ASSERT_GL_ERROR;
			if (iVertShader == 0)
			{
				LOG_ERROR("LoadShader: %s -- glCreateShader failed for GL_VERTEX_SHADER...");
				return false;
			}
		}



		// Compile and link the shaders...
		{
			ASSERT(CompileShader(iFragShader, FragSource, iFragSourceSize), "Failed to compile fragment shader! %s", pFragFilename);
			ASSERT(CompileShader(iVertShader, VertSource, iVertSourceSize), "Failed to compile vertex shader! %s", pVertFilename);

			// Attach shaders to program ID
			glAttachShader(s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID, iFragShader);
			ASSERT_GL_ERROR;
			glAttachShader(s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID, iVertShader);
			ASSERT_GL_ERROR;

			// Link program
			if (!LinkShader(s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID))
			{
				glDeleteShader(iVertShader);
				iVertShader = 0;
				glDeleteShader(iFragShader);
				iFragShader = 0;
				glDeleteProgram(s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID);
				s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID = 0;

				DELETEX FragSource;
				DELETEX VertSource;
				return false;
			}
		}


		// Setup the uniforms so we can access them later
		{
			// TO_DO: Need these to be custom for each shader as well...
			glBindAttribLocation(s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID, ATTRIB_VERTEX, "position");
			ASSERT_GL_ERROR;
			glBindAttribLocation(s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID, ATTRIB_COLOR, "color");
			ASSERT_GL_ERROR;
			glBindAttribLocation(s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID, ATTRIB_TEXTURE, "textureCoordinates");
			ASSERT_GL_ERROR;

			// Get uniform locations
			s_aLoadedShaders[s_iNumShadersLoaded]._Uniforms[UNIFORM_TRANSLATE] = glGetUniformLocation(s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID, "translate");
			s_aLoadedShaders[s_iNumShadersLoaded]._Uniforms[UNIFORM_TEXTURE] = glGetUniformLocation(s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID, "texture");
			s_aLoadedShaders[s_iNumShadersLoaded]._Uniforms[UNIFORM_MATRIX] = glGetUniformLocation(s_aLoadedShaders[s_iNumShadersLoaded]._ProgramID, "matrix");
		}


		// Clean up
		{
			glDeleteShader(iVertShader);
			glDeleteShader(iFragShader);
			DELETEX FragSource;
			DELETEX VertSource;
		}


		// We've loaded a shader
		++s_iNumShadersLoaded;
		return true;
	}

	void DetachShaders()
	{
		GLsizei iCount = 0;
		GLuint* aShaders = NEWX GLuint[2];
		for (int i = 0; i < s_iNumShadersLoaded; i++)
		{
			iCount = 0;
			glGetAttachedShaders(s_aLoadedShaders[i]._ProgramID, 2, &iCount, aShaders);
			for (int j = 0; j < iCount; j++)
			{
				glDetachShader(s_aLoadedShaders[i]._ProgramID, aShaders[j]);
			}
		}
		DELETEX[] aShaders;
		LOG_INFO("Shaders detached");
	}


	bool AttachShaders()
	{
		// This is a fixed pool of known shaders. Not going to support arbitrary shader params atm. 
		// TO_DO: Shaders to load should be read from GameConfig.txt
		LoadShader(s_pDefaultShaderFragFilename, s_pDefaultShaderVertFilename);
		LoadShader(s_pDefaultUntexturedFragFilename, s_pDefaultUntexturedVertFilename);
		LoadShader(s_pBlurHorizShaderFragFilename, s_pBlurHorizShaderVertFilename);
		LoadShader(s_pBlurVertShaderFragFilename, s_pBlurVertShaderVertFilename);
		LoadShader(s_pOutputCRTShaderFragFilename, s_pOutputCRTShaderVertFilename);
		LoadShader(s_pThresholdFragFilename, s_pThresholdVertFilename);
		LoadShader(s_pAlphaScaledFragFilename, s_pAlphaScaledVertFilename);

		// Grab the uniforms for the CRT shader, as these are bespoke.
		s_pCRTSettings = NEWX(ScanlineSettings_t);
		s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_SCANLINE] = glGetUniformLocation(s_aLoadedShaders[eStandardShaders::OUTPUT_CRT]._ProgramID, "ScanlineDark");
		s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_VSCANLINE] = glGetUniformLocation(s_aLoadedShaders[eStandardShaders::OUTPUT_CRT]._ProgramID, "VScanlineDark");
		s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_PIXELBIAS] = glGetUniformLocation(s_aLoadedShaders[eStandardShaders::OUTPUT_CRT]._ProgramID, "PixelBias");
		s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_BRIGHTNESS] = glGetUniformLocation(s_aLoadedShaders[eStandardShaders::OUTPUT_CRT]._ProgramID, "Brightness");
		s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_CONTRAST] = glGetUniformLocation(s_aLoadedShaders[eStandardShaders::OUTPUT_CRT]._ProgramID, "Contrast");
		s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_ADDITIVE] = glGetUniformLocation(s_aLoadedShaders[eStandardShaders::OUTPUT_CRT]._ProgramID, "AdditiveStrength");
		s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_THRESHOLD] = glGetUniformLocation(s_aLoadedShaders[eStandardShaders::THRESHOLD]._ProgramID, "Threshold");
		s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_THRESBRIGHTSCALER] = glGetUniformLocation(s_aLoadedShaders[eStandardShaders::THRESHOLD]._ProgramID, "ThresholdBrightScaler");
		s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_BLOOMALPHA] = glGetUniformLocation(s_aLoadedShaders[eStandardShaders::ALPHA_SCALED]._ProgramID, "BloomAlpha");
		s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_BLOOMCONTRAST] = glGetUniformLocation(s_aLoadedShaders[eStandardShaders::ALPHA_SCALED]._ProgramID, "BloomContrast");
		s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_BLOOMBRIGHT] = glGetUniformLocation(s_aLoadedShaders[eStandardShaders::ALPHA_SCALED]._ProgramID, "BloomBrightness");
		// Just in case. 
		SetActiveShader(DEFAULT_UNTEXTURED);
		return true;
	}


	GLint* GetActiveUniforms()
	{
		return s_pActiveShader->_Uniforms;
	};

	ScanlineSettings_t* GetCRTSettings()
	{
		return s_pCRTSettings;
	}

	void SetActiveShader(eStandardShaders iIndex)
	{
		s_pActiveShader = &s_aLoadedShaders[iIndex];
		GL_ERROR;
		glUseProgram(s_pActiveShader->_ProgramID);
		GL_ERROR;
		glUniformMatrix4fv(s_pActiveShader->_Uniforms[UNIFORM_MATRIX], 1, GL_FALSE, GLUtils::GetCameraMatrix());
		GL_ERROR;
	}

	void SetOutputScanlines(float* pCameraMatrix)
	{
		s_pActiveShader = &s_aLoadedShaders[OUTPUT_CRT];
		GL_ERROR;
		glUseProgram(s_pActiveShader->_ProgramID);
		GL_ERROR;
		glUniformMatrix4fv(s_pActiveShader->_Uniforms[UNIFORM_MATRIX], 1, GL_FALSE, pCameraMatrix);
		GL_ERROR;
		glUniform1f(s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_SCANLINE], s_pCRTSettings->_fScanlineDark);
		GL_ERROR;
		glUniform1f(s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_VSCANLINE], s_pCRTSettings->_fVScanlineDark);
		GL_ERROR;
		glUniform1f(s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_PIXELBIAS], s_pCRTSettings->_fPixelBias);
		GL_ERROR;
		glUniform1f(s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_BRIGHTNESS], s_pCRTSettings->_fBrightness);
		GL_ERROR;
		glUniform1f(s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_CONTRAST], s_pCRTSettings->_fContrast);
		GL_ERROR;
		glUniform1f(s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_ADDITIVE], s_pCRTSettings->_fAdditiveStrength);
		GL_ERROR;
	}

	void SetOutputBloom(float* pCameraMatrix)
	{
		s_pActiveShader = &s_aLoadedShaders[ALPHA_SCALED];
		GL_ERROR;
		glUseProgram(s_pActiveShader->_ProgramID);
		GL_ERROR;
		glUniformMatrix4fv(s_pActiveShader->_Uniforms[UNIFORM_MATRIX], 1, GL_FALSE, pCameraMatrix);
		GL_ERROR;
		glUniform1f(s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_BLOOMALPHA], s_pCRTSettings->_fBloomAlpha);
		GL_ERROR;
		glUniform1f(s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_BLOOMCONTRAST], s_pCRTSettings->_fBloomContrast);
		GL_ERROR;
		glUniform1f(s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_BLOOMBRIGHT], s_pCRTSettings->_fBloomBright);
		GL_ERROR;
	}

	void SetOutputThreshold(float* pCameraMatrix)
	{
		s_pActiveShader = &s_aLoadedShaders[THRESHOLD];
		GL_ERROR;
		glUseProgram(s_pActiveShader->_ProgramID);
		GL_ERROR;
		glUniformMatrix4fv(s_pActiveShader->_Uniforms[UNIFORM_MATRIX], 1, GL_FALSE, pCameraMatrix);
		GL_ERROR;
		glUniform1f(s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_THRESHOLD], s_pCRTSettings->_fThreshold);
		GL_ERROR;
		glUniform1f(s_pCRTSettings->_aUniforms[eCRTShaderUniforms::UNIFORM_THRESBRIGHTSCALER], s_pCRTSettings->_fThresholdBrightScaler);
		GL_ERROR;
	}


	void SetActiveShaderWithMatrix(eStandardShaders iIndex, float* pCameraMatrix)
	{
		s_pActiveShader = &s_aLoadedShaders[iIndex];
		GL_ERROR;
		glUseProgram(s_pActiveShader->_ProgramID);
		GL_ERROR;
		glUniformMatrix4fv(s_pActiveShader->_Uniforms[UNIFORM_MATRIX], 1, GL_FALSE, pCameraMatrix);
		GL_ERROR;
	}

}
