#pragma once
#include "Clamp.h"
#include <math.h>

namespace Neutrino
{

	// GetPackedColour()
	// 		Returns colour from RBG (0.0f -> 1.0f range) inputs to AABBGGRR packed int.
	//   	Suitable for the packed VBO vert definition used within the framework.  
	uint32_t GetPackedColour(float fR, float fG, float fB, float fA=1.0f);

	// GetPackedColourHSV
	// 		Returns colour from HSV (0.0f -> 1.0f range) inputs to AABBGGRR packed int.
	uint32_t GetPackedColourHSV(float fH, float fS, float fV, float fA=1.0f);


	// TO_DO:
	// 
	// 		Colour functions should also operate on glm::vec4
}