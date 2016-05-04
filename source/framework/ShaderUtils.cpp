#include "ShaderUtils.h"
#include "Log.h"
#include "Assert.h"
#include "File.h"


namespace Neutrino {

	enum 
	{
		ATTRIB_VERTEX,
		ATTRIB_COLOR,
		ATTRIB_TEXTURE,
		NUM_ATTRIBUTES
	};

	enum _ShaderUniforms
	{
	  UNIFORM_TRANSLATE,
	  UNIFORM_MATRIX,
	  UNIFORM_TEXTURE,
	  NUM_UNIFORMS
	};

	struct _ShaderSettings_t
	{
	  GLint _Uniforms   [NUM_UNIFORMS];
	  GLint _ProgramID; 
	};


	static _ShaderSettings_t* pActiveShader = NULL;
	static _ShaderSettings_t* aLoadedShaders = NEWX _ShaderSettings_t[NUM_SHADERS];
	static uint8 iNumShadersLoaded = 0;


	// LogShader()
	//		Output the GL log info for shader compile
	//
	static void LogShader(GLuint iID)
	{
		GLint iLogLength;
  		glGetShaderiv(iID, GL_INFO_LOG_LENGTH, &iLogLength);
  		if (iLogLength > 1)
  		{
    		GLchar *pLog = (GLchar *)malloc(iLogLength);
    		glGetShaderInfoLog(iID, iLogLength, &iLogLength, pLog);
    		LOG_INFO("Shader compile log: %s", pLog);
    		free(pLog);
  		}	
	}
	 

	// LogProgram()
	//		Output the GL log for the program compile
	//
	static void LogProgram ( GLuint iProg )
	{
		GLint iLogLength;
		glGetProgramiv(iProg, GL_INFO_LOG_LENGTH, &iLogLength);
		if (iLogLength > 1)
		{
			GLchar *pLog = (GLchar *)malloc(iLogLength);
			glGetProgramInfoLog(iProg, iLogLength, &iLogLength, pLog);
    		LOG_INFO("Program compile log: %s", pLog);
			free(pLog);
		}
	}

	//	LinkShader()
	//		Link and optionally validate. 
	//
	static bool LinkShader ( GLuint iProg )
	{
		GLint iStatus;
		glLinkProgram(iProg);

#if defined(DEBUG)
		LogProgram( iProg );
#endif
		glGetProgramiv(iProg, GL_LINK_STATUS, &iStatus);
		
		if(iStatus != GL_TRUE)
		{
			glDeleteProgram(iProg);
			return false;
		}

		return true;
	}



	// CompileShader()
	//		Take C-String shader source and compile it. With optional debug validation and logging
	//
	static bool CompileShader ( GLuint iShader, GLenum iType, const char* pShaderSource, const int iBytes )
	{
 		GLint iStatus;
		iShader = glCreateShader(iType);

		glShaderSource(iShader, 1, &pShaderSource, &iBytes);
		glCompileShader(iShader);

#if defined(DEBUG)
		LogShader(iShader);
#endif

		glGetShaderiv(iShader, GL_COMPILE_STATUS, &iStatus);
		if (iStatus != GL_TRUE)
		{
			glDeleteShader(iShader);
			return false;
		}

		return true;
	}


