#pragma once
#include "Clamp.h"
#include "Types.h"
#include <math.h>

namespace Neutrino
{

	// GetPackedColour()
	// 		Returns colour from RBG (0.0f -> 1.0f range) inputs to AABBGGRR packed int.
	//   	Suitable for the packed VBO vert definition used within the framework.  
	uint32 GetPackedColour(const float fR, const float fG, const float fB, const float fA);

	// GetPackedColourHSV
	// 		Returns colour from HSV (0.0f -> 1.0f range) inputs to AABBGGRR packed int.
	uint32 GetPackedColourHSV(const float fH, const float fS, const float fV, const float fA);


	// TO_DO:
	// 		
	// 		Colour functions should also operate on glm::vec4
}