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
		static glm::mat4 s_mProjectionMatrix;
		static glm::vec4 s_vClearColour = glm::vec4(0.25f, 0.25f, 0.0f, 1.0f);

		static float s_fOGL_X_RANGE;
		static float s_fOGL_Y_RANGE;

		static float s_fViewportWidth;
		static float s_fViewportHeight;

		// Pixel scaler for low res render output 
		static float s_fScaledPixelWidth;
		static float s_fScaledPixelHeight;

		// Pixel scaler for editor modes
		static float s_fUnscaledPixelWidth;
		static float s_fUnscaledPixelHeight;

		static const int s_iSizeOfSprite = 6*sizeof(Vertex_t);
		static const int s_iSizeOfVertex = sizeof(Vertex_t);

		// Static allocated arrays for the buffered VBOs
		static DynamicVBO_t* s_pVBOArrays[iMAX_TEXTURES];
		static DynamicVBO_t* s_pDebugVBOs = NULL;						// Debug Builds only. 
		static GLuint s_pTilemapVBOs[iMAX_TILEMAPS];

		// Counters
		static uint8 s_iAllocatedDynamicVBOSets = 0;
		static uint8 s_iAllocatedTilemapVBOs = 0;

		// Transform matrices
		static GLfloat s_mTransformationMatrix[9] = { 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f };
		static GLfloat s_mScaleMatrix[9] = { 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f };
		static GLfloat s_mRotationMatrix[9] = { 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f };


		float* GetCameraMatrix()
		{
			return glm::value_ptr(s_mCameraMatrix);
		}


		void SetClearColour(const float fR, const float fG, const float fB, const float fA)
		{
			s_vClearColour.x = fR; s_vClearColour.y = fG; s_vClearColour.z = fB; s_vClearColour.w = fA;
			//LOG_INFO("Clear Colour Set to: %.2f, %.2f, %.2f, %.2f",  s_vClearColour.x, s_vClearColour.y, s_vClearColour.z, s_vClearColour.w );
		}

		void ClearBuffers()
		{
			glClearColor(s_vClearColour.x, s_vClearColour.y, s_vClearColour.z, s_vClearColour.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void SetViewport(const int iViewportWidth, const int iViewportHeight, const int iInternalWidth, const int iInternalHeight)
		{
			// Create an OGL viewport with basic settings
			{
				glViewport(0, 0, iViewportWidth, iViewportHeight);       
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_BLEND);
				glActiveTexture(GL_TEXTURE0);
				glClearColor( s_vClearColour.x, s_vClearColour.y, s_vClearColour.z, s_vClearColour.w );
				glClearDepth(1.0f);
				glDepthFunc(GL_LEQUAL);
				glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				ASSERT_GL_ERROR;                
			}

			// Set viewport dimensions, OGL coord range, and internal pixel dimensions
			{
				s_fViewportWidth = (float)iViewportWidth;
				s_fViewportHeight = (float)iViewportHeight;

				s_fOGL_X_RANGE = s_fViewportWidth / s_fViewportHeight;
				s_fOGL_Y_RANGE = 1.0f;

				s_fScaledPixelWidth = s_fOGL_X_RANGE / (float)iInternalWidth;
				s_fScaledPixelHeight = s_fOGL_Y_RANGE / (float)iInternalHeight;

				s_fUnscaledPixelWidth = s_fOGL_X_RANGE /(float)iViewportWidth;
				s_fUnscaledPixelHeight = s_fOGL_Y_RANGE / (float)iViewportHeight;
			}

			// Set the projection matrix for this viewport. 0,0 at TOP LEFT.
			s_mProjectionMatrix = glm::ortho(0.0f, s_fOGL_X_RANGE, s_fOGL_Y_RANGE,  0.0f,  1.0f, -10.0f );

		}

		const glm::vec2 GetViewportDimensions()
		{
			return glm::vec2(s_fViewportWidth, s_fViewportHeight);
		}

		const glm::vec2 GetViewportPixelScale()
		{
			return glm::vec2(s_fUnscaledPixelWidth, s_fUnscaledPixelHeight);
		}

		const glm::vec2 GetInternalPixelScale()
		{
			return glm::vec2(s_fScaledPixelWidth, s_fScaledPixelHeight);
		}

		void GenerateMVCMatrices(glm::vec3* vPos)
		{
			// We're not going to rotate the camera: 
			// glm::mat4 mRotationMatrix = glm::mat4();
			// 
			// We are going to move the camera:
			glm::mat4 mTranslationMatrix = glm::translate( glm::mat4(1.0f), *vPos);

			s_mModelViewMatrix = mTranslationMatrix;    // * mRotationMatrix;  // Add in scale? Are we going to zoom in and out?
			s_mCameraMatrix = s_mProjectionMatrix * s_mModelViewMatrix;
		}

		void AllocateDynamicVBOSet()
		{
			ASSERT(s_iAllocatedDynamicVBOSets < iMAX_TEXTURES, "Call to AllocateDynamicVBOSet made when max textures has been reached.");

			s_pVBOArrays[s_iAllocatedDynamicVBOSets] = NEWX(DynamicVBO_t);
			s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_iVBOCounter = 0;

			glGenBuffers(1, &s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_aVBOs[0]); 
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_aVBOs[0]); 
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * iMAX_SPRITES, NULL, GL_STREAM_DRAW  );
			ASSERT_GL_ERROR;

			glGenBuffers(1, &s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_aVBOs[1]); 
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_aVBOs[1]); 
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * iMAX_SPRITES, NULL, GL_STREAM_DRAW);
			ASSERT_GL_ERROR;

			glGenBuffers(1, &s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_aVBOs[2]); 
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_aVBOs[2]); 
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * iMAX_SPRITES, NULL, GL_STREAM_DRAW);
			ASSERT_GL_ERROR;

			++s_iAllocatedDynamicVBOSets;
		}


		void DeallocateDynamicVBOs()
		{
			for ( int i=0; i<s_iAllocatedDynamicVBOSets; i++)
			{
				glDeleteBuffers( 1, &s_pVBOArrays[i]->_aVBOs[0] );
				GL_ERROR;                
				glDeleteBuffers( 1, &s_pVBOArrays[i]->_aVBOs[1] );
				GL_ERROR;                
				glDeleteBuffers( 1, &s_pVBOArrays[i]->_aVBOs[2] );
				GL_ERROR;                

				DELETEX s_pVBOArrays[i];
			}

			LOG_INFO("Dynamic VBOs Deallocated.");
		}


		uint8 CreateTilemapVBO(const uint32 iTilemapSize)
		{
			ASSERT(s_iAllocatedTilemapVBOs < iMAX_TILEMAPS, "Call to CreateTilemapVBO made when max tilemaps has been reached.");
			glGenBuffers(1, &s_pTilemapVBOs[s_iAllocatedTilemapVBOs]);
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_pTilemapVBOs[s_iAllocatedTilemapVBOs]);
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * iTilemapSize, NULL, GL_STATIC_DRAW);
			ASSERT_GL_ERROR;
			++s_iAllocatedTilemapVBOs;
			return s_iAllocatedTilemapVBOs-1;
		}


		void DeallocateTilemapVBOs()
		{
			for (int i = 0; i < s_iAllocatedDynamicVBOSets; i++)
			{
				glDeleteBuffers(1, &s_pTilemapVBOs[i]);
				GL_ERROR;
			}

			LOG_INFO("Tilemap VBOs Deallocated.");
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



		void PopulateVBO( const float* pX_S,
											const float* pY_T,
											const float* pX_SnS,
											const float* pY_TnT,
											const float* pHWidths, 
											const float* pHHeights, 
											const float* pRots, 
											const float* pScales, 
											glm::vec4* pColours, 
											glm::vec3* pPos, 
											const uint32 iCount, 
											const int iVBOSet   )
		{
			ASSERT(iCount < iMAX_SPRITES, "Sprite count is greater than VBO limits, something's gone very horribly wrong...");

			glm::vec4* vQuadBL_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec4* vQuadBR_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec4* vQuadTL_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec4* vQuadTR_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec4 vTransBL = glm::vec4();
			glm::vec4 vTransBR = glm::vec4();
			glm::vec4 vTransTL = glm::vec4();
			glm::vec4 vTransTR = glm::vec4();
			glm::vec3* vPos = NEWX glm::vec3();


			GLuint iVBO_ID = s_pVBOArrays[iVBOSet]->_aVBOs[s_pVBOArrays[iVBOSet]->_iVBOCounter];
			glBindBuffer( GL_ARRAY_BUFFER, iVBO_ID );
			ASSERT_GL_ERROR;
			Vertex_t* pVertex = (Vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

			// Traverse the sprite arrays
			// TODO: Split this into 4 and spread across threads
			{
#ifdef GLM_MATRIX
				glm::mat4 mScale = glm::mat4(1.0f);
				glm::mat4 mRotation = glm::mat4(1.0f);
				glm::mat4 mTranslate = glm::mat4(1.0f);
				glm::mat4 mTransform = glm::mat4(1.0f);
#endif
				float fScaledWidth;
				float fScaledHeight;
				uint32 iColour;

				// For each sprite up to iCount
				for (uint32 i = 0; i < iCount; i++)
				{
					// TODO: Ignore sprites outside the frustrum

					// Build the transform matrix for this sprite
#ifdef GLM_MATRIX
					mScale[0].x = *pScales;
					mScale[1].y = *pScales;

					mRotation[0].x = (float)cos(*pRots);
					mRotation[0].y = (float)-sin(*pRots);
					mRotation[1].x = (float)sin(*pRots);
					mRotation[1].y = (float)cos(*pRots);
					
					mTranslate[3] = glm::vec4(vPos->x, vPos->y, vPos->z, 1.0f);
					// This is the slowest line in here. 
					// TODO: Move this into the Vertex Shader
					mTransform = mTranslate * mRotation * mScale;
#else
					s_mRotationMatrix[0] = (float)cos(*pRots);
					s_mRotationMatrix[1] = (float)sin(*pRots);
					s_mRotationMatrix[3] = (float)-sin(*pRots);
					s_mRotationMatrix[4] = (float)cos(*pRots);
					s_mRotationMatrix[8] = 1.0f;

					s_mScaleMatrix[0] = *pScales;
					s_mScaleMatrix[4] = *pScales;
					s_mScaleMatrix[8] = 1.0f;

					s_mTransformationMatrix[0] = (s_mRotationMatrix[0] * s_mScaleMatrix[0]);
					s_mTransformationMatrix[1] = (s_mRotationMatrix[1] * s_mScaleMatrix[4]);
					s_mTransformationMatrix[3] = (s_mRotationMatrix[3] * s_mScaleMatrix[0]);
					s_mTransformationMatrix[4] = (s_mRotationMatrix[4] * s_mScaleMatrix[4]);
					s_mTransformationMatrix[8] = (s_mRotationMatrix[8] * s_mScaleMatrix[8]);
#endif
					vPos->x = (float)pPos->x * s_fScaledPixelWidth;
					vPos->y = (float)pPos->y * s_fScaledPixelHeight;
					vPos->z = (float)pPos->z;

					// Build the vertex positions
					{
						fScaledWidth = (*pHWidths * s_fScaledPixelWidth);
						fScaledHeight = (*pHHeights * s_fScaledPixelHeight);

						vQuadTL_Pos->x = 0.0f - fScaledWidth;
						vQuadTL_Pos->y = 0.0f + fScaledHeight;
#ifdef GLM_MATRIX
						vQuadTL_Pos->z = 0.0f;   // TO_DO: need to put z-layer in here
						vQuadTL_Pos->w = 1.0f;   
#endif
						vQuadTR_Pos->x = 0.0f + fScaledWidth;
						vQuadTR_Pos->y = 0.0f + fScaledHeight;
#ifdef GLM_MATRIX
 						vQuadTR_Pos->z = 0.0f;   // TO_DO: need to put z-layer in here
						vQuadTR_Pos->w = 1.0f;   
#endif
						vQuadBL_Pos->x = 0.0f - fScaledWidth;
						vQuadBL_Pos->y = 0.0f - fScaledHeight;
#ifdef GLM_MATRIX
						vQuadBL_Pos->z = 0.0f;   // TO_DO: need to put z-layer in here
						vQuadBL_Pos->w = 1.0f;   
#endif
						vQuadBR_Pos->x = 0.0f + fScaledWidth;
						vQuadBR_Pos->y = 0.0f - fScaledHeight;
#ifdef GLM_MATRIX
						vQuadBR_Pos->z = 0.0f;   // TO_DO: need to put z-layer in here                    
						vQuadBR_Pos->w = 1.0f;   
#endif

				}

#ifdef GLM_MATRIX
					// Transform the vertex positions
					// TODO: Move this into the Vertex Shader
					vTransBL = mTransform * *vQuadBL_Pos;
					vTransBR = mTransform * *vQuadBR_Pos;
					vTransTL = mTransform * *vQuadTL_Pos;
					vTransTR = mTransform * *vQuadTR_Pos;
#else
					
					vTransBL.x = (vQuadBL_Pos->x * s_mTransformationMatrix[0]) + (vQuadBL_Pos->y * s_mTransformationMatrix[1]) + vPos->x;
					vTransBL.y = (vQuadBL_Pos->y * s_mTransformationMatrix[3]) + (vQuadBL_Pos->y * s_mTransformationMatrix[4]) + vPos->y;
					vTransBL.z = vPos->z;

					vTransBR.x = (vQuadBR_Pos->x * s_mTransformationMatrix[0]) + (vQuadBR_Pos->y * s_mTransformationMatrix[1]) + vPos->x;
					vTransBR.y = (vQuadBR_Pos->x * s_mTransformationMatrix[3]) + (vQuadBR_Pos->y * s_mTransformationMatrix[4]) + vPos->y;
					vTransBR.z = vPos->z;

					vTransTL.x = (vQuadTL_Pos->x * s_mTransformationMatrix[0]) + (vQuadTL_Pos->y * s_mTransformationMatrix[1]) + vPos->x;
					vTransTL.y = (vQuadTL_Pos->x * s_mTransformationMatrix[3]) + (vQuadTL_Pos->y * s_mTransformationMatrix[4]) + vPos->y;
					vTransTL.z = vPos->z;

					vTransTR.x = (vQuadTR_Pos->x * s_mTransformationMatrix[0]) + (vQuadTR_Pos->y * s_mTransformationMatrix[1]) + vPos->x;
					vTransTR.y = (vQuadTR_Pos->x * s_mTransformationMatrix[3]) + (vQuadTR_Pos->y * s_mTransformationMatrix[4]) + vPos->y;
					vTransTR.z = vPos->z;
#endif

					// Populate the VBO vertex corners
					iColour = GetPackedColourV4(pColours);
					pVertex->_colour = iColour;
					pVertex->_uv[0] = *pX_S;
					pVertex->_uv[1] = *pY_TnT;
					pVertex->_position[0] = vTransBL.x;
					pVertex->_position[1] = vTransBL.y;
					pVertex->_position[2] = vTransBL.z;
					pVertex++;

					pVertex->_colour = iColour;
					pVertex->_uv[0] = *pX_SnS;
					pVertex->_uv[1] = *pY_TnT;
					pVertex->_position[0] = vTransBR.x;
					pVertex->_position[1] = vTransBR.y;
					pVertex->_position[2] = vTransBR.z;
					pVertex++;

					pVertex->_colour = iColour;
					pVertex->_uv[0] = *pX_S;
					pVertex->_uv[1] = *pY_T;
					pVertex->_position[0] = vTransTL.x;
					pVertex->_position[1] = vTransTL.y;
					pVertex->_position[2] = vTransTL.z;
					pVertex++;

					pVertex->_colour = iColour;
					pVertex->_uv[0] = *pX_SnS;
					pVertex->_uv[1] = *pY_TnT;
					pVertex->_position[0] = vTransBR.x;
					pVertex->_position[1] = vTransBR.y;
					pVertex->_position[2] = vTransBR.z;
					pVertex++;

					pVertex->_colour = iColour;
					pVertex->_uv[0] = *pX_SnS;
					pVertex->_uv[1] = *pY_T;
					pVertex->_position[0] = vTransTR.x;
					pVertex->_position[1] = vTransTR.y;
					pVertex->_position[2] = vTransTR.z;
					pVertex++;

					pVertex->_colour = iColour;
					pVertex->_uv[0] = *pX_S;
					pVertex->_uv[1] = *pY_T;
					pVertex->_position[0] = vTransTL.x;
					pVertex->_position[1] = vTransTL.y;
					pVertex->_position[2] = vTransTL.z;
					pVertex++;

					// VBO Vertex pointer is now pointing at the next sprite, so increment through the 
					// sprite settings arrays to get the data for the next iteration
					++pX_S;
					++pY_T;
					++pX_SnS;
					++pY_TnT;
					++pHWidths;
					++pHHeights;
					++pRots;
					++pScales;
					++pColours;
					++pPos;
				}
			}

			glUnmapBuffer(GL_ARRAY_BUFFER);

			DELETEX vQuadBL_Pos;
			DELETEX vQuadBR_Pos;
			DELETEX vQuadTL_Pos;
			DELETEX vQuadTR_Pos;
			DELETEX vPos;
		}


		void RenderVBO(const uint32 iSpriteCount, GLuint iID, const int iVBOSet)
		{
			// TO_DO: 
			//      Blend Func should be a parameter of the texture page. 
			//      Split additive textures off into their own pool with sep VBO
			//      Render this last for mobile. 
			//      
			
			GLint* pUniforms = GetActiveUniforms();
			glBindTexture( GL_TEXTURE_2D, iID );
			glUniform1i (pUniforms[UNIFORM_TEXTURE], 0);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBindBuffer ( GL_ARRAY_BUFFER, s_pVBOArrays[iVBOSet]->_aVBOs[s_pVBOArrays[iVBOSet]->_iVBOCounter]);
			ASSERT_GL_ERROR;

			if (iSpriteCount != 0)
			{
				glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, s_iSizeOfVertex, (void*)offsetof(Vertex_t, _position));
				ASSERT_GL_ERROR;

				glEnableVertexAttribArray(ATTRIB_VERTEX);
				ASSERT_GL_ERROR;

				glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, 1, s_iSizeOfVertex, (void*)offsetof(Vertex_t, _colour));
				ASSERT_GL_ERROR;

				glEnableVertexAttribArray(ATTRIB_COLOR);
				ASSERT_GL_ERROR;

				glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, s_iSizeOfVertex, (void*)offsetof(Vertex_t, _uv));
				ASSERT_GL_ERROR;

				glEnableVertexAttribArray(ATTRIB_TEXTURE);
				ASSERT_GL_ERROR;

				glDrawArrays(GL_TRIANGLES, 0, iSpriteCount * 6);
				ASSERT_GL_ERROR;            
			}

			s_pVBOArrays[iVBOSet]->_iVBOCounter++;
			if ( s_pVBOArrays[iVBOSet]->_iVBOCounter == 3) s_pVBOArrays[iVBOSet]->_iVBOCounter = 0;
		}


		void PopulateTilemapVBO(const float* pX_S,
			const float* pY_T,
			const float* pX_SnS,
			const float* pY_TnT,
			const float* pHWidths,
			const float* pHHeights,
			const float* pRots,
			const float* pScales,
			glm::vec4* pColours,
			glm::vec3* pPos,
			const uint32 iCount,
			const int iStaticVBO_Index)
		{
			glm::vec4* vQuadBL_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec4* vQuadBR_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec4* vQuadTL_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec4* vQuadTR_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec4 vTransBL = glm::vec4();
			glm::vec4 vTransBR = glm::vec4();
			glm::vec4 vTransTL = glm::vec4();
			glm::vec4 vTransTR = glm::vec4();
			glm::vec3* vPos = NEWX glm::vec3();


			GLuint iVBO_ID = s_pTilemapVBOs[iStaticVBO_Index];
			glBindBuffer(GL_ARRAY_BUFFER, s_pTilemapVBOs[iStaticVBO_Index]);
			ASSERT_GL_ERROR;
			Vertex_t* pVertex = (Vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			ASSERT(NULL != pVertex, "Unable to map to a tilemap's buffer. Have you passed the correct StaticVBO_Index?");

			// Traverse the sprite array
			{
				float fScaledWidth;
				float fScaledHeight;
				uint32 iColour;

				// For each sprite up to iCount
				for (uint32 i = 0; i < iCount; i++)
				{
					// TODO: Ignore sprites outside the frustrum

					// Build the transform matrix for this sprite
					s_mRotationMatrix[0] = (float)cos(*pRots);
					s_mRotationMatrix[1] = (float)sin(*pRots);
					s_mRotationMatrix[3] = (float)-sin(*pRots);
					s_mRotationMatrix[4] = (float)cos(*pRots);
					s_mRotationMatrix[8] = 1.0f;

					s_mScaleMatrix[0] = *pScales;
					s_mScaleMatrix[4] = *pScales;
					s_mScaleMatrix[8] = 1.0f;

					s_mTransformationMatrix[0] = (s_mRotationMatrix[0] * s_mScaleMatrix[0]);
					s_mTransformationMatrix[1] = (s_mRotationMatrix[1] * s_mScaleMatrix[4]);
					s_mTransformationMatrix[3] = (s_mRotationMatrix[3] * s_mScaleMatrix[0]);
					s_mTransformationMatrix[4] = (s_mRotationMatrix[4] * s_mScaleMatrix[4]);
					s_mTransformationMatrix[8] = (s_mRotationMatrix[8] * s_mScaleMatrix[8]);
					vPos->x = (float)pPos->x * s_fScaledPixelWidth;
					vPos->y = (float)pPos->y * s_fScaledPixelHeight;
					vPos->z = (float)pPos->z;

					// Build the vertex positions
					{
						fScaledWidth = (*pHWidths * s_fScaledPixelWidth);
						fScaledHeight = (*pHHeights * s_fScaledPixelHeight);

						vQuadTL_Pos->x = 0.0f - fScaledWidth;
						vQuadTL_Pos->y = 0.0f + fScaledHeight;

						vQuadTR_Pos->x = 0.0f + fScaledWidth;
						vQuadTR_Pos->y = 0.0f + fScaledHeight;

						vQuadBL_Pos->x = 0.0f - fScaledWidth;
						vQuadBL_Pos->y = 0.0f - fScaledHeight;

						vQuadBR_Pos->x = 0.0f + fScaledWidth;
						vQuadBR_Pos->y = 0.0f - fScaledHeight;
					}

					vTransBL.x = (vQuadBL_Pos->x * s_mTransformationMatrix[0]) + (vQuadBL_Pos->y * s_mTransformationMatrix[1]) + vPos->x;
					vTransBL.y = (vQuadBL_Pos->y * s_mTransformationMatrix[3]) + (vQuadBL_Pos->y * s_mTransformationMatrix[4]) + vPos->y;
					vTransBL.z = vPos->z;

					vTransBR.x = (vQuadBR_Pos->x * s_mTransformationMatrix[0]) + (vQuadBR_Pos->y * s_mTransformationMatrix[1]) + vPos->x;
					vTransBR.y = (vQuadBR_Pos->x * s_mTransformationMatrix[3]) + (vQuadBR_Pos->y * s_mTransformationMatrix[4]) + vPos->y;
					vTransBR.z = vPos->z;

					vTransTL.x = (vQuadTL_Pos->x * s_mTransformationMatrix[0]) + (vQuadTL_Pos->y * s_mTransformationMatrix[1]) + vPos->x;
					vTransTL.y = (vQuadTL_Pos->x * s_mTransformationMatrix[3]) + (vQuadTL_Pos->y * s_mTransformationMatrix[4]) + vPos->y;
					vTransTL.z = vPos->z;

					vTransTR.x = (vQuadTR_Pos->x * s_mTransformationMatrix[0]) + (vQuadTR_Pos->y * s_mTransformationMatrix[1]) + vPos->x;
					vTransTR.y = (vQuadTR_Pos->x * s_mTransformationMatrix[3]) + (vQuadTR_Pos->y * s_mTransformationMatrix[4]) + vPos->y;
					vTransTR.z = vPos->z;

					// Populate the VBO vertex corners
					iColour = GetPackedColourV4(pColours);
					pVertex->_colour = iColour;
					pVertex->_uv[0] = *pX_S;
					pVertex->_uv[1] = *pY_TnT;
					pVertex->_position[0] = vTransBL.x;
					pVertex->_position[1] = vTransBL.y;
					pVertex->_position[2] = vTransBL.z;
					pVertex++;

					pVertex->_colour = iColour;
					pVertex->_uv[0] = *pX_SnS;
					pVertex->_uv[1] = *pY_TnT;
					pVertex->_position[0] = vTransBR.x;
					pVertex->_position[1] = vTransBR.y;
					pVertex->_position[2] = vTransBR.z;
					pVertex++;

					pVertex->_colour = iColour;
					pVertex->_uv[0] = *pX_S;
					pVertex->_uv[1] = *pY_T;
					pVertex->_position[0] = vTransTL.x;
					pVertex->_position[1] = vTransTL.y;
					pVertex->_position[2] = vTransTL.z;
					pVertex++;

					pVertex->_colour = iColour;
					pVertex->_uv[0] = *pX_SnS;
					pVertex->_uv[1] = *pY_TnT;
					pVertex->_position[0] = vTransBR.x;
					pVertex->_position[1] = vTransBR.y;
					pVertex->_position[2] = vTransBR.z;
					pVertex++;

					pVertex->_colour = iColour;
					pVertex->_uv[0] = *pX_SnS;
					pVertex->_uv[1] = *pY_T;
					pVertex->_position[0] = vTransTR.x;
					pVertex->_position[1] = vTransTR.y;
					pVertex->_position[2] = vTransTR.z;
					pVertex++;

					pVertex->_colour = iColour;
					pVertex->_uv[0] = *pX_S;
					pVertex->_uv[1] = *pY_T;
					pVertex->_position[0] = vTransTL.x;
					pVertex->_position[1] = vTransTL.y;
					pVertex->_position[2] = vTransTL.z;
					pVertex++;

					// VBO Vertex pointer is now pointing at the next sprite, so increment through the 
					// sprite settings arrays to get the data for the next iteration
					++pX_S;
					++pY_T;
					++pX_SnS;
					++pY_TnT;
					++pHWidths;
					++pHHeights;
					++pRots;
					++pScales;
					++pColours;
					++pPos;
				}
			}

			glUnmapBuffer(GL_ARRAY_BUFFER);

			DELETEX vQuadBL_Pos;
			DELETEX vQuadBR_Pos;
			DELETEX vQuadTL_Pos;
			DELETEX vQuadTR_Pos;
			DELETEX vPos;
		}


		void RenderTilemapVBO(const uint32 iTilemapSize, GLuint iTextureID, const int iStaticVBO_Index)
		{
			GLint* pUniforms = GetActiveUniforms();
			glBindTexture(GL_TEXTURE_2D, iTextureID);
			glUniform1i(pUniforms[UNIFORM_TEXTURE], 0);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBindBuffer(GL_ARRAY_BUFFER,s_pTilemapVBOs[iStaticVBO_Index]);

			ASSERT_GL_ERROR;

			if (iTilemapSize != 0)
			{
				glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, s_iSizeOfVertex, (void*)offsetof(Vertex_t, _position));
				ASSERT_GL_ERROR;

				glEnableVertexAttribArray(ATTRIB_VERTEX);
				ASSERT_GL_ERROR;

				glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, 1, s_iSizeOfVertex, (void*)offsetof(Vertex_t, _colour));
				ASSERT_GL_ERROR;

				glEnableVertexAttribArray(ATTRIB_COLOR);
				ASSERT_GL_ERROR;

				glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, s_iSizeOfVertex, (void*)offsetof(Vertex_t, _uv));
				ASSERT_GL_ERROR;

				glEnableVertexAttribArray(ATTRIB_TEXTURE);
				ASSERT_GL_ERROR;

				glDrawArrays(GL_TRIANGLES, 0, iTilemapSize * 6);
				ASSERT_GL_ERROR;
			}
		}




#ifdef DEBUG 
		void AllocateDebugVBOs()
		{
				s_pDebugVBOs = NEWX(DynamicVBO_t);
				s_pDebugVBOs->_iVBOCounter = 0;

				glGenBuffers(1, &s_pDebugVBOs->_aVBOs[0]); 
				ASSERT_GL_ERROR;
				glBindBuffer(GL_ARRAY_BUFFER, s_pDebugVBOs->_aVBOs[0]); 
				ASSERT_GL_ERROR;
				glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * iMAX_SPRITES, NULL, GL_DYNAMIC_DRAW  );
				ASSERT_GL_ERROR;

				glGenBuffers(1, &s_pDebugVBOs->_aVBOs[1]); 
				ASSERT_GL_ERROR;
				glBindBuffer(GL_ARRAY_BUFFER, s_pDebugVBOs->_aVBOs[1]); 
				ASSERT_GL_ERROR;
				glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * iMAX_SPRITES, NULL,  GL_DYNAMIC_DRAW  );
				ASSERT_GL_ERROR;

				glGenBuffers(1, &s_pDebugVBOs->_aVBOs[2]); 
				ASSERT_GL_ERROR;
				glBindBuffer(GL_ARRAY_BUFFER, s_pDebugVBOs->_aVBOs[2]); 
				ASSERT_GL_ERROR;
				glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * iMAX_SPRITES, NULL, GL_DYNAMIC_DRAW  );
				ASSERT_GL_ERROR;
			}


			void DeallocateDebugVBOs()
			{
				glDeleteBuffers( 1, &s_pDebugVBOs->_aVBOs[0] );
				GL_ERROR;                
				glDeleteBuffers( 1, &s_pDebugVBOs->_aVBOs[1] );
				GL_ERROR;                
				glDeleteBuffers( 1, &s_pDebugVBOs->_aVBOs[2] );
				GL_ERROR;                

				DELETEX s_pDebugVBOs;
				LOG_INFO("Debug VBOs Deallocated.");
			}



			// This is an untextured variant of the Populate VBO function with hardcoded UV to maintain the vertext_t structure in the VBO
			// It's not really worth generalising between the two  as this is only used in the editor modes and not the main RELEASE runtime. 
			void PopulateDebugVBO(const float* pHWidths, 
														const float* pHHeights, 
														const float* pRots, 
														const float* pScales, 
														glm::vec4* pColours, 
														glm::vec3* pPos, 
														const uint32 iCount)
			{
				ASSERT(iCount < iMAX_SPRITES, "PopulateDebugVBO: Sprite count is greater than VBO limits, something's gone very horribly wrong...");

				glm::vec4* vQuadBL_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
				glm::vec4* vQuadBR_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
				glm::vec4* vQuadTL_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
				glm::vec4* vQuadTR_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
				glm::vec4 vTransBL = glm::vec4();
				glm::vec4 vTransBR = glm::vec4();
				glm::vec4 vTransTL = glm::vec4();
				glm::vec4 vTransTR = glm::vec4();
				glm::vec3* vPos = NEWX glm::vec3();

				// Get the position of the first vertex in the VBO
				GLuint iVBO_ID = s_pDebugVBOs->_aVBOs[s_pDebugVBOs->_iVBOCounter];

				glBindBuffer( GL_ARRAY_BUFFER, iVBO_ID );
				ASSERT_GL_ERROR;
				Vertex_t* pVertex = (Vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

				// Traverse the sprite arrays
				{
#ifdef GLM_MATRIX
					glm::mat4 mScale = glm::mat4(1.0f);
					glm::mat4 mRotation = glm::mat4(1.0f);
					glm::mat4 mTranslate = glm::mat4(1.0f);
					glm::mat4 mTransform = glm::mat4(1.0f);
#endif
					float fScaledWidth;
					float fScaledHeight;
					uint32 iColour;

					// For each sprite up to iCount
					for(uint32 i=0; i<iCount; i++)
					{
#ifdef GLM_MATRIX
						// Build the transform matrix for this sprite
						mScale[0].x = *pScales;
						mScale[1].y = *pScales;

						mRotation[0].x = (float)cos(*pRots);
						mRotation[0].y = (float)-sin(*pRots);
						mRotation[1].x = (float)sin(*pRots);
						mRotation[1].y = (float)cos(*pRots);
						mTranslate[3] = glm::vec4(vPos->x, vPos->y, vPos->z, 1.0f);

						// This is the slowest line in here. 
						// TODO: Possible to get a speed gain by removing the operator* ? Would remove the vec4 constructors
						// glm::operator*<float,0>	Neutrino	e:\bitbucket\neutrino\external_dependencies\glm\glm\detail\type_mat4x4.inl	Line: 608	
						mTransform = mTranslate * mRotation * mScale;
#else
						s_mRotationMatrix[0] = (float)cos(*pRots);
						s_mRotationMatrix[1] = (float)sin(*pRots);
						s_mRotationMatrix[3] = (float)-sin(*pRots);
						s_mRotationMatrix[4] = (float)cos(*pRots);
						s_mRotationMatrix[8] = 1.0f;

						s_mScaleMatrix[0] = *pScales;
						s_mScaleMatrix[4] = *pScales;
						s_mScaleMatrix[8] = 1.0f;

						s_mTransformationMatrix[0] = (s_mRotationMatrix[0] * s_mScaleMatrix[0]);
						s_mTransformationMatrix[1] = (s_mRotationMatrix[1] * s_mScaleMatrix[4]);
						s_mTransformationMatrix[3] = (s_mRotationMatrix[3] * s_mScaleMatrix[0]);
						s_mTransformationMatrix[4] = (s_mRotationMatrix[4] * s_mScaleMatrix[4]);
						s_mTransformationMatrix[8] = (s_mRotationMatrix[8] * s_mScaleMatrix[8]);
#endif
						vPos->x = (float)pPos->x * s_fScaledPixelWidth;
						vPos->y = (float)pPos->y * s_fScaledPixelHeight;
						vPos->z = (float)pPos->z;
						// Build the vertex positions
						{
							fScaledWidth = (*pHWidths * s_fScaledPixelWidth);
							fScaledHeight = (*pHHeights * s_fScaledPixelHeight);

							vQuadTL_Pos->x = 0.0f - fScaledWidth;
							vQuadTL_Pos->y = 0.0f + fScaledHeight;
#ifdef GLM_MATRIX
							vQuadTL_Pos->z = 0.0f;   // TO_DO: need to put z-layer in here
							vQuadTL_Pos->w = 1.0f;   
#endif
							vQuadTR_Pos->x = 0.0f + fScaledWidth;
							vQuadTR_Pos->y = 0.0f + fScaledHeight;
#ifdef GLM_MATRIX							
							vQuadTR_Pos->z = 0.0f;   // TO_DO: need to put z-layer in here
							vQuadTR_Pos->w = 1.0f;   
#endif
							vQuadBL_Pos->x = 0.0f - fScaledWidth;
							vQuadBL_Pos->y = 0.0f - fScaledHeight;
#ifdef GLM_MATRIX							
							vQuadBL_Pos->z = 0.0f;   // TO_DO: need to put z-layer in here
							vQuadBL_Pos->w = 1.0f;   
#endif
							vQuadBR_Pos->x = 0.0f + fScaledWidth;
							vQuadBR_Pos->y = 0.0f - fScaledHeight;
#ifdef GLM_MATRIX							
							vQuadBR_Pos->z = 0.0f;   // TO_DO: need to put z-layer in here                    
							vQuadBR_Pos->w = 1.0f;   
#endif
						}

#ifdef GLM_MATRIX
						// Transform the vertex positions
						vTransBL = mTransform * *vQuadBL_Pos;
						vTransBR = mTransform * *vQuadBR_Pos;
						vTransTL = mTransform * *vQuadTL_Pos;
						vTransTR = mTransform * *vQuadTR_Pos;
#else
						vTransBL.x = (vQuadBL_Pos->x * s_mTransformationMatrix[0]) + (vQuadBL_Pos->y * s_mTransformationMatrix[1]) + vPos->x;
						vTransBL.y = (vQuadBL_Pos->y * s_mTransformationMatrix[3]) + (vQuadBL_Pos->y * s_mTransformationMatrix[4]) + vPos->y;
						vTransBL.z = vPos->z;

						vTransBR.x = (vQuadBR_Pos->x * s_mTransformationMatrix[0]) + (vQuadBR_Pos->y * s_mTransformationMatrix[1]) + vPos->x;
						vTransBR.y = (vQuadBR_Pos->x * s_mTransformationMatrix[3]) + (vQuadBR_Pos->y * s_mTransformationMatrix[4]) + vPos->y;
						vTransBR.z = vPos->z;

						vTransTL.x = (vQuadTL_Pos->x * s_mTransformationMatrix[0]) + (vQuadTL_Pos->y * s_mTransformationMatrix[1]) + vPos->x;
						vTransTL.y = (vQuadTL_Pos->x * s_mTransformationMatrix[3]) + (vQuadTL_Pos->y * s_mTransformationMatrix[4]) + vPos->y;
						vTransTL.z = vPos->z;

						vTransTR.x = (vQuadTR_Pos->x * s_mTransformationMatrix[0]) + (vQuadTR_Pos->y * s_mTransformationMatrix[1]) + vPos->x;
						vTransTR.y = (vQuadTR_Pos->x * s_mTransformationMatrix[3]) + (vQuadTR_Pos->y * s_mTransformationMatrix[4]) + vPos->y;
						vTransTR.z = vPos->z;
#endif
						// Get the packed colour
						iColour = GetPackedColourV4(pColours);

						// Populate the VBO vertex corners
						pVertex->_colour = iColour;
						pVertex->_position[0] = vTransBL.x;
						pVertex->_position[1] = vTransBL.y;
						pVertex->_position[2] = vTransBL.z;
						pVertex++;

						pVertex->_colour = iColour;
						pVertex->_position[0] = vTransBR.x;
						pVertex->_position[1] = vTransBR.y;
						pVertex->_position[2] = vTransBR.z;
						pVertex++;

						pVertex->_colour = iColour;
						pVertex->_position[0] = vTransTL.x;
						pVertex->_position[1] = vTransTL.y;
						pVertex->_position[2] = vTransTL.z;
						pVertex++;

						pVertex->_colour = iColour;
						pVertex->_position[0] = vTransBR.x;
						pVertex->_position[1] = vTransBR.y;
						pVertex->_position[2] = vTransBR.z;
						pVertex++;

						pVertex->_colour = iColour;
						pVertex->_position[0] = vTransTR.x;
						pVertex->_position[1] = vTransTR.y;
						pVertex->_position[2] = vTransTR.z;
						pVertex++;

						pVertex->_colour = iColour;
						pVertex->_position[0] = vTransTL.x;
						pVertex->_position[1] = vTransTL.y;
						pVertex->_position[2] = vTransTL.z;
						pVertex++;

						// VBO Vertex pointer is now pointing at the next sprite, so increment through the 
						// sprite settings arrays to get the data for the next iteration
						++pHWidths;
						++pHHeights;
						++pRots;
						++pScales;
						++pColours;
						++pPos;
					}
				}

				glUnmapBuffer(GL_ARRAY_BUFFER);

				DELETEX vQuadBL_Pos;
				DELETEX vQuadBR_Pos;
				DELETEX vQuadTL_Pos;
				DELETEX vQuadTR_Pos;
				DELETEX vPos;
			}


			void RenderDebugVBO(const uint32 iSpriteCount)
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBindBuffer ( GL_ARRAY_BUFFER, s_pDebugVBOs->_aVBOs[s_pDebugVBOs->_iVBOCounter]);
				ASSERT_GL_ERROR;

				if (iSpriteCount != 0)
				{
					glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, s_iSizeOfVertex, (void*)offsetof(Vertex_t, _position));
					ASSERT_GL_ERROR;

					glEnableVertexAttribArray(ATTRIB_VERTEX);
					ASSERT_GL_ERROR;

					glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, 1, s_iSizeOfVertex, (void*)offsetof(Vertex_t, _colour));
					ASSERT_GL_ERROR;

					glEnableVertexAttribArray(ATTRIB_COLOR);
					ASSERT_GL_ERROR;

					glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, s_iSizeOfVertex, (void*)offsetof(Vertex_t, _uv));
					ASSERT_GL_ERROR;

					glEnableVertexAttribArray(ATTRIB_TEXTURE);
					ASSERT_GL_ERROR;

					glDrawArrays(GL_TRIANGLES, 0, iSpriteCount * 6);
					ASSERT_GL_ERROR;            
				}

				s_pDebugVBOs->_iVBOCounter++;
				if ( s_pDebugVBOs->_iVBOCounter == 3) s_pDebugVBOs->_iVBOCounter = 0;
			}
#endif
		} // GLUtils
}	// Neutrino
