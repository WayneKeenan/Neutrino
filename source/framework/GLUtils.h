#pragma once
#include "GL/glew.h"
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include "Types.h"
#include "Assert.h"
#include "Colour.h"

// Macros for glError Wrapper
//
#if defined(DEBUG)
#define GL_ERROR do{GLUtils::LogGlError(__FILE__,__LINE__);}while(0)
#define ASSERT_GL_ERROR do{ASSERT(!GLUtils::LogGlError(__FILE__,__LINE__));}while(0)
#else
#define GL_ERROR
#define ASSERT_GL_ERROR
#endif

namespace Neutrino 
{

	namespace GLUtils 
	{

		// Vertex structure defines content placed in the VBO for rendering. 
		// 
		typedef struct Vertex_t
		{
			uint32_t 	_colour; 
			glm::vec2 	_uv;
			glm::vec3 	_position;
		} Vertex_t; 

		// Temporary, need to find a balance of the amount we're copying vs number needed.
		// 
		static const uint16 s_iMaxSprites = 2048;		
		

		// Temporary
		void TestRender();

		// SetViewport
		//		Attempts to setup the OGL viewport for given dimensions and calcs the associated matrices
		void SetViewport(const int iViewportWidth, const int iViewportHeight, const int iInternalWidth, const int iInternalHeight);

		// GenerateMatrices
		// 		Create the new ModelView and Camera Matrices for this tick
		void GenerateMVCMatrices();

		// LogGlError
		// 	 Wrapper to chcek glError and LOG_ERR to log file in a clean way. 
		// 	 Use the GL_ERROR / ASSERT_GL_ERROR macros to use this in code...
		bool LogGlError(const char *pFile, int iLine);

		// CreateVBO
		//		TO_DO:
		//			Return a struct of 3 VBOs (triple buffering) of s_iMaxSprites in size
		void CreateVBO();


		// GetCameraMatrix
		// 		Returns address of the camera matrix for current viewport
		float* GetCameraMatrix();


		// DeleteVBO
		// 		Safely cleans up any created VBOs, with error checking.
		void DeleteVBO( GLuint iVBO_ID  );


		// GetMaxSpriteCount
		// 		Return maximum number of sprites per VBO
		uint16 GetMaxSpriteCount();

		// RenderSpriteArrays
		// 		Traverses the sprite settings array and builds the VBO for rendering this frame
		void RenderSpriteArrays(float* pHWidths, float* pHHeights, float* pRots, float* pScales, glm::vec4* pColours, glm::vec3* pPos, const int iCount);
	};
}
