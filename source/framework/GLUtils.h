#pragma once
#include "GL/glew.h"
#include <glm/mat4x4.hpp>

namespace Neutrino {

	namespace GLUtils {

		void TestRender();
		void SetViewport(const int iScreenWidth, const int iScreenHeight);
		void Term();
		float* GetCameraMatrix();
	};
}
