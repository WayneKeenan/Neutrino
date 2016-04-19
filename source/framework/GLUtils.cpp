#include "GLUtils.h"
#include "Log.h"
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> 

namespace Neutrino {

        namespace GLUtils {

                static glm::mat4 mCameraMatrix;
                static glm::mat4 mModelViewMatrix;


                void SetViewport(const int iScreenWidth, const int iScreenHeight)
                {
                        LOG_WARNING("GLUtils::SetViewport working with fixed aspect ratio in ortho ProjectionMatrix calc...");
                        glViewport(0, 0, iScreenWidth, iScreenHeight);        
                        glEnable(GL_DEPTH_TEST);
                        glEnable(GL_BLEND);
                        glActiveTexture(GL_TEXTURE0);
                        glClearColor( 0.05, 0.15, 0.25, 1.0 );
                        glClearDepth(1.0f);
                        glDepthFunc(GL_LEQUAL);

                        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                        glm::mat4 mRotationMatrix = glm::rotate(glm::mat4(), 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
                        glm::mat4 mTranslationMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
                        glm::mat4 mProjectionMatrix = glm::ortho(0.0f, (float)((float)iScreenWidth/(float)iScreenHeight), 0.0f, 1.0f, 0.1f, 10.0f );

                        mModelViewMatrix = mTranslationMatrix * mRotationMatrix;
                        mCameraMatrix = mProjectionMatrix * mModelViewMatrix;
                }

                void TestRender()
                {
                        glBegin(GL_TRIANGLES);
                        glVertex3f(0.0, 2.0, 1.0);
                        glVertex3f(-2.0, -2.0, 1.0);
                        glVertex3f(2.0, -2.0, 1.0);
                        glEnd();
                }


                void Term()
                {
                }
        }
}
