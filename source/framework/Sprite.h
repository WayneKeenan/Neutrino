#pragma once
#include "GLUtils.h"
#include "Types.h"

namespace Neutrino
{
	typedef struct Sprite_t
	{
		// Texture ID
		// Sprite Index
		uint32* 	_u32_Colour;
		glm::vec3* 	_v3_Position;
	} Sprite_t;


	void AllocateSpriteArrays(uint16_t iSpriteCount);

	void DeallocateSpriteArrays();
}