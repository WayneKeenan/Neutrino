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



	void GLKill()
	{
	}
}
