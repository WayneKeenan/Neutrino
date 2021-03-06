#include "GLUtils.h"
#include "Log.h"
#include "Types.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Memory.h"
#include "ShaderUtils.h"
#include "Colour.h"

namespace Neutrino {

	namespace GLUtils {

		// Transform matrices
		static GLfloat s_mTransformationMatrix[9] = { 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f };
		static GLfloat s_mScaleMatrix[9] = { 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f };
		static GLfloat s_mRotationMatrix[9] = { 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f };
		static glm::mat4 s_mCameraMatrix;
		static glm::mat4 s_mFinalOutputCameraMatrix;
		static glm::mat4 s_mModelViewMatrix;
		static glm::mat4 s_mFinalOutputModelViewMatrix;
		static glm::mat4 s_mProjectionMatrix;
		static glm::mat4 s_mFinalOutputProjectionMatrix;
		static glm::vec4 s_vClearColour = glm::vec4(0.25f, 0.25f, 0.0f, 1.0f);

		// OGL Ranges
		static float s_fOGL_X_RANGE;
		static float s_fOGL_Y_RANGE;

		// Internal and External Viewport dimensions. 
		static float s_fViewportWidth;
		static float s_fViewportHeight;
		static GLsizei s_iInternalWidth;
		static GLsizei s_iInternalHeight;

		// Pixel scaler for low res render output 
		static float s_fScaledPixelWidth;
		static float s_fScaledPixelHeight;

		// Dimensions for the blur render texture
		static float s_fBlurWidth;
		static float s_fBlurHeight;
		static const float s_fBlurDivisor = 4.0f;

		// Pixel scaler for editor modes
		static float s_fUnscaledPixelWidth;
		static float s_fUnscaledPixelHeight;

		// Constants referenced when drawing arrays
		static const int s_iSizeOfSprite = 6 * sizeof(Vertex_t);
		static const int s_iSizeOfVertex = sizeof(Vertex_t);

		// Static allocated arrays for the buffered VBOs
		static DynamicVBO_t* s_pVBOArrays[_iMAX_TEXTURES];
		static DynamicVBO_t* s_pDebugVBOs = NULL;
		static GLuint s_pTilemapVBOs[_iMAX_TILEMAPS];
		static GLuint s_iFullScreenQuadVBO;

		// FBO / RBO pointers for the final compositing stages
		static GLuint s_iFBO_PixelRenderTarget;
		static GLuint s_iFBO_BloomThresholdTexture;
		static GLuint s_iFBO_BlurHorizTexture;
		static GLuint s_iFBO_BlurVertTexture;
		static GLuint s_iFBOID;

		// Controls for the shaders when doing the final composit. These are loaded
		// from a file during Init and can be tweaked in Debug builds. 
		static const Neutrino::PostProcessSettings_t* s_pPostProcessSettings;

		// Counters
		static uint8 s_iAllocatedDynamicVBOSets = 0;
		static uint8 s_iAllocatedTilemapVBOs = 0;

		// Dimensions
		static glm::vec2 s_vViewportDimensions;
		static glm::vec2 s_vInternalDimensions;


		float* GetCameraMatrix()
		{
			return glm::value_ptr(s_mCameraMatrix);
		}

		// TODO: Get Box2DDebugWorldMatrix

		void SetClearColour(const float fR, const float fG, const float fB, const float fA)
		{
			s_vClearColour.x = fR; s_vClearColour.y = fG; s_vClearColour.z = fB; s_vClearColour.w = fA;
			//LOG_INFO("Clear Colour Set to: %.2f, %.2f, %.2f, %.2f",  s_vClearColour.x, s_vClearColour.y, s_vClearColour.z, s_vClearColour.w );
		}

		void SetPostProcessSettings(const PostProcessSettings_t* pSettings)
		{
			s_pPostProcessSettings = pSettings;
		}

