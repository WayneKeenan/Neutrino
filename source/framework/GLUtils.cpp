#include "GLUtils.h"
#include "Log.h"

namespace Neutrino {

        namespace GLUtils {
                void SetViewport(const int iScreenWidth, const int iScreenHeight)
                {
                        glViewport(0, 0, iScreenWidth, iScreenHeight);        
                        glEnable(GL_DEPTH_TEST);
                        glEnable(GL_BLEND);
                        glActiveTexture(GL_TEXTURE0);
                        glClearColor( 0.05, 0.15, 0.25, 1.0 );
                        glClearDepth(1.0f);
                        glDepthFunc(GL_LEQUAL);

                        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                        // Vector3D rotationVector;
                        // rotationVector.x = 0.0f;
                        // rotationVector.y = 0.0f;
                        // rotationVector.z = 0.0f;

                        // Matrix3DSetRotationByDegrees(rotationMatrix, 0.0f, rotationVector);
                        // Matrix3DSetTranslation(translationMatrix, 0.f, 0.f, 0.0f);
                        // Matrix3DMultiply(translationMatrix, rotationMatrix, modelViewMatrix);

                        // Matrix3DSetOrthoProjection(projectionMatrix, 0.0f, OGL_X_RANGE, 0.0f, OGL_Y_RANGE, 1.0f, -1.0f);
                        // Matrix3DMultiply(projectionMatrix, modelViewMatrix, matrix);

                        // Reshape( bounds.size.width, bounds.size.height );
                }

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


                void Term()
                {
                }
        }
}
