#include "GLUtils.h"
#include "Log.h"

namespace Neutrino {


	// Test render of a quad
	void TestRender()
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, 1280/720, 1.0, 500.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBegin(GL_TRIANGLES);
        glVertex3f(0.0, 2.0, -5.0);
        glVertex3f(-2.0, -2.0, -5.0);
        glVertex3f(2.0, -2.0, -5.0);
        glEnd();
	}

	void GLProgramInfoLog( GLuint iProgramID )
	{
		if( glIsProgram( iProgramID ))
		{
			int iProgramLogLength = 0, iMaxLength = 0;
		
			glGetProgramiv( iProgramID, GL_INFO_LOG_LENGTH, &iMaxLength );
			char* pProgramLog = new char[ iMaxLength ];
			glGetProgramInfoLog( iProgramID, iMaxLength, &iProgramLogLength, pProgramLog );

			if( iProgramLogLength > 0 )
				LOG_INFO("GL Program Log %d: %s", iProgramID, pProgramLog);
			else
				LOG_WARNING("GL Program Log empty for Program ID %d", iProgramID);
		
			delete[] pProgramLog;
		}
		else
		{
			LOG_ERROR("GL Program %d is not a program!", iProgramID);
		}
	}


	void GLShaderInfoLog( GLuint iShaderID )
	{
		if( glIsShader( iShaderID ))
		{
			int iShaderLogLength = 0, iMaxLength = 0;
		
			glGetShaderiv( iShaderID, GL_INFO_LOG_LENGTH, &iMaxLength );
			char* pShaderLog = new char[ iMaxLength ];
			glGetProgramInfoLog( iShaderID, iMaxLength, &iShaderLogLength, pShaderLog );

			if( iShaderLogLength > 0 )
				LOG_INFO("GL Shader Log %d: %s", iShaderID, pShaderLog);
			else
				LOG_WARNING("GL Shader Log empty for Shader ID %d", iShaderID);
		
			delete[] pShaderLog;
		}
		else
		{
			LOG_ERROR("GL Shader %d is not a Shader!", iShaderID);
		}
	}


	void GLKill()
	{
	}

}
