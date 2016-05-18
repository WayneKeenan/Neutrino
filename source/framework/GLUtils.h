#pragma once
#include "GL/glew.h"
#include <glm/mat4x4.hpp>

namespace Neutrino {

	namespace GLUtils {

		void TestRender();

		// SetViewport
		//		Attempts to setup the OGL viewport for given dimensions and calcs the associated matrices
		void SetViewport(const int iScreenWidth, const int iScreenHeight);

		// GetCameraMatrix
		// 		Returns address of the camera matrix for current viewport
		float* GetCameraMatrix();
	};
}