	// LoadShaders()
	// 		Load a fixed set of shaders from the Resource folder
	// 		Currently shaders are predefined and loaded in a fixed order...
	//		TO_DO: all resources need to be put in, and loaded from, a PHYSFS pack file
	static bool LoadShader( const char* pFragFilename, const char* pVertFilename )
	{
		ASSERT(iNumShadersLoaded < NUM_SHADERS, "Attempting to load more shaders than currently defined");

		GLuint iVertShader = 0, iFragShader = 0;
		int iFragSourceSize = 0, iVertSourceSize = 0;
		const char* FragSource = NULL;
		const char* VertSource = NULL;

		// Load both components of he shader
		{
			iFragSourceSize = GetFileSizeBytes( pFragFilename );
			iVertSourceSize = GetFileSizeBytes( pVertFilename );

			ASSERT(iFragSourceSize > 0, "GetFileSizeBytes returned 0 when attempting to load: %s", pFragFilename );
			ASSERT(iVertSourceSize > 0, "GetFileSizeBytes returned 0 when attempting to load: %s", pVertFilename );


			FragSource = LoadResourceBytes( pFragFilename );
			ASSERT(NULL != FragSource, "LoadResourceBytes for %s returned null", pFragFilename);
			VertSource = LoadResourceBytes( pVertFilename );
			ASSERT(NULL != FragSource, "LoadResourceBytes for %s returned null", pVertFilename);
		}


		aLoadedShaders[ iNumShadersLoaded ]._ProgramID	= glCreateProgram();

		// Compile the shaders...
		{
			ASSERT( CompileShader (iFragShader, GL_FRAGMENT_SHADER, FragSource, iFragSourceSize ), "Failed to compile fragment shader!");
			ASSERT( CompileShader (iVertShader, GL_VERTEX_SHADER, VertSource, iVertSourceSize ), "Failed to compile vertex shader!");  

			// Attach shaders to program ID
			glAttachShader(aLoadedShaders[ iNumShadersLoaded ]._ProgramID, iFragShader);
			glAttachShader(aLoadedShaders[ iNumShadersLoaded ]._ProgramID, iVertShader);  

			// TO_DO: Need these to be custom for each shader as well...
			glBindAttribLocation(aLoadedShaders[ iNumShadersLoaded ]._ProgramID, ATTRIB_VERTEX, "position");
			glBindAttribLocation(aLoadedShaders[ iNumShadersLoaded ]._ProgramID, ATTRIB_COLOR, "color");
			glBindAttribLocation(aLoadedShaders[ iNumShadersLoaded ]._ProgramID, ATTRIB_TEXTURE, "textureCoordinates");
		}


		// Link program
		if ( !LinkShader( aLoadedShaders[ iNumShadersLoaded ]._ProgramID ) )
		{    
				glDeleteShader(iVertShader);
				iVertShader = 0;
				glDeleteShader(iFragShader);
		  		iFragShader = 0;
			 	glDeleteProgram(aLoadedShaders[ iNumShadersLoaded ]._ProgramID);
		  		aLoadedShaders[ iNumShadersLoaded ]._ProgramID = 0;

		  		DELETEX FragSource;
		  		DELETEX VertSource;
		  		return false;
		}

		// Get uniform locations
		aLoadedShaders[ iNumShadersLoaded ]._Uniforms[UNIFORM_TRANSLATE] = glGetUniformLocation( aLoadedShaders[ iNumShadersLoaded ]._ProgramID, "translate");
		aLoadedShaders[ iNumShadersLoaded ]._Uniforms[UNIFORM_TEXTURE] = glGetUniformLocation( aLoadedShaders[ iNumShadersLoaded ]._ProgramID, "texture" );
		aLoadedShaders[ iNumShadersLoaded ]._Uniforms[UNIFORM_MATRIX] = glGetUniformLocation( aLoadedShaders[ iNumShadersLoaded ]._ProgramID, "matrix" );

		// Release vertex and fragment shaders
		glDeleteShader(iVertShader);
		glDeleteShader(iFragShader);
  		DELETEX FragSource;
  		DELETEX VertSource;

		// We've loaded a shader
		++iNumShadersLoaded;
		return true;
	}


	bool LoadEngineShaders()
	{
		LoadShader(s_pDefaultShaderFragFilename, s_pDefaultShaderVertFilename);
		LoadShader(s_pBloomShaderFragFilename, s_pBloomShaderVertFilename);
		LoadShader(s_pBlurHorizShaderFragFilename, s_pBlurHorizShaderVertFilename);
		LoadShader(s_pBlurVertShaderFragFilename, s_pBlurVertShaderVertFilename);

		SetActiveShader(DEFAULT_SHADER);

		return true;
	}

	inline GLint* GetActiveUniforms ()
	{ 
		return pActiveShader->_Uniforms; 
	};

	void SetActiveShader(eStandardShaders iIndex)
	{
		pActiveShader = &aLoadedShaders[iIndex];

		glUseProgram( pActiveShader->_ProgramID );
		glUniformMatrix4fv( pActiveShader->_Uniforms[UNIFORM_MATRIX], 1, GL_FALSE, GLUtils::GetCameraMatrix());
	}
}