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
	inline uint32 GetPackedColour(const float fR, const float fG, const float fB, const float fA)
	{
		return (uint32)((uint8)(fA * 255.0f) << 24) | ((uint8)(fB * 255.0f) << 16) | ((uint8)(fG * 255.0f) << 8) | (uint8)(fR * 255.0f);
	};

	// GetPackedColourV4()
	// 		Returns colour from glm::vec4 input to AABBGGRR packed int.
	//   	Suitable for the packed VBO vert definition used within the framework.  
	inline uint32 GetPackedColourV4(const glm::vec4 vColour)
	{
		return (uint32)((uint8)(vColour.w * 255.0f) << 24) | ((uint8)(vColour.z * 255.0f) << 16) | ((uint8)(vColour.y * 255.0f) << 8) | (uint8)(vColour.x * 255.0f);
	};

	inline uint32 GetPackedColourV4(const glm::vec4* pColour)
	{
		return (uint32)((uint8)(pColour->w * 255.0f) << 24) | ((uint8)(pColour->z * 255.0f) << 16) | ((uint8)(pColour->y * 255.0f) << 8) | (uint8)(pColour->x * 255.0f);
	};

	// GetPackedColourHSV
	// 		Returns colour from HSV (0.0f -> 1.0f range) inputs to AABBGGRR packed int.
	uint32 GetPackedColourHSV(const float fH, const float fS, const float fV, const float fA);
}





