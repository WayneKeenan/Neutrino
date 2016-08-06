#include "Sprite.h"
#include "Log.h"
#include "Memory.h"
#include "Colour.h"
#include <string.h>

namespace Neutrino 
{

    static float* a_fHalfWidth;
    static float* a_fHalfHeight;
    static float* a_fSpriteRotRadians;
    static float* a_fSpriteScale;
    static uint32* a_u32SprColours;
    static glm::vec3* a_vSprPositions;

	static Sprite_t* a_sSprite = NULL;
	static uint16 iActiveSpriteCount = 0;

	static uint32 iMaxSpriteCount =0;


	void AllocateSpriteArrays(uint16_t iSpriteCount)
	{
		iActiveSpriteCount = 0;
		iMaxSpriteCount = iSpriteCount;

        a_fHalfWidth = NEWX float[iSpriteCount];
		LOG_INFO("Allocated %d bytes [%dK] for Half Width", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

        a_fHalfHeight = NEWX float[iSpriteCount];
		LOG_INFO("Allocated %d bytes [%dK] for Half Width", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

        a_fSpriteRotRadians = NEWX float[iSpriteCount];
		LOG_INFO("Allocated %d bytes [%dK] for sprite rotatoins", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

        a_fSpriteScale = NEWX float[iSpriteCount];
		LOG_INFO("Allocated %d bytes [%dK] for sprite scale", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

        a_u32SprColours = NEWX uint32[iSpriteCount];
        a_u32SprColours[0] = 256;
		LOG_INFO("Allocated %d bytes [%dK] for sprite colours", sizeof(uint32) * iSpriteCount, (sizeof(uint32) * iSpriteCount) / 1024 );		

        a_vSprPositions = NEWX glm::vec3[iSpriteCount];
		LOG_INFO("Allocated %d bytes [%dK] for sprite positions", sizeof(glm::vec3) * iSpriteCount, (sizeof(glm::vec3) * iSpriteCount) / 1024);

        a_sSprite = NEWX Sprite_t[iSpriteCount];
		LOG_INFO("Allocated %d bytes [%dK] for sprite structs", sizeof(Sprite_t) * iSpriteCount, (sizeof(Sprite_t) * iSpriteCount) / 1024);

		for(int i = 0; i < iSpriteCount; i ++)
		{
	        a_sSprite[i]._fHalfWidth = &a_fHalfWidth[iActiveSpriteCount];
	        a_sSprite[i]._fHalfHeight = &a_fHalfHeight[iActiveSpriteCount];
	        a_sSprite[i]._fRotRadians = &a_fSpriteRotRadians[iActiveSpriteCount];
	        a_sSprite[i]._fScale = &a_fSpriteScale[iActiveSpriteCount];
	        a_sSprite[i]._uPackedColour = &a_u32SprColours[iActiveSpriteCount];
	        a_sSprite[i]._vPosition = &a_vSprPositions[iActiveSpriteCount];
		}
	}

	void DeallocateSpriteArrays()
	{
        DELETEX a_fHalfWidth;
        DELETEX a_fHalfHeight;
        DELETEX a_fSpriteScale;
        DELETEX a_fSpriteRotRadians;
        DELETEX a_u32SprColours;
        DELETEX a_vSprPositions;
		DELETEX a_sSprite;
	}

	Sprite_t* GetActiveSprite()
	{
		Sprite_t* pRet = NULL;

		if ( iActiveSpriteCount < iMaxSpriteCount)
		{
			pRet = &a_sSprite[iActiveSpriteCount]; 
			iActiveSpriteCount++;
		}
		else
		{
			ASSERT(iActiveSpriteCount < iMaxSpriteCount, "Sprite count limit reached! Raise maximum number of sprites");
			// Exit cleanly...
		}

		return pRet;
	}

	void ResetSpriteCount()
	{
		iActiveSpriteCount = 0;
	}

	void TestSprite()
	{
        Sprite_t* mySprite = GetActiveSprite();
        ASSERT(mySprite, "TestSprite, GetActiveSprite returned NULL");

        *(mySprite->_uPackedColour) = GetPackedColour(1.0f, 1.0f, 0.0f, 1.0f);

        LOG_INFO("Active sprite count: %d", iActiveSpriteCount);
        LOG_INFO("Packed colour" FOUR_BYTE_HEX, GetPackedColour(1.0f, 1.0f, 0.0f, 1.0f));
        LOG_INFO("Sprite colour" FOUR_BYTE_HEX, *mySprite->_uPackedColour );
        LOG_INFO("Array color" FOUR_BYTE_HEX, a_u32SprColours[iActiveSpriteCount-1]);
	}
}
