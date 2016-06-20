#pragma once
#include "GL/glew.h"
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include "Types.h"

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

		// Temporary MAX_SPRITES setting
		static const uint16 s_iMaxSprites = 4096*2;

		typedef struct Vertex_t
		{
			glm::vec4 _colours; 
			glm::vec2 _uv;
			glm::vec3 _position;
		} Vertex_t; 

		static const uint16 s_iSizeOfVertext = sizeof(Vertex_t);


		void TestRender();

		// SetViewport
		//		Attempts to setup the OGL viewport for given dimensions and calcs the associated matrices
		void SetViewport(const int iScreenWidth, const int iScreenHeight);

		void CreateVBO();

		bool LogGlError(const char *pFile, int iLine);

		// GetCameraMatrix
		// 		Returns address of the camera matrix for current viewport
		float* GetCameraMatrix();


		// DeleteVBO
		void DeleteVBO( GLuint iVBO_ID  );
	};
}
