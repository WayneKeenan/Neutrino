#include "Sprite.h"
#include "Log.h"
#include "Memory.h"
#include "Colour.h"
#include <string.h>

namespace Neutrino 
{

	// Static allocated arrays for the sprite settings
    static float* a_fHalfWidth;
    static float* a_fHalfHeight;
    static float* a_fSpriteRotRadians;
    static float* a_fSpriteScale;
    static glm::vec4* a_vSprColours;
    static glm::vec3* a_vSprPositions;

    // Array of struct of pointers into the sprite details arrays
	static Sprite_t* a_sSprite = NULL;

	// Counters
	static uint16 iActiveSpriteCount = 0;
	static uint32 iMaxSpriteCount =0;


	void AllocateSpriteArrays(uint16_t iSpriteCount)
	{
		iActiveSpriteCount = 0;
		iMaxSpriteCount = iSpriteCount;

		// Allocate the memory for our sprite settings arrays...
		{
	        a_fHalfWidth = NEWX float[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for Half Width", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

	        a_fHalfHeight = NEWX float[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for Half Width", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

	        a_fSpriteRotRadians = NEWX float[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for sprite rotations", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

	        a_fSpriteScale = NEWX float[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for sprite scale", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		
					
	        a_vSprColours = NEWX glm::vec4[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for sprite colours", sizeof(glm::vec4) * iSpriteCount, (sizeof(glm::vec4) * iSpriteCount) / 1024 );		

	        a_vSprPositions = NEWX glm::vec3[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for sprite positions", sizeof(glm::vec3) * iSpriteCount, (sizeof(glm::vec3) * iSpriteCount) / 1024);

	        a_sSprite = NEWX Sprite_t[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for sprite structs", sizeof(Sprite_t) * iSpriteCount, (sizeof(Sprite_t) * iSpriteCount) / 1024);
		}


		// Setup the pointers in the sprite structs to correct locations in the sprite settings arrays. 
		for(int i = 0; i < iSpriteCount; i ++)
		{
	        a_sSprite[i]._fHalfWidth = &a_fHalfWidth[iActiveSpriteCount];
	        a_sSprite[i]._fHalfHeight = &a_fHalfHeight[iActiveSpriteCount];
	        a_sSprite[i]._fRotRadians = &a_fSpriteRotRadians[iActiveSpriteCount];
	        a_sSprite[i]._fScale = &a_fSpriteScale[iActiveSpriteCount];
	        a_sSprite[i]._vColour = &a_vSprColours[iActiveSpriteCount];
	        a_sSprite[i]._vPosition = &a_vSprPositions[iActiveSpriteCount];
		}
	}




	void DeallocateSpriteArrays()
	{
        DELETEX [] a_fHalfWidth;
        DELETEX [] a_fHalfHeight;
        DELETEX [] a_fSpriteScale;
        DELETEX [] a_fSpriteRotRadians;
        DELETEX [] a_vSprColours;
        DELETEX [] a_vSprPositions;
		DELETEX [] a_sSprite;
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

        glm::vec4* vColour = NEWX glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        glm::vec3* vPos = NEWX glm::vec3(320/2, 180/2, 0);

        *(mySprite->_vColour) = *vColour;
        *(mySprite->_vPosition) = *vPos;
        *(mySprite->_fHalfWidth) = 16.0f;
        *(mySprite->_fHalfHeight) = 16.0f;
        *(mySprite->_fScale) = 1.0f;
        *(mySprite->_fRotRadians) = 0.0f;

//      *(mySprite->_uPackedColour) = GetPackedColour(1.0f, 1.0f, 0.0f, 1.0f);

/*
        LOG_INFO("Active sprite count: %d", iActiveSpriteCount);
		LOG_INFO("x %f", (mySprite->_vColour->x));
		LOG_INFO("y %f", (mySprite->_vColour->y));
		LOG_INFO("z %f", (mySprite->_vColour->z));
		LOG_INFO("w %f", (mySprite->_vColour->w));
*/

		DELETEX vColour;
		DELETEX vPos;
	}
}
