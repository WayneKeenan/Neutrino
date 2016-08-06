#include "Sprite.h"
#include "Log.h"
#include "Memory.h"
#include "Colour.h"

namespace Neutrino 
{
	SpriteArrayBase_t* sSpriteArrayBase = NULL;
	static Sprite_t* a_sSprite = NULL;
	static uint16 iActiveSpriteCount = 0;


	void AllocateSpriteArrays(uint16_t iSpriteCount)
	{
		iActiveSpriteCount = 0;

		sSpriteArrayBase = NEWX SpriteArrayBase_t;
		LOG_INFO("Allocated %d bytes [%dK] for sprite array base", sizeof(SpriteArrayBase_t), (sizeof(SpriteArrayBase_t) * iSpriteCount) / 1024 );		

		sSpriteArrayBase->a_fHalfWidth = NEWX float[iSpriteCount];
		LOG_INFO("Allocated %d bytes [%dK] for Half Width", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

		sSpriteArrayBase->a_fHalfHeight = NEWX float[iSpriteCount];
		LOG_INFO("Allocated %d bytes [%dK] for Half Width", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

		sSpriteArrayBase->a_fSpriteRotRadians = NEWX float[iSpriteCount];
		LOG_INFO("Allocated %d bytes [%dK] for sprite rotatoins", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

		sSpriteArrayBase->a_fSpriteScale = NEWX float[iSpriteCount];
		LOG_INFO("Allocated %d bytes [%dK] for sprite scale", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

		sSpriteArrayBase->a_u32SprColours = NEWX uint32[iSpriteCount];
		LOG_INFO("Allocated %d bytes [%dK] for sprite colours", sizeof(uint32) * iSpriteCount, (sizeof(uint32) * iSpriteCount) / 1024 );		

		sSpriteArrayBase->a_vSprPositions = NEWX glm::vec3[iSpriteCount];		
		LOG_INFO("Allocated %d bytes [%dK] for sprite positions", sizeof(glm::vec3) * iSpriteCount, (sizeof(glm::vec3) * iSpriteCount) / 1024);

		a_sSprite = NEWX Sprite_t[iSpriteCount];
		LOG_INFO("Allocated %d bytes [%dK] for sprite structs", sizeof(Sprite_t) * iSpriteCount, (sizeof(Sprite_t) * iSpriteCount) / 1024);
	}

	void DeallocateSpriteArrays()
	{
		DELETEX sSpriteArrayBase->a_fHalfWidth;
		DELETEX sSpriteArrayBase->a_fHalfHeight;
		DELETEX sSpriteArrayBase->a_fSpriteScale;
		DELETEX sSpriteArrayBase->a_fSpriteRotRadians;
		DELETEX sSpriteArrayBase->a_u32SprColours;
		DELETEX sSpriteArrayBase->a_vSprPositions;
		DELETEX a_sSprite;
		DELETEX sSpriteArrayBase;
	}


	Sprite_t* GetNextSprite()
	{
		a_sSprite[iActiveSpriteCount]._fHalfWidth = &sSpriteArrayBase->a_fHalfWidth[iActiveSpriteCount];
		a_sSprite[iActiveSpriteCount]._fHalfHeight = &sSpriteArrayBase->a_fHalfHeight[iActiveSpriteCount];
		a_sSprite[iActiveSpriteCount]._fRotRadians = &sSpriteArrayBase->a_fSpriteRotRadians[iActiveSpriteCount];
		a_sSprite[iActiveSpriteCount]._fScale = &sSpriteArrayBase->a_fSpriteScale[iActiveSpriteCount];
		a_sSprite[iActiveSpriteCount]._uPackedColour = &sSpriteArrayBase->a_u32SprColours[iActiveSpriteCount];
		a_sSprite[iActiveSpriteCount]._vPosition = &sSpriteArrayBase->a_vSprPositions[iActiveSpriteCount];

		iActiveSpriteCount++;

		return &a_sSprite[iActiveSpriteCount];
	}


	void ResetSpriteCount()
	{
		iActiveSpriteCount = 0;
	}

	void TestSprite()
	{
		Sprite_t* mySprite = GetNextSprite();
		*mySprite->_uPackedColour = GetPackedColour(1.0f, 1.0f, 0.0f, 1.0f);
	}
}