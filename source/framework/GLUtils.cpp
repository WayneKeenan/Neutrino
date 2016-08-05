#include "GLUtils.h"
#include "Log.h"

namespace Neutrino {

    namespace GLUtils {

        static glm::mat4 mCameraMatrix;
        static glm::mat4 mModelViewMatrix;

        float* GetCameraMatrix()
        {
            return &mCameraMatrix[0][0];
        }



        void SetViewport(const int iScreenWidth, const int iScreenHeight)
        {
            LOG_WARNING("GLUtils::SetViewport working with fixed aspect ratio in ortho ProjectionMatrix calc...");
            glViewport(0, 0, iScreenWidth, iScreenHeight);       
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glActiveTexture(GL_TEXTURE0);
            glClearColor( 0.05f, 0.15f, 0.25f, 1.0f );
            glClearDepth(1.0f);
            glDepthFunc(GL_LEQUAL);

            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            GL_ERROR;

            glm::mat4 mRotationMatrix = glm::rotate(glm::mat4(), 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
            glm::mat4 mTranslationMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
            glm::mat4 mProjectionMatrix = glm::ortho(0.0f, (float)((float)iScreenWidth/(float)iScreenHeight), 0.0f, 1.0f, 0.1f, 10.0f );

            mModelViewMatrix = mTranslationMatrix * mRotationMatrix;
            mCameraMatrix = mProjectionMatrix * mModelViewMatrix;
        }


        void CreateVBO()
        {
            GLuint iVBO_ID;
            glGenBuffers(1, &iVBO_ID); 
            ASSERT_GL_ERROR;
            glBindBuffer(GL_ARRAY_BUFFER, iVBO_ID); 
            ASSERT_GL_ERROR;
            glBufferData(GL_ARRAY_BUFFER, (sizeof(Vertex_t) * 6) * s_iMaxSprites, NULL, GL_DYNAMIC_DRAW  );
            ASSERT_GL_ERROR;
        }


        void DeleteVBO( GLuint iVBO_ID )
        {
            glDeleteBuffers( 1, &iVBO_ID );
            GL_ERROR;
        }


        void TestRender()
        {
            glBegin(GL_TRIANGLES);
            glVertex3f(0.0f, 2.0f, 1.0f);
            glVertex3f(-2.0f, -2.0f, 1.0f);
            glVertex3f(2.0f, -2.0f, 1.0f);
            glEnd();
        }


        bool LogGlError(const char *pFile, int iLine) 
        {
            bool bReturnVal = false;
            GLenum err (glGetError());

            while(err!=GL_NO_ERROR) 
            {
                switch(err) 
                {
                    case GL_INVALID_OPERATION:      
                        LOG_ERROR("%s, %d: INVALID_OPERATION", pFile, iLine);   
                        bReturnVal = true;   
                        break;

                    case GL_INVALID_ENUM:           
                        LOG_ERROR("%s, %d: INVALID_ENUM", pFile, iLine);           
                        bReturnVal = true;   
                        break;

                    case GL_INVALID_VALUE:          
                        LOG_ERROR("%s, %d: INVALID_VALUE", pFile, iLine);          
                        bReturnVal = true;   
                        break;

                    case GL_OUT_OF_MEMORY:          
                        LOG_ERROR("%s, %d: OUT_OF_MEMORY", pFile, iLine);
                        bReturnVal = true;   
                        break;

                    case GL_INVALID_FRAMEBUFFER_OPERATION:  
                        LOG_ERROR("%s, %d: INVALID_FRAMEBUFFER_OPERATION", pFile, iLine);  
                        bReturnVal = true;   
                        break;

                }
                err=glGetError();
            }
            return bReturnVal;
        }
    }
}
