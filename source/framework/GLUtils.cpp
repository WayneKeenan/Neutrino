#include "GLUtils.h"
#include "Log.h"

namespace Neutrino {

    namespace GLUtils {

        static glm::mat4 s_mCameraMatrix;
        static glm::mat4 s_mModelViewMatrix;

        static float s_fOGL_X_RATIO;
        static float s_fOGL_Y_RATIO;

        static float s_fViewportWidth;
        static float s_fViewportHeight;

        static float s_fInternalWidth;
        static float s_fInternalHeight;

        static const int s_iSizeOfSprite = 6*sizeof(Vertex_t);
        

        float* GetCameraMatrix()
        {
            return &s_mCameraMatrix[0][0];
        }

        // TO_DO:
        // 
        // May need to calculate these matrices multiple times, especially if we're moving the camera about. 
        // Split these out into their own function?
        // 
        // Should iScreenwidth and height be stored locally here?


        void SetViewport(const int iViewportWidth, const int iViewportHeight, const int iInternalWidth, const int iInternalHeight)
        {
            LOG_WARNING("GLUtils::SetViewport working with fixed aspect ratio in ortho ProjectionMatrix calc...");

            // Create an OGL viewport with basic settings
            {
                glViewport(0, 0, iViewportWidth, iViewportHeight);       
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
                glActiveTexture(GL_TEXTURE0);
                glClearColor( 0.05f, 0.15f, 0.25f, 1.0f );
                glClearDepth(1.0f);
                glDepthFunc(GL_LEQUAL);

                glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                GL_ERROR;                
            }

            // Set viewport dimensions, OGL coord range, and internal pixel dimensions
            {
                s_fViewportWidth = (float)iViewportWidth;
                s_fViewportHeight = (float)iViewportHeight;
                s_fOGL_X_RATIO = s_fViewportWidth / s_fViewportHeight;
                s_fOGL_Y_RATIO = 1.0f;
                s_fInternalWidth = s_fOGL_X_RATIO / (float)iInternalWidth;
                s_fInternalHeight = s_fOGL_Y_RATIO / (float)iInternalHeight;
            }
        }

        void GenerateMVCMatrices()
        {
            // TO_DO:
            // 
            // Play with these to work out how to move the camera about...
            glm::mat4 mRotationMatrix = glm::rotate(glm::mat4(), 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
            glm::mat4 mTranslationMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
            glm::mat4 mProjectionMatrix = glm::ortho(0.0f, s_fViewportWidth/s_fViewportHeight, 0.0f, 1.0f, 0.1f, 10.0f );

            s_mModelViewMatrix = mTranslationMatrix * mRotationMatrix;
            s_mCameraMatrix = mProjectionMatrix * s_mModelViewMatrix;
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
            glVertex3f(0.0f, 1.0f, 1.0f);
            glVertex3f(-1.0f, -1.0f, 1.0f);
            glVertex3f(1.0f, -1.0f, 1.0f);
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

        uint16 GetMaxSpriteCount()
        {
            return s_iMaxSprites;
        }


        void RenderSpriteArrays(float* pHWidths, float* pHHeights, float* pRots, float* pScales, glm::vec4* pColours, glm::vec3* pPos, const int iCount)
        {

        }
    }
}