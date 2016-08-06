#pragma once
#include "GLUtils.h"
#include "Types.h"

namespace Neutrino
{
	typedef struct Sprite_t
	{
		// Texture ID
		// Sprite Index
		float*		_fHalfWidth;
		float* 		_fHalfHeight;
		float*		_fRotRadians;
		float*		_fScale;
		uint32* 	_uPackedColour;
		glm::vec3* 	_vPosition;
	} Sprite_t;

	void AllocateSpriteArrays(uint16_t iSpriteCount);

	void DeallocateSpriteArrays();

	Sprite_t* GetActiveSprite();

	void ResetSpriteCount();

	void TestSprite();
}
