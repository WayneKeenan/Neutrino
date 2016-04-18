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


	struct _LoadedShader_t
	{
		const char*	_FragSource;
		const char*	_VertSource;
		_ShaderSettings_t _ShaderSettings;
	};

	static _ShaderSettings_t* pActiveShader = NULL;
	static _LoadedShader_t* aLoadedShaders = NEWX _LoadedShader_t[MAX_SHADER_COUNT];
	static uint8 iNumShadersLoaded = 0;


	// LogShader()
	//		Output the GL log info for compilation/link stage of shader loading process
	//
	static bool LogShader(bool bIsShader, GLuint iID)
	{
		if (bIsShader && !glIsShader(iID))
		{
			LOG_ERROR("GL Shader ID: %d does not point to valid shader", iID);
			return false;
		}
		else if (!bIsShader && !glIsProgram(iID))
		{
			LOG_ERROR("GL Program ID: %d does not point to a valid program", iID);
			return false;
		}

		int iLength = 0, iMaxLength = 0;
		char* pLog;

		if( bIsShader)
		{
			glGetShaderiv( iID, GL_INFO_LOG_LENGTH, &iMaxLength );
			pLog = new char[ iMaxLength ];
			glGetShaderInfoLog( iID, iMaxLength, &iLength, pLog );
		}
		else
		{
			glGetProgramiv( iID, GL_INFO_LOG_LENGTH, &iMaxLength );
			pLog = new char[ iMaxLength ];
			glGetProgramInfoLog( iID, iMaxLength, &iLength, pLog );
		}

		if( iLength > 0 )
		{
			if(bIsShader)
				LOG_INFO("Compilation Log for shader %d: %s", iID, pLog);
			else
				LOG_INFO("Compilation Log for program %d: %s", iID, pLog);
		}
		else
		{
			if(bIsShader)
				LOG_WARNING("Compilation Log for shader %d: %s", iID, pLog);
			else
				LOG_WARNING("Compilation Log for program %d: %s", iID, pLog);
		}	

		return true;
		delete[] pLog;
	}
	 

	// ValidateShader()
	//		Debug function to validate loaded shader
	//
	static bool ValidateShader ( GLuint iProg )
	{
		GLint iStatus;

		glValidateProgram(iProg);
		LogShader(false, iProg);

		glGetProgramiv(iProg, GL_VALIDATE_STATUS, &iStatus);

		if(iStatus == GL_TRUE)
			return true;
		else
			return false;
	}

	//	LinkShader()
	//		Link and optionally validate the loaded shader. 
	//
	static bool LinkShader ( GLuint iProg )
	{
		GLint iStatus;
		glLinkProgram(iProg);

#if defined(DEBUG)
		LogShader(true, iProg);
		ASSERT( ValidateShader( iProg ), "Shader validation failed." );
#endif
		glGetProgramiv(iProg, GL_LINK_STATUS, &iStatus);
		
		if(iStatus == GL_TRUE)
			return true;
		else
			return false;
	}



	// CompileShader()
	//		Take C-String shader source and compile it. With optional debug validation and logging
	//
	static bool CompileShader ( GLuint* pShader, GLenum iType, const char* pShaderSource, const int iBytes )
	{
 		GLint iStatus;
		*pShader = glCreateShader(iType);

		glShaderSource(*pShader, 1, &pShaderSource, &iBytes);
		glCompileShader(*pShader);

#if defined(DEBUG)
		LogShader(true, *pShader);
#endif

		glGetShaderiv(*pShader, GL_COMPILE_STATUS, &iStatus);
		if (iStatus == 0)
		{
			glDeleteShader(*pShader);
			return false;
		}

		return true;
	}


	// LoadShaders()
	// 		Load a fixed set of shaders from the Resource folder
	// 		Currently shaders are predefined and loaded in a fixed order...
	//		TO_DO: all resources need to be put in, and loaded from, a PHYSFS pack file
	static bool LoadShader( const char* pVertFilename, const char* pFragFilename )
	{
		GLuint iVertShader, iFragShader;
		int iFragSourceSize, iVertSourceSize = 0;

		// Load both components of he shader
		{
			iFragSourceSize = GetFileSizeBytes( pFragFilename );
			iVertSourceSize = GetFileSizeBytes( pVertFilename );

			ASSERT(iFragSourceSize > 0, "GetFileSizeBytes returned 0 when attempting to load: %s", pFragFilename );
			ASSERT(iVertSourceSize > 0, "GetFileSizeBytes returned 0 when attempting to load: %s", pVertFilename );

			aLoadedShaders[ iNumShadersLoaded ]._FragSource = LoadResource( pFragFilename );
			aLoadedShaders[ iNumShadersLoaded ]._VertSource = LoadResource( pVertFilename );
			aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._ProgramID	= glCreateProgram();
		}


		// Compile the shaders...
		{
			ASSERT( CompileShader (&iFragShader, GL_FRAGMENT_SHADER, aLoadedShaders[ iNumShadersLoaded ]._FragSource, iFragSourceSize ), "Failed to compile fragment shader!");
			ASSERT( CompileShader (&iVertShader, GL_VERTEX_SHADER, aLoadedShaders[ iNumShadersLoaded ]._VertSource, iVertSourceSize ), "Failed to compile vertex shader!");  

			// Attach shaders to program ID
			glAttachShader(aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._ProgramID, iFragShader);
			glAttachShader(aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._ProgramID, iVertShader);  

			// TO_DO: Need these to be custom for each shader as well...
			glBindAttribLocation(aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._ProgramID, ATTRIB_VERTEX, "position");
			glBindAttribLocation(aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._ProgramID, ATTRIB_COLOR, "color");
			glBindAttribLocation(aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._ProgramID, ATTRIB_TEXTURE, "textureCoordinates");
		}


		// Link program
		if ( !LinkShader( aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._ProgramID ) )
		{    
				glDeleteShader(iVertShader);
				iVertShader = 0;
				glDeleteShader(iFragShader);
		  		iFragShader = 0;
			 	glDeleteProgram(aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._ProgramID);
		  		aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._ProgramID = 0;
		  		return false;
		}

		// Get uniform locations
		aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._Uniforms[UNIFORM_TRANSLATE] = glGetUniformLocation( aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._ProgramID, "translate");
		aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._Uniforms[UNIFORM_TEXTURE] = glGetUniformLocation( aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._ProgramID, "texture" );
		aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._Uniforms[UNIFORM_MATRIX] = glGetUniformLocation( aLoadedShaders[ iNumShadersLoaded ]._ShaderSettings._ProgramID, "matrix" );

		// Release vertex and fragment shaders
		glDeleteShader(iVertShader);
		glDeleteShader(iFragShader);

		// We've loaded a shader
		++iNumShadersLoaded;
		return true;
	}


	bool LoadEngineShaders()
	{
		return true;
	}

	inline GLint* GetActiveUniforms ()
	{ 
		return pActiveShader->_Uniforms; 
	};

	void SetActiveShader(const uint8 iIndex)
	{
		/*
		pActiveShader = &aLoadedShaders[iIndex]._ShaderSettings;

		glUseProgram( pActiveShader->_ProgramID );
		glUniformMatrix4fv( pActiveShader->_Uniforms[UNIFORM_MATRIX], 1, GL_FALSE, matrix);
		*/
	}
}