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
		typedef struct Vertex_t
		{
			uint32_t 	_colour; 
			glm::vec2 	_uv;
			glm::vec3 	_position;
		} Vertex_t; 

		static const uint16 s_iMaxSprites = 2048;		// Temporary, need to find a balance of the amount we're copying vs number needed.
		
		extern float OGL_X_RATIO;
		extern float OGL_Y_RATIO;
		extern float DISPLAY_X_SCALE;
		extern float DISPLAY_Y_SCALE;
		extern uint16 INTERNAL_PIXEL_X;
		extern uint16 INTERNAL_PIXEL_Y;

		// Temporary
		void TestRender();

		// SetViewport
		//		Attempts to setup the OGL viewport for given dimensions and calcs the associated matrices
		void SetViewport(const int iScreenWidth, const int iScreenHeight);

		// GenerateMatrices
		// 		Create the new ModelView and Camera Matrices for this tick
		void GenerateMVCMatrices(const int iScreenWidth, const int iScreenHeight);

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
	};
}