		void ClearBuffers()
		{
			glClearColor(s_vClearColour.x, s_vClearColour.y, s_vClearColour.z, s_vClearColour.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void SetViewport(const int iViewportWidth, const int iViewportHeight)
		{
			// Create an OGL viewport with basic settings
			{
				glViewport(0, 0, iViewportWidth, iViewportHeight);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_BLEND);
				glActiveTexture(GL_TEXTURE0);
				glClearColor(s_vClearColour.x, s_vClearColour.y, s_vClearColour.z, s_vClearColour.w);
				glClearDepth(1.0f);
				glDepthFunc(GL_LEQUAL);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				ASSERT_GL_ERROR;
			}
		}

		void SetDimensions(const int iViewportWidth, const int iViewportHeight, const int iInternalWidth, const int iInternalHeight)
		{
			// Set viewport dimensions, OGL coord range, and internal pixel dimensions
			{
				s_fViewportWidth = (float)iViewportWidth;
				s_fViewportHeight = (float)iViewportHeight;
				s_fBlurWidth = s_fViewportWidth / s_fBlurDivisor;
				s_fBlurHeight = s_fViewportHeight / s_fBlurDivisor;
				s_iInternalWidth = iInternalWidth;
				s_iInternalHeight = iInternalHeight;

				// Store dimensions in two vectors we'll pass to the shader uniforms during rendering 
				s_vViewportDimensions.x = s_fViewportWidth;
				s_vViewportDimensions.y = s_fViewportHeight;
				s_vInternalDimensions.x = (float)iInternalWidth;
				s_vInternalDimensions.y = (float)iInternalHeight;

				// The aspect ratios for the viewport and the internal render buffers are
				// assumed to be the same: 1.777 by 1. (Widescreen)
				s_fOGL_X_RANGE = s_fViewportWidth / s_fViewportHeight;
				s_fOGL_Y_RANGE = 1.0f;

				s_fScaledPixelWidth = s_fOGL_X_RANGE / (float)iInternalWidth;
				s_fScaledPixelHeight = s_fOGL_Y_RANGE / (float)iInternalHeight;

				s_fUnscaledPixelWidth = s_fOGL_X_RANGE / (float)iViewportWidth;
				s_fUnscaledPixelHeight = s_fOGL_Y_RANGE / (float)iViewportHeight;
			}
			
			// Set the projection matrix for this viewport. 0,0 at TOP LEFT.
			s_mProjectionMatrix = glm::ortho(0.0f, s_fOGL_X_RANGE, s_fOGL_Y_RANGE, 0.0f, 1.0f, -10.0f);

			// Define a camera matrix for the final output of the low res texture to the screen. 
			s_mFinalOutputModelViewMatrix = glm::mat4(1.0f);   
			s_mFinalOutputCameraMatrix = s_mProjectionMatrix * s_mFinalOutputModelViewMatrix;
		}

		const glm::vec2 GetViewportDimensions()
		{
			return s_vViewportDimensions;
		}

		const glm::vec2 GetInternalDimensions()
		{
			return s_vInternalDimensions;
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

			// int iX = (int)(vPos->x / s_fScaledPixelWidth);
			// int iY = (int)(vPos->y / s_fScaledPixelHeight);
			// glm::mat4 mTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(iX*s_fScaledPixelWidth, iY*s_fScaledPixelHeight, 0.0f));
			glm::mat4 mTranslationMatrix = glm::translate(glm::mat4(1.0f), *vPos);
			s_mModelViewMatrix = mTranslationMatrix;    // * mRotationMatrix;  // Add in scale? Are we going to zoom in and out?
			s_mCameraMatrix = s_mProjectionMatrix * s_mModelViewMatrix;
		}


		bool AllocateFBOs()
		{
			// Create two textures for this FBO
			// Note, not using Mipmaps here - just linear filter the textures when miniturising. 
			glGenTextures(1, &s_iFBO_PixelRenderTarget);
			ASSERT_GL_ERROR;
			glBindTexture(GL_TEXTURE_2D, s_iFBO_PixelRenderTarget);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			ASSERT_GL_ERROR;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, s_iInternalWidth, s_iInternalHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			ASSERT_GL_ERROR;
			glBindTexture(GL_TEXTURE_2D, 0);
			ASSERT_GL_ERROR;


			glGenTextures(1, &s_iFBO_BloomThresholdTexture);
			ASSERT_GL_ERROR;
			glBindTexture(GL_TEXTURE_2D, s_iFBO_BloomThresholdTexture);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			ASSERT_GL_ERROR;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, s_iInternalWidth, s_iInternalHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			ASSERT_GL_ERROR;
			glBindTexture(GL_TEXTURE_2D, 0);
			ASSERT_GL_ERROR;

			glGenTextures(1, &s_iFBO_BlurHorizTexture);
			ASSERT_GL_ERROR;
			glBindTexture(GL_TEXTURE_2D, s_iFBO_BlurHorizTexture);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			ASSERT_GL_ERROR;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)s_fBlurWidth, (GLsizei)s_fBlurHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			ASSERT_GL_ERROR;
			glBindTexture(GL_TEXTURE_2D, 0);
			ASSERT_GL_ERROR;

			glGenTextures(1, &s_iFBO_BlurVertTexture);
			ASSERT_GL_ERROR;
			glBindTexture(GL_TEXTURE_2D, s_iFBO_BlurVertTexture);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			ASSERT_GL_ERROR;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			ASSERT_GL_ERROR;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)s_fBlurWidth, (GLsizei)s_fBlurHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			ASSERT_GL_ERROR;
			glBindTexture(GL_TEXTURE_2D, 0);
			ASSERT_GL_ERROR;


			// Create the framebuffer object...
			glGenFramebuffersEXT(1, &s_iFBOID);
			ASSERT_GL_ERROR;
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, s_iFBOID);
			ASSERT_GL_ERROR;
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, s_iFBO_PixelRenderTarget, 0);
			ASSERT_GL_ERROR;
			GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

			if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
			{
				LOG_ERROR("Unable to create FBO object");
				return false;
			}

			// Clear any garbage that's in the two textures we've just allocated
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, s_iFBOID);
			glClearColor(0.0, 0.0, 0.0, 0.0);
			glViewport(0, 0, s_iInternalWidth, s_iInternalHeight);
			glDisable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, s_iFBO_PixelRenderTarget, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, s_iFBO_BloomThresholdTexture, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			// Generate a VBO for the final output
			glGenBuffers(1, &s_iFullScreenQuadVBO);
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_iFullScreenQuadVBO);
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite, NULL, GL_STATIC_DRAW);
			ASSERT_GL_ERROR;

			// Grab the VBO for the output sprite
			glBindBuffer(GL_ARRAY_BUFFER, s_iFullScreenQuadVBO);
			ASSERT_GL_ERROR;

			// Map to it
			Vertex_t* pVertex = (Vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			ASSERT_GL_ERROR;
			ASSERT(NULL != pVertex, "Unable to map to a buffer. Have you passed the correct VBO Index");

			// Write a quad to the VBO
			uint32 iColour = 0xFFFFFFFF;

			pVertex->_colour = iColour;
			pVertex->_uv[0] = 0.0f;
			pVertex->_uv[1] = 0.0f;
			pVertex->_position[0] = 0.0f;
			pVertex->_position[1] = s_fOGL_Y_RANGE;
			pVertex->_position[2] = 0.0f;
			pVertex++;

			pVertex->_colour = iColour;
			pVertex->_uv[0] = 1.0f;
			pVertex->_uv[1] = 0.0f;
			pVertex->_position[0] = s_fOGL_X_RANGE;
			pVertex->_position[1] = s_fOGL_Y_RANGE;
			pVertex->_position[2] = 0.0f;
			pVertex++;

			pVertex->_colour = iColour;
			pVertex->_uv[0] = 0.0f;
			pVertex->_uv[1] = 1.0f;
			pVertex->_position[0] = 0.0f;
			pVertex->_position[1] = 0.0f;
			pVertex->_position[2] = 0.0f;
			pVertex++;

			pVertex->_colour = iColour;
			pVertex->_uv[0] = 1.0f;
			pVertex->_uv[1] = 0.0f;
			pVertex->_position[0] = s_fOGL_X_RANGE;
			pVertex->_position[1] = s_fOGL_Y_RANGE;
			pVertex->_position[2] = 0.0f;
			pVertex++;

			pVertex->_colour = iColour;
			pVertex->_uv[0] = 1.0f;
			pVertex->_uv[1] = 1.0f;
			pVertex->_position[0] = s_fOGL_X_RANGE;
			pVertex->_position[1] = 0.0f;
			pVertex->_position[2] = 0.0f;
			pVertex++;

			pVertex->_colour = iColour;
			pVertex->_uv[0] = 0.0f;
			pVertex->_uv[1] = 1.0f;
			pVertex->_position[0] = 0.0f;
			pVertex->_position[1] = 0.0f;
			pVertex->_position[2] = 0.0f;
			pVertex++;

			glUnmapBuffer(GL_ARRAY_BUFFER);

			// Return to the default render output settings. 
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			glViewport(0, 0, (GLsizei)s_fViewportWidth, (GLsizei)s_fViewportHeight);
			glClearColor(s_vClearColour.x, s_vClearColour.y, s_vClearColour.z, s_vClearColour.w);
			glEnable(GL_DEPTH_TEST);
				
			return true;
		}


		void DeallocateFBOs()
		{
			glDeleteFramebuffersEXT(1, &s_iFBOID);
			glDeleteTextures(1, &s_iFBO_PixelRenderTarget);
			glDeleteTextures(1, &s_iFBO_BloomThresholdTexture);
			glDeleteTextures(1, &s_iFBO_BlurHorizTexture);
			glDeleteTextures(1, &s_iFBO_BlurVertTexture);
			glDeleteBuffers(1, &s_iFullScreenQuadVBO);
			GL_ERROR;
			LOG_INFO("FBOs and Render Textures Deallocated");
		}



		void AllocateDynamicVBOSet()
		{
			ASSERT(s_iAllocatedDynamicVBOSets < _iMAX_TEXTURES, "Call to AllocateDynamicVBOSet made when max textures has been reached.");

			s_pVBOArrays[s_iAllocatedDynamicVBOSets] = NEWX(DynamicVBO_t);
			s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_iVBOCounter = 0;

			glGenBuffers(1, &s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_aVBOs[0]);
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_aVBOs[0]);
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * _iMAX_SPRITES, NULL, GL_STREAM_DRAW);
			ASSERT_GL_ERROR;

			glGenBuffers(1, &s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_aVBOs[1]);
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_aVBOs[1]);
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * _iMAX_SPRITES, NULL, GL_STREAM_DRAW);
			ASSERT_GL_ERROR;

			glGenBuffers(1, &s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_aVBOs[2]);
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_pVBOArrays[s_iAllocatedDynamicVBOSets]->_aVBOs[2]);
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * _iMAX_SPRITES, NULL, GL_STREAM_DRAW);
			ASSERT_GL_ERROR;

			++s_iAllocatedDynamicVBOSets;
		}


		void DeallocateDynamicVBOs()
		{
			for (int i = 0; i < s_iAllocatedDynamicVBOSets; i++)
			{
				glDeleteBuffers(1, &s_pVBOArrays[i]->_aVBOs[0]);
				GL_ERROR;
				glDeleteBuffers(1, &s_pVBOArrays[i]->_aVBOs[1]);
				GL_ERROR;
				glDeleteBuffers(1, &s_pVBOArrays[i]->_aVBOs[2]);
				GL_ERROR;

				DELETEX s_pVBOArrays[i];
			}

			LOG_INFO("Dynamic VBOs Deallocated.");
		}


		uint8 CreateTilemapVBO(const uint32 iTilemapSize)
		{
			ASSERT(s_iAllocatedTilemapVBOs < _iMAX_TILEMAPS, "Call to CreateTilemapVBO made when max tilemaps has been reached.");
			glGenBuffers(1, &s_pTilemapVBOs[s_iAllocatedTilemapVBOs]);
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_pTilemapVBOs[s_iAllocatedTilemapVBOs]);
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * iTilemapSize, NULL, GL_STATIC_DRAW);
			ASSERT_GL_ERROR;
			++s_iAllocatedTilemapVBOs;
			return (uint8)(s_iAllocatedTilemapVBOs - 1);
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
			GLenum err(glGetError());

			while (err != GL_NO_ERROR)
			{
				LOG_INFO("glError hit: %d %s %d", err, pFile, iLine);
				switch (err)
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
				err = glGetError();
			}
			return bReturnVal;
		}



		void PopulateVBO(const float* pX_S,
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
			const int iVBO_Index,
			bool bIsScaled,
			bool bIsTilemap)
		{
			if (!bIsTilemap)	ASSERT(iCount < _iMAX_SPRITES, "Sprite count is greater than VBO limits, something's gone very horribly wrong...");

			glm::vec4* vQuadBL_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec4* vQuadBR_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec4* vQuadTL_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec4* vQuadTR_Pos = NEWX glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec4 vTransBL = glm::vec4();
			glm::vec4 vTransBR = glm::vec4();
			glm::vec4 vTransTL = glm::vec4();
			glm::vec4 vTransTR = glm::vec4();
			glm::vec3* vPos = NEWX glm::vec3();

			GLuint iVBO_ID;
			if (!bIsTilemap)
				iVBO_ID = s_pVBOArrays[iVBO_Index]->_aVBOs[s_pVBOArrays[iVBO_Index]->_iVBOCounter];
			else
				iVBO_ID = s_pTilemapVBOs[iVBO_Index];

			glBindBuffer(GL_ARRAY_BUFFER, iVBO_ID);
			ASSERT_GL_ERROR;
			Vertex_t* pVertex = (Vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			ASSERT(NULL != pVertex, "Unable to map to a buffer. Have you passed the correct VBO Index");

			// Traverse the sprite arrays
			// TODO: Split this into 4 and spread across threads
			{
				uint32 iColour;
				float fScaledWidth, fScaledHeight, fPixelWidth, fPixelHeight; //, fHalfPixelWidth, fHalfPixelHeight;

				// We're scaled if we are rendering to the low res FBO, otherwise this is probably an editor mode. 
				if (!bIsScaled)
				{
					fPixelWidth = s_fUnscaledPixelWidth;
					fPixelHeight = s_fUnscaledPixelHeight;

				}
				else
				{
					fPixelWidth = s_fScaledPixelWidth;
					fPixelHeight = s_fScaledPixelHeight;
				}

				//fHalfPixelWidth = fPixelWidth / 2.0f;
				//fHalfPixelHeight = fPixelHeight / 2.0f;

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

					vPos->x = ((float)pPos->x * fPixelWidth);
					vPos->y = ((float)pPos->y * fPixelHeight);
					vPos->z = (float)pPos->z;

					// Build the vertex positions
					{
						fScaledWidth = (*pHWidths * fPixelWidth);
						fScaledHeight = (*pHHeights * fPixelHeight);

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


		void RenderVBO(const uint32 iSpriteCount, GLuint iID, const int iVBOSet)
		{
			// TO_DO: 
			//      Blend Func should be a parameter of the texture page. 
			//      Split additive textures off into their own pool with sep VBO
			//      Render this last for mobile. 
			//      

			GLint* pUniforms = GetActiveUniforms();
			glBindTexture(GL_TEXTURE_2D, iID);
			glUniform1i(pUniforms[UNIFORM_TEXTURE], 0);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBindBuffer(GL_ARRAY_BUFFER, s_pVBOArrays[iVBOSet]->_aVBOs[s_pVBOArrays[iVBOSet]->_iVBOCounter]);
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
			if (s_pVBOArrays[iVBOSet]->_iVBOCounter == 3) s_pVBOArrays[iVBOSet]->_iVBOCounter = 0;
		}


		void RenderTilemapVBO(const uint32 iTilemapSize, GLuint iTextureID, const int iStaticVBO_Index)
		{
			GLint* pUniforms = GetActiveUniforms();
			glBindTexture(GL_TEXTURE_2D, iTextureID);
			glUniform1i(pUniforms[UNIFORM_TEXTURE], 0);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBindBuffer(GL_ARRAY_BUFFER, s_pTilemapVBOs[iStaticVBO_Index]);

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

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

		//
		// Render Stage:
		// 1. All sprites, tilemaps etc are rendered to a low resolution RT: s_iFBO_PixelRenderTarget
		// 2. This RT is then rendered to a second RT, via a high pass threshold shader: s_iFBO_BloomThresholdTexture
		// 3. s_iFBO_BloomThresholdTexture is then blurred in two passes to a third RT, giving us our "bloom"
		// 4. s_iFBO_PixelRenderTarget is rendered to the screen (full screen quad) through a scanline/CRT shader
		// 5. The blurred, "bloom" texture is then rendered to the screen, over the top, to give us the final output. 
		// 
		// Note: these render stages can be toggled on or off, so it's possible to just have scanlines, or just have bloom etc. 
		//

		void StartOffscreenRender()
		{
			// We're going to redirect all sprite output to a low resolution render texture, bound to this FBO
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, s_iFBOID);
			ASSERT_GL_ERROR;
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, s_iFBO_PixelRenderTarget, 0);
			ASSERT_GL_ERROR;

			// Contents are cleared, and any Draw* call from the framework will hit this texture...
			glViewport(0, 0, s_iInternalWidth, s_iInternalHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		// DrawSprites()
		// DrawTileMaps()
		// etc.

		void FinishOffScreenRender()
		{
			// Bind to the VBO containing the fullscreen quad
			glBindBuffer(GL_ARRAY_BUFFER, s_iFullScreenQuadVBO);
			ASSERT_GL_ERROR;

			// If we're doing the bloom pass....
			if (s_pPostProcessSettings->_bDoBloom)
			{
				// Stage 2:
				{
					// Re-render the low res pixel RT, via a threshold shader, to a new RT, so only "bright" pixels are output
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, s_iFBOID);
					ASSERT_GL_ERROR;
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
					ASSERT_GL_ERROR;
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, s_iFBO_BloomThresholdTexture, 0);
					ASSERT_GL_ERROR;
					glBindTexture(GL_TEXTURE_2D, s_iFBO_PixelRenderTarget);
					ASSERT_GL_ERROR;

					glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					{
						SetOutputThreshold(glm::value_ptr(s_mFinalOutputCameraMatrix));

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

						glDrawArrays(GL_TRIANGLES, 0, 6);
						ASSERT_GL_ERROR;
					}
				}

				// Stage 3:
				{
					// Do a two stage blur pass, once horiz, once vert. 
					ASSERT_GL_ERROR;
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
					ASSERT_GL_ERROR;
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, s_iFBO_BlurHorizTexture, 0);
					ASSERT_GL_ERROR;

					glViewport(0, 0, (GLsizei)s_fBlurWidth, (GLsizei)s_fBlurHeight);
					glBindTexture(GL_TEXTURE_2D, s_iFBO_BloomThresholdTexture);
					ASSERT_GL_ERROR;

					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					// Horizontal
					{
						SetActiveShaderWithMatrix(BLUR_HORIZ, glm::value_ptr(s_mFinalOutputCameraMatrix));

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

						glDrawArrays(GL_TRIANGLES, 0, 6);
						ASSERT_GL_ERROR;
					}

					ASSERT_GL_ERROR;
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
					ASSERT_GL_ERROR;
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, s_iFBO_BlurVertTexture, 0);
					ASSERT_GL_ERROR;


					glBindTexture(GL_TEXTURE_2D, s_iFBO_BlurHorizTexture);
					ASSERT_GL_ERROR;

					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					// Vertical
					{
						SetActiveShaderWithMatrix(BLUR_VERT, glm::value_ptr(s_mFinalOutputCameraMatrix));

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

						glDrawArrays(GL_TRIANGLES, 0, 6);
						ASSERT_GL_ERROR;
					}
				}
			} // End If Bloom. 



			// GNTODO:
			// If the current viewport dimensions aren't a divisor of 1080p, then add an additional step to render to a 1080p  render target, with scanlines
			// or look for a way to hack the parameters passed to the shader so scanlines work at any resolution...

			// Stage 4:
			{
				// Bind to the screen Viewport, remove the binding to the FBO from StartOffScreenRender
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
				ASSERT_GL_ERROR;

				// Set dimensions to the screen output
				glViewport(0, 0, (GLsizei)s_fViewportWidth, (GLsizei)s_fViewportHeight);

				// Bind to the low resolution render texture we initially redirected all the draw calls to at StartOffScreenRender
				glBindTexture(GL_TEXTURE_2D, s_iFBO_PixelRenderTarget);

				// Output scanline or clean version of that to the screen
				{
					if (s_pPostProcessSettings->_bDoScanlines)
						SetOutputScanlines(glm::value_ptr(s_mFinalOutputCameraMatrix));
					else
						SetActiveShaderWithMatrix(DEFAULT_SHADER, glm::value_ptr(s_mFinalOutputCameraMatrix));

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

					glDrawArrays(GL_TRIANGLES, 0, 6);
					ASSERT_GL_ERROR;
				}

				// Output the bloom, if we want it. 
				if (s_pPostProcessSettings->_bDoBloom)
				{
					glBlendFunc(GL_ONE, GL_ONE);
					glBindTexture(GL_TEXTURE_2D, s_iFBO_BlurVertTexture);

					SetOutputBloom(glm::value_ptr(s_mFinalOutputCameraMatrix));

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

					glDrawArrays(GL_TRIANGLES, 0, 6);
					ASSERT_GL_ERROR;
				}
			}


			// Reset clear colour, just in case. 
			SetClearColour(s_vClearColour.x, s_vClearColour.y, s_vClearColour.z, s_vClearColour.w);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 


#ifdef DEBUG 
		// Box2D supports callbacks for drawing the physics world, which are exposed in DEBUG builds. 
		// This allocates the Point, Line and Triangle VBOs used by Box2D_DebugDraw. Called by 
		// AllocateDebugVBOs...
		// Debug builds can render the physics world, but require slightly 
		// custom VBOs and shaders for output. 

		static GLuint s_iBox2DDebugLineVBO;
		static GLuint s_iBox2DDebugPointVBO;
		static GLuint s_iBox2DDebugTriangleVBO;

		static const int s_iSizeOfDebugPoint = sizeof(Box2D_DebugPoint_t);
		static const int s_iSizeOfDebugVertex = sizeof(Box2D_Vertex_t);

		void AllocateB2D_DebugVBOs()
		{
			glGenBuffers(1, &s_iBox2DDebugLineVBO);
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_iBox2DDebugLineVBO);
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfDebugVertex * _iMAX_BOX2D_SPRITES, NULL, GL_DYNAMIC_DRAW);
			ASSERT_GL_ERROR;

			glGenBuffers(1, &s_iBox2DDebugTriangleVBO);
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_iBox2DDebugTriangleVBO);
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfDebugVertex * _iMAX_BOX2D_SPRITES, NULL, GL_DYNAMIC_DRAW);
			ASSERT_GL_ERROR;

			glGenBuffers(1, &s_iBox2DDebugPointVBO);
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_iBox2DDebugPointVBO);
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfDebugPoint * _iMAX_BOX2D_SPRITES, NULL, GL_DYNAMIC_DRAW);
			ASSERT_GL_ERROR;
		}

		void AllocateDebugVBOs()
		{
			// Allocate the set for the debug, untextured sprites, used by some of the editors
			s_pDebugVBOs = NEWX(DynamicVBO_t);
			s_pDebugVBOs->_iVBOCounter = 0;

			glGenBuffers(1, &s_pDebugVBOs->_aVBOs[0]);
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_pDebugVBOs->_aVBOs[0]);
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * _iMAX_SPRITES, NULL, GL_DYNAMIC_DRAW);
			ASSERT_GL_ERROR;

			glGenBuffers(1, &s_pDebugVBOs->_aVBOs[1]);
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_pDebugVBOs->_aVBOs[1]);
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * _iMAX_SPRITES, NULL, GL_DYNAMIC_DRAW);
			ASSERT_GL_ERROR;

			glGenBuffers(1, &s_pDebugVBOs->_aVBOs[2]);
			ASSERT_GL_ERROR;
			glBindBuffer(GL_ARRAY_BUFFER, s_pDebugVBOs->_aVBOs[2]);
			ASSERT_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, s_iSizeOfSprite * _iMAX_SPRITES, NULL, GL_DYNAMIC_DRAW);
			ASSERT_GL_ERROR;

			// Allocate the set used by Box2D's world debug rendering. 
			if(_BOX2D_DEBUG_RENDER) AllocateB2D_DebugVBOs();
		}

		void DeallocateB2D_DebugVBOs()
		{
			glDeleteBuffers(1, &s_iBox2DDebugLineVBO);
			GL_ERROR;
			glDeleteBuffers(1, &s_iBox2DDebugPointVBO);
			GL_ERROR;
			glDeleteBuffers(1, &s_iBox2DDebugTriangleVBO);
			GL_ERROR;
			LOG_INFO("Box2D Debug VBOs Deallocated.");
		}

		void DeallocateDebugVBOs()
		{
			glDeleteBuffers(1, &s_pDebugVBOs->_aVBOs[0]);
			GL_ERROR;
			glDeleteBuffers(1, &s_pDebugVBOs->_aVBOs[1]);
			GL_ERROR;
			glDeleteBuffers(1, &s_pDebugVBOs->_aVBOs[2]);
			GL_ERROR;

			DELETEX s_pDebugVBOs;
			LOG_INFO("Debug VBOs Deallocated.");

			if (_BOX2D_DEBUG_RENDER) DeallocateB2D_DebugVBOs();
		}

		// This is an untextured variant of the Populate VBO function with hardcoded UV to maintain the vertext_t structure in the VBO
		// It's not really worth generalising between the two  as this is only used in the editor modes and not the main RELEASE runtime. 
		void PopulateDebugVBO(const float* pHWidths,
			const float* pHHeights,
			const float* pRots,
			const float* pScales,
			glm::vec4* pColours,
			glm::vec3* pPos,
			const uint32 iCount, bool bIsScaled)
		{
			ASSERT(iCount < _iMAX_SPRITES, "PopulateDebugVBO: Sprite count is greater than VBO limits, something's gone very horribly wrong...");

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

			glBindBuffer(GL_ARRAY_BUFFER, iVBO_ID);
			ASSERT_GL_ERROR;
			Vertex_t* pVertex = (Vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

			// Traverse the sprite arrays
			{
				uint32 iColour;
				float fScaledWidth, fScaledHeight, fPixelWidth, fPixelHeight;

				// We're scaled if we are rendering to the low res FBO, otherwise this is probably an editor mode. 
				if (!bIsScaled)
				{
					fPixelWidth = s_fUnscaledPixelWidth;
					fPixelHeight = s_fUnscaledPixelHeight;
				}
				else
				{
					fPixelWidth = s_fScaledPixelWidth;
					fPixelHeight = s_fScaledPixelHeight;
				}

				// For each sprite up to iCount
				for (uint32 i = 0; i < iCount; i++)
				{
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

					vPos->x = (float)pPos->x * fPixelWidth;
					vPos->y = (float)pPos->y * fPixelHeight;
					vPos->z = (float)pPos->z;
					// Build the vertex positions
					{
						fScaledWidth = (*pHWidths * fPixelWidth);
						fScaledHeight = (*pHHeights * fPixelHeight);

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


		void RenderDebugVBOs(const uint32 iSpriteCount)
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBindBuffer(GL_ARRAY_BUFFER, s_pDebugVBOs->_aVBOs[s_pDebugVBOs->_iVBOCounter]);
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
			if (s_pDebugVBOs->_iVBOCounter == 3) s_pDebugVBOs->_iVBOCounter = 0;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// THESE FUNCTIONS ARE ONLINE NEEDED FOR MAINLINE DEVELOPMENT. SHOULD NOT BE PORTED
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void RenderBox2DWorld(const uint32 iLineCount, const int iPointCount, const int iTriangleCount)
		{
			// Render any debug lines Box2D has sent to the VBO
			{
				SetBox2DShader(BOX2D_LINE);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBindBuffer(GL_ARRAY_BUFFER, s_iBox2DDebugLineVBO);
				ASSERT_GL_ERROR;

				if (iLineCount != 0)
				{
					glVertexAttribPointer(ATTRIB_BOX2D_DEBUG_POSITION, 3, GL_FLOAT, 0, s_iSizeOfDebugVertex, (void*)offsetof(Box2D_Vertex_t, _position));
					ASSERT_GL_ERROR;

					glEnableVertexAttribArray(ATTRIB_BOX2D_DEBUG_POSITION);
					ASSERT_GL_ERROR;

					glVertexAttribPointer(ATTRIB_BOX2D_DEBUG_COLOR, 4, GL_UNSIGNED_BYTE, 1, s_iSizeOfDebugVertex, (void*)offsetof(Box2D_Vertex_t, _colour));
					ASSERT_GL_ERROR;

					glEnableVertexAttribArray(ATTRIB_BOX2D_DEBUG_COLOR);
					ASSERT_GL_ERROR;

					glDrawArrays(GL_LINES, 0, iLineCount);
					ASSERT_GL_ERROR;
				}
			}

			// Render any triangles
			{
				SetBox2DShader(BOX2D_TRIANGLE);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBindBuffer(GL_ARRAY_BUFFER, s_iBox2DDebugTriangleVBO);
				ASSERT_GL_ERROR;

				if (iTriangleCount != 0)
				{
					glVertexAttribPointer(ATTRIB_BOX2D_DEBUG_POSITION, 3, GL_FLOAT, 0, s_iSizeOfDebugVertex, (void*)offsetof(Box2D_Vertex_t, _position));
					ASSERT_GL_ERROR;

					glEnableVertexAttribArray(ATTRIB_BOX2D_DEBUG_POSITION);
					ASSERT_GL_ERROR;

					glVertexAttribPointer(ATTRIB_BOX2D_DEBUG_COLOR, 4, GL_UNSIGNED_BYTE, 1, s_iSizeOfDebugVertex, (void*)offsetof(Box2D_Vertex_t, _colour));
					ASSERT_GL_ERROR;

					glEnableVertexAttribArray(ATTRIB_BOX2D_DEBUG_COLOR);
					ASSERT_GL_ERROR;

					glDrawArrays(GL_TRIANGLES, 0, iTriangleCount);
					ASSERT_GL_ERROR;
				}
			}

			// And finally render the point sprites
			{
				SetBox2DShader(BOX2D_POINT);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBindBuffer(GL_ARRAY_BUFFER, s_iBox2DDebugPointVBO);
				ASSERT_GL_ERROR;

				if (iPointCount != 0)
				{
					glVertexAttribPointer(ATTRIB_BOX2D_DEBUG_POSITION, 3, GL_FLOAT, 0, s_iSizeOfDebugPoint, (void*)offsetof(Box2D_DebugPoint_t, _position));
					ASSERT_GL_ERROR;

					glEnableVertexAttribArray(ATTRIB_BOX2D_DEBUG_POSITION);
					ASSERT_GL_ERROR;

					glVertexAttribPointer(ATTRIB_BOX2D_DEBUG_COLOR, 4, GL_UNSIGNED_BYTE, 1, s_iSizeOfDebugPoint, (void*)offsetof(Box2D_DebugPoint_t, _colour));
					ASSERT_GL_ERROR;

					glEnableVertexAttribArray(ATTRIB_BOX2D_DEBUG_COLOR);
					ASSERT_GL_ERROR;

					glVertexAttribPointer(ATTRIB_BOX2D_DEBUG_SIZE, 1, GL_FLOAT, 1, s_iSizeOfDebugPoint, (void*)offsetof(Box2D_DebugPoint_t, _size));
					ASSERT_GL_ERROR;

					glEnableVertexAttribArray(ATTRIB_BOX2D_DEBUG_SIZE);
					ASSERT_GL_ERROR;

					glEnable(GL_PROGRAM_POINT_SIZE);
					ASSERT_GL_ERROR;
					glDrawArrays(GL_POINTS, 0, iPointCount);
					ASSERT_GL_ERROR;
					glDisable(GL_PROGRAM_POINT_SIZE);
					ASSERT_GL_ERROR;
				}
			}
		}

		void SetBox2DVBOPtrs(GLuint * pLine, GLuint * pPoint, GLuint * pTriangle)
		{
			*pLine = s_iBox2DDebugLineVBO;
			*pPoint = s_iBox2DDebugPointVBO;
			*pTriangle = s_iBox2DDebugTriangleVBO;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
	} // GLUtils
}	// Neutrino
