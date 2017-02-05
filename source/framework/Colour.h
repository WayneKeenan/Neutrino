#pragma once
#include "NeutrinoMath.h"
#include "Types.h"
#include <math.h>
#include <glm/glm.hpp> 

namespace Neutrino
{

	// GetPackedColour()
	// 		Returns colour from RBG (0.0f -> 1.0f range) inputs to AABBGGRR packed int.
	//   	Suitable for the packed VBO vert definition used within the framework.  
	uint32 GetPackedColour(const float fR, const float fG, const float fB, const float fA);

	// GetPackedColourV4()
	// 		Returns colour from glm::vec4 input to AABBGGRR packed int.
	//   	Suitable for the packed VBO vert definition used within the framework.  
	uint32 GetPackedColourV4(const glm::vec4 vColour);
	uint32 GetPackedColourV4(const glm::vec4* pColour);

	// GetPackedColourHSV
	// 		Returns colour from HSV (0.0f -> 1.0f range) inputs to AABBGGRR packed int.
	uint32 GetPackedColourHSV(const float fH, const float fS, const float fV, const float fA);
}