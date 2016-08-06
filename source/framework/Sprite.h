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


	typedef struct SpriteArrayBase_t
	{
		float* a_fHalfWidth;
		float* a_fHalfHeight;
		float* a_fSpriteRotRadians;
		float* a_fSpriteScale;
		uint32* a_u32SprColours;
		glm::vec3* a_vSprPositions;
	} SpriteArrayBase_t;

	extern SpriteArrayBase_t sSpriteBase;

	void AllocateSpriteArrays(uint16_t iSpriteCount);

	void DeallocateSpriteArrays();

	Sprite_t* GetNextSprite();

	void ResetSpriteCount();

	void TestSprite();
}