#include "GLUtils.h"
#include "Log.h"
#include "Types.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Memory.h"
#include "ShaderUtils.h"
#include "Colour.h"

namespace Neutrino {

    namespace GLUtils {

        static glm::mat4 s_mCameraMatrix;
        static glm::mat4 s_mModelViewMatrix;

        static float s_fOGL_X_RATIO;
        static float s_fOGL_Y_RATIO;

        static float s_fViewportWidth;
        static float s_fViewportHeight;

        static float s_fScaledWidth;
        static float s_fScaledHeight;

        static const int s_iSizeOfSprite = 6*sizeof(Vertex_t);
        static const int s_iSizeOfVertex = sizeof(Vertex_t);

        GLuint iVBO_ID;

        float* GetCameraMatrix()
        {
            return glm::value_ptr(s_mCameraMatrix);
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
                //glActiveTexture(GL_TEXTURE0);
                glClearColor( 0.25f, 0.25f, 0.0f, 1.0f );
                //glClearDepth(1.0f);
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

                s_fScaledWidth = s_fOGL_X_RATIO / (float)iInternalWidth;
                s_fScaledHeight = s_fOGL_Y_RATIO / (float)iInternalHeight;
            }
        }

        void GenerateMVCMatrices()
        {
            // TO_DO:
            // 
            // Play with these to work out how to move the camera about...
            glm::mat4 mRotationMatrix = glm::rotate(glm::mat4(), 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
            glm::mat4 mTranslationMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));

            // This defines 0,0 to be bottom left...
            glm::mat4 mProjectionMatrix = glm::ortho(0.0f, s_fOGL_X_RATIO, 0.0f, s_fOGL_Y_RATIO, 1.0f, -1.0f );

            s_mModelViewMatrix = mTranslationMatrix * mRotationMatrix;
            s_mCameraMatrix = mProjectionMatrix * glm::mat4();
        }


        void CreateVBO()
        {
            glGenBuffers(1, &iVBO_ID); 
            ASSERT_GL_ERROR;
            glBindBuffer(GL_ARRAY_BUFFER, iVBO_ID); 
            ASSERT_GL_ERROR;
            glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * s_iMaxSprites, NULL, GL_DYNAMIC_DRAW  );
            ASSERT_GL_ERROR;
        }


        void DeleteVBO( GLuint iVBO_ID )
        {
            glDeleteBuffers( 1, &iVBO_ID );
            GL_ERROR;
        }


        void TestRender()
        {
            /*
            glBegin(GL_TRIANGLES);
            glVertex3f(0.0f, 1.0f, 1.0f);
            glVertex3f(-1.0f, -1.0f, 1.0f);
            glVertex3f(1.0f, -1.0f, 1.0f);
            glEnd();
            */
        }


        bool LogGlError(const char *pFile, int iLine) 
        {
            bool bReturnVal = false;
            GLenum err (glGetError());

            while(err!=GL_NO_ERROR) 
            {
                LOG_INFO("glError hit: %d %s %d", err, pFile, iLine );
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


        void PopulateVBO(float* pHWidths, float* pHHeights, float* pRots, float* pScales, glm::vec4* pColours, glm::vec3* pPos, const int iCount)
        {
            ASSERT(iCount < s_iMaxSprites, "Sprite count is greater than VBO limits, something's gone very horribly wrong...");

            glm::vec4* vQuadBL_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            glm::vec4* vQuadBR_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            glm::vec4* vQuadTL_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            glm::vec4* vQuadTR_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            glm::vec3* vPos = NEWX glm::vec3();


            // Get the position of the first vertex in the VBO
            glBindBuffer( GL_ARRAY_BUFFER, iVBO_ID );
            ASSERT_GL_ERROR;
            Vertex_t* pVertex = (Vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);


            // Traverse the sprite arrays
            {
                // For each sprite up to iCount
                for(int i=0; i<iCount; i++)
                {
                    /*
                    LOG_INFO("Scale: %f", *pScales);
                    LOG_INFO("Rot: %f ", *pRots);
                    LOG_INFO("Pos: %f,%f,%f", pPos->x, pPos->y, pPos->z);
                    */

                    // Build the transform matrix for this sprite
                    glm::mat4 mScale = glm::scale(glm::vec3(*pScales, *pScales, 1.0f));
                    glm::mat4 mRotation = glm::rotate(*pRots, glm::vec3(0.0f, 0.0f, 1.0f));
                    
                    vPos->x = pPos->x * s_fScaledWidth;
                    vPos->y = pPos->y * s_fScaledHeight;
                    vPos->z = pPos->z;

                    glm::mat4 mTranslate = glm::translate(*vPos);
                    glm::mat4 mTransform = mTranslate * mRotation * mScale;


                    // Build the vertex positions
                    {
                        float fScaledWidth = (*pHWidths * s_fScaledWidth);
                        float fScaledHeight = (*pHHeights * s_fScaledHeight);

                        /*
                        LOG_INFO("Scaled Width: %f", fScaledWidth);
                        LOG_INFO("Scaled Height: %f", fScaledHeight);
                        */


                        vQuadTL_Pos->x = 0.0f - fScaledWidth;
                        vQuadTL_Pos->y = 0.0f + fScaledHeight;
                        vQuadTL_Pos->z = 0.0f;   // TO_DO: need to put z-layer in here
                        vQuadTL_Pos->w = 1.0f;   // TO_DO: need to put z-layer in here

                        vQuadTR_Pos->x = 0.0f + fScaledWidth;
                        vQuadTR_Pos->y = 0.0f + fScaledHeight;
                        vQuadTR_Pos->z = 0.0f;   // TO_DO: need to put z-layer in here
                        vQuadTR_Pos->w = 1.0f;   // TO_DO: need to put z-layer in here
                                                 
                        vQuadBL_Pos->x = 0.0f - fScaledWidth;
                        vQuadBL_Pos->y = 0.0f - fScaledHeight;
                        vQuadBL_Pos->z = 0.0f;   // TO_DO: need to put z-layer in here
                        vQuadBL_Pos->w = 1.0f;   // TO_DO: need to put z-layer in here

                        vQuadBR_Pos->x = 0.0f + fScaledWidth;
                        vQuadBR_Pos->y = 0.0f - fScaledHeight;
                        vQuadBR_Pos->z = 0.0f;   // TO_DO: need to put z-layer in here                    
                        vQuadBR_Pos->w = 1.0f;   // TO_DO: need to put z-layer in here
                    }


                    // Transform the vertex positions
                    glm::vec4 vTransBL = mTransform * *vQuadBL_Pos;
                    glm::vec4 vTransBR = mTransform * *vQuadBR_Pos;
                    glm::vec4 vTransTL = mTransform * *vQuadTL_Pos;
                    glm::vec4 vTransTR = mTransform * *vQuadTR_Pos;

                    /*
                    LOG_INFO("BL: %f, %f, %f", vTransBL.x, vTransBL.y, vTransBL.z );
                    LOG_INFO("BR: %f, %f, %f", vTransBR.x, vTransBR.y, vTransBR.z );
                    LOG_INFO("TL: %f, %f, %f", vTransTL.x, vTransTL.y, vTransTL.z );
                    LOG_INFO("TR: %f, %f, %f", vTransTR.x, vTransTR.y, vTransTR.z );
                    */

                    
                    // Get the packed colour
                    uint32 iColour = GetPackedColourV4(pColours);

//                    LOG_INFO("Colour:" FOUR_BYTE_HEX, iColour);

                    // TO_DO: Texture coords...
                    
                    // Populate the VBO vertex corner
                    pVertex->_colour = iColour;
                    pVertex->_uv[0] = 0;
                    pVertex->_uv[1] = 0;
                    pVertex->_position[0] = vTransBL.x;
                    pVertex->_position[1] = vTransBL.y;
                    pVertex->_position[2] = vTransBL.z;
                    pVertex++;

                    pVertex->_colour = iColour;
                    pVertex->_uv[0] = 0;
                    pVertex->_uv[1] = 0;
                    pVertex->_position[0] = vTransBR.x;
                    pVertex->_position[1] = vTransBR.y;
                    pVertex->_position[2] = vTransBR.z;
                    pVertex++;

                    pVertex->_colour = iColour;
                    pVertex->_uv[0] = 0;
                    pVertex->_uv[1] = 0;
                    pVertex->_position[0] = vTransTL.x;
                    pVertex->_position[1] = vTransTL.y;
                    pVertex->_position[2] = vTransTL.z;
                    pVertex++;

                    pVertex->_colour = iColour;
                    pVertex->_uv[0] = 0;
                    pVertex->_uv[1] = 0;
                    pVertex->_position[0] = vTransBR.x;
                    pVertex->_position[1] = vTransBR.y;
                    pVertex->_position[2] = vTransBR.z;
                    pVertex++;

                    pVertex->_colour = iColour;
                    pVertex->_uv[0] = 0;
                    pVertex->_uv[1] = 0;
                    pVertex->_position[0] = vTransTR.x;
                    pVertex->_position[1] = vTransTR.y;
                    pVertex->_position[2] = vTransTR.z;
                    pVertex++;

                    pVertex->_colour = iColour;
                    pVertex->_uv[0] = 0;
                    pVertex->_uv[1] = 0;
                    pVertex->_position[0] = vTransTL.x;
                    pVertex->_position[1] = vTransTL.y;
                    pVertex->_position[2] = vTransTL.z;
                    pVertex++;

                    // VBO Vertex pointer is now pointing at the next sprite, so increment through the 
                    // sprite settings arrays to get the data for the next iteration
                    pHWidths++;
                    pHHeights++;
                    pRots++;
                    pScales++;
                    pColours++,
                    pPos++;
                }
            }

            glUnmapBuffer(GL_ARRAY_BUFFER);


            DELETEX vQuadBL_Pos;
            DELETEX vQuadBR_Pos;
            DELETEX vQuadTL_Pos;
            DELETEX vQuadTR_Pos;
        }


        void RenderVBO(const int iSpriteCount)
        {
            ASSERT_GL_ERROR;

            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      
            // glBindTexture( GL_TEXTURE_2D, mVBO_Map[i]->_Texture_ID );
            // glUniform1i (uniforms[UNIFORM_TEXTURE], 0);

            glBindBuffer ( GL_ARRAY_BUFFER, iVBO_ID);
            ASSERT_GL_ERROR;
                
            glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, s_iSizeOfVertex, (void*)offsetof(Vertex_t, _position));
            ASSERT_GL_ERROR;

            glEnableVertexAttribArray(ATTRIB_VERTEX);
            ASSERT_GL_ERROR;

            glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, 1, s_iSizeOfVertex, (void*)offsetof(Vertex_t, _colour));
            ASSERT_GL_ERROR;

            glEnableVertexAttribArray(ATTRIB_COLOR);
            ASSERT_GL_ERROR;

            //glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, s_iSizeOfVertex, (void*)offsetof(Vertex_t, _uv));
            //ASSERT_GL_ERROR;

            //glEnableVertexAttribArray(ATTRIB_TEXTURE);
            //ASSERT_GL_ERROR;

            glDrawArrays(GL_TRIANGLES, 0, iSpriteCount * 6);
            ASSERT_GL_ERROR;            
        }
    }
}