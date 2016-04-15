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

	void GLLogCompilationInfo(bool bIsShader, GLuint iID)
	{
		if (bIsShader && !glIsShader(iID))
		{
			LOG_ERROR("GL Shader ID: %d does not point to valid shader", iID);
			return;
		}
		else if (!bIsShader && !glIsProgram(iID))
		{
			LOG_ERROR("GL Program ID: %d does not point to a valid program", iID);
			return;
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
	
		delete[] pLog;
	}

	void GLKill()
	{
	}

}
