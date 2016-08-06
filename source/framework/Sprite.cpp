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


	void AllocateSpriteArrays(uint16_t iSpriteCount)
	{
		iActiveSpriteCount = 0;

        a_fHalfWidth = (float*) malloc(sizeof(float)*iSpriteCount);
		LOG_INFO("Allocated %d bytes [%dK] for Half Width", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

        a_fHalfHeight = (float*) malloc(sizeof(float)*iSpriteCount);
		LOG_INFO("Allocated %d bytes [%dK] for Half Width", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

        a_fSpriteRotRadians = (float*) malloc(sizeof(float)*iSpriteCount);
		LOG_INFO("Allocated %d bytes [%dK] for sprite rotatoins", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

        a_fSpriteScale = (float*) malloc(sizeof(float)*iSpriteCount);
		LOG_INFO("Allocated %d bytes [%dK] for sprite scale", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

        a_u32SprColours = (uint32*) malloc(sizeof(uint32)*iSpriteCount);
		LOG_INFO("Allocated %d bytes [%dK] for sprite colours", sizeof(uint32) * iSpriteCount, (sizeof(uint32) * iSpriteCount) / 1024 );		

        a_vSprPositions = (glm::vec3*) malloc(sizeof(glm::vec3)*iSpriteCount);
		LOG_INFO("Allocated %d bytes [%dK] for sprite positions", sizeof(glm::vec3) * iSpriteCount, (sizeof(glm::vec3) * iSpriteCount) / 1024);

        a_sSprite = (Sprite_t*)malloc(sizeof(Sprite_t)*iSpriteCount);
        memset (a_sSprite,'0',sizeof(Sprite_t)*iSpriteCount);
		LOG_INFO("Allocated %d bytes [%dK] for sprite structs", sizeof(Sprite_t) * iSpriteCount, (sizeof(Sprite_t) * iSpriteCount) / 1024);
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


    Sprite_t* GetNextSprite(Sprite_t* pNextSprite)
	{
        pNextSprite = &a_sSprite[iActiveSpriteCount];

        pNextSprite->_fHalfWidth = &a_fHalfWidth[iActiveSpriteCount];
        pNextSprite->_fHalfHeight = &a_fHalfHeight[iActiveSpriteCount];
        pNextSprite->_fRotRadians = &a_fSpriteRotRadians[iActiveSpriteCount];
        pNextSprite->_fScale = &a_fSpriteScale[iActiveSpriteCount];
        pNextSprite->_uPackedColour = &a_u32SprColours[iActiveSpriteCount];
        pNextSprite->_vPosition = &a_vSprPositions[iActiveSpriteCount];

		iActiveSpriteCount++;

        return pNextSprite;
	}


	void ResetSpriteCount()
	{
		iActiveSpriteCount = 0;
	}

	void TestSprite()
	{
        Sprite_t* mySprite;
        GetNextSprite(mySprite);
        *(mySprite->_uPackedColour) = GetPackedColour(1.0f, 1.0f, 0.0f, 1.0f);
	}
}
