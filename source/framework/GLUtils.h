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

		// Vertex structure defines content order for quad data placed in the current VBO. 
		// Colour is a AABBGGRR packed
		// 
		typedef struct Vertex_t
		{
			uint32 	_colour; 
			GLfloat _uv[2];
			GLfloat _position[3];
		} Vertex_t; 

		// VBOs are triple buffered for dynamically drawn sprites, this structure wraps the 
		// assigned VBOs for each texture, and holds the counter for the VBO to be rendered in 
		// a given frame
		// 
		typedef struct DynamicVBO_t
		{
			GLuint _aVBOs[3];
			uint8  _iVBOCounter;
		} DynamicVBO_t;
	
		
		// SetViewport
		//		Attempts to setup the OGL viewport for given dimensions and calcs the associated matrices
		void SetViewport(const int iViewportWidth, const int iViewportHeight);

		// SetDimensions
		//		Stores internal and viewport dimensions, and calculates initial projection matrix for the 
		//		viewport. "Internal" refers to the low res render texture dimensions used. 
		void SetDimensions(const int iViewportWidth, const int iViewportHeight, const int iInternalWidth, const int iInternalHeight);

		// GetViewportDimensions
		//		Returns a vector2 for the width and height of the active viewport
		const glm::vec2 GetViewportDimensions();

		// GetPixelScale
		//		Returns a vector containing the OGL range of a single pixel
		const glm::vec2 GetViewportPixelScale();

		// GetPixelScale
		//		Returns a vector containing the scaled internal range of a single pixel (for the pixel 
		//		size when rendering to the low resolution FBO
		const glm::vec2 GetInternalPixelScale();

		// GenerateMatrices
		// 		Create the new ModelView and Camera Matrices for this tick
		void GenerateMVCMatrices(glm::vec3* vPos);

		// LogGlError
		// 	 Wrapper to chcek glError & LOG_ERR to the logfile in a clean way. 
		// 	 Use the GL_ERROR / ASSERT_GL_ERROR for simplicity
		bool LogGlError(const char *pFile, int iLine);

		bool AllocateFBOs();

		void DeallocateFBOs();

		// CreateDynamicVBOSet
		//		Generates 3 VBO arrays which we iterate over each frame. 
		//		Bit of fluff logic here, as I'm second guessing what the driver is going to do, but
		//		avoiding the case where we're doing a data map&copy into a VBO the driver might still 
		//		be using by trple-buffering. Can be pretty sure we're only copying data into a buffer
		//		that the driver has already freed. 
		//
		//		None of this is user facing, Render* functions have to get active VBO which is always
		//		one that was used two frames ago...
		//
		//		Called by LoadTexturesFromConfigFile()
		void AllocateDynamicVBOSet();

		// DeleteVBO
		// 		Safely cleans up any created VBOs, with error checking.
		void DeallocateDynamicVBOs();

		// CreateTilemapVBO()
		//		Level tilemaps are created once during framework init and stored through the lifetime
		//		of the game. VBOs are GL_STATIC_DRAW.
		//		
		//		TODO: Should this function return a struct or a straight pointer to the VBO?
		uint8 CreateTilemapVBO(const uint32 iTilemapSize);

		// DeallocateTilemapVBOs()
		//		Cleans up any tilemap VBOs that were created by the levels
		void DeallocateTilemapVBOs();

		// GetCameraMatrix
		// 		Returns address of the camera matrix for current viewport
		float* GetCameraMatrix();

		// PopulateVBO
		// 		Traverses the "sprite settings" arrays, and builds the VBO to be rendered this frame
		// TODO: Pass in the basePtr struct, rather than all the pointers. 
		void PopulateVBO(	const float* pX_S,
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
											bool bIsTilemap);

		// RenderVBO
		// 		Bind the current VBO and call GLDrawArrays
		//   	TO_DO: If there's not a lot of shader changes, merge this and Populate VBO above...
		void RenderVBO(const uint32 iSpriteCount, GLuint iTextureID, const int iVBOSet);

		// RenderVBO
		// 		Bind the a Tilemap VBO and call GLDrawArrays
		void RenderTilemapVBO(const uint32 iTilemapSize, GLuint iTextureID, const int iStaticVBO_Index);

		// SetClearColour
		// 		Changes the glClearColor parameter. This will remain the background colour every tick until 
		// 		either the game or editor mode changes it. 
		void SetClearColour(const float fR, const float fG, const float fB, const float fA);

		// ClearBuffers
		//		Clears the colour and depth buffers, effectively clearing the screen. Should be called once
		//		per tick
		void ClearBuffers();

#if defined DEBUG
		// CreateDebugVBOs()
		// 		In DEBUG builds, editor modes are able to output untextured sprites for information 
		//		to the user. These use a separate set of VBOs, that for consistency, are still triple 
		// 		buffered in the same way the main render path is. 
		//
		//		This will be called by the Framework Init
		void AllocateDebugVBOs();

		// DeleteDebugVBO
		// 		Safely cleans up any created VBOs, with error checking.
		void DeallocateDebugVBOs();

		// PopulateDebugVBO
		// 		Debug version of the above. 
		// TODO: pass in the baseptr struct here 
		void PopulateDebugVBO(const float* pHWidths, 
													const float* pHHeights, 
													const float* pRots, 
													const float* pScales, 
													glm::vec4* pColours, 
													glm::vec3* pPos, 
													const uint32 iCount, bool bIsScaled = true);

		// RenderVBO
		// 		Bind the DEBUG VBO and render it. 
		void RenderDebugVBO(const uint32 iSpriteCount);
#endif
	};
}
