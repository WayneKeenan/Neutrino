#include "Sprite.h"
#include "Log.h"
#include "Memory.h"
#include "Colour.h"
#include <string.h>

namespace Neutrino 
{

	// Static allocated arrays for the sprite settings
    static float* s_afHalfWidth;
    static float* s_afHalfHeight;
    static float* s_afSpriteRotRadians;
    static float* s_afSpriteScale;
    static glm::vec4* s_avSprColours;
    static glm::vec3* s_avSprPositions;

    // Array of struct of pointers into the sprite details arrays
	static Sprite_t* s_Sprites = NULL;

	// Counters
	static uint16 iActiveSpriteCount = 0;
	static uint32 iMaxSpriteCount =0;


	void AllocateSpriteArrays(uint16_t iSpriteCount)
	{
		iActiveSpriteCount = 0;
		iMaxSpriteCount = iSpriteCount;

		// Allocate the memory for our sprite settings arrays...
		{
	        s_afHalfWidth = NEWX float[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for Half Width", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

	        s_afHalfHeight = NEWX float[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for Half Width", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

	        s_afSpriteRotRadians = NEWX float[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for sprite rotations", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		

	        s_afSpriteScale = NEWX float[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for sprite scale", sizeof(float) * iSpriteCount, (sizeof(float) * iSpriteCount) / 1024 );		
					
	        s_avSprColours = NEWX glm::vec4[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for sprite colours", sizeof(glm::vec4) * iSpriteCount, (sizeof(glm::vec4) * iSpriteCount) / 1024 );		

	        s_avSprPositions = NEWX glm::vec3[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for sprite positions", sizeof(glm::vec3) * iSpriteCount, (sizeof(glm::vec3) * iSpriteCount) / 1024);

	        s_Sprites = NEWX Sprite_t[iSpriteCount];
			LOG_INFO("Allocated %d bytes [%dK] for sprite structs", sizeof(Sprite_t) * iSpriteCount, (sizeof(Sprite_t) * iSpriteCount) / 1024);
		}


		// Setup the pointers in the sprite structs to correct locations in the sprite settings arrays. 
		for(int i = 0; i < iSpriteCount; i ++)
		{
	        s_Sprites[i]._fHalfWidth = &s_afHalfWidth[iActiveSpriteCount];
	        s_Sprites[i]._fHalfHeight = &s_afHalfHeight[iActiveSpriteCount];
	        s_Sprites[i]._fRotRadians = &s_afSpriteRotRadians[iActiveSpriteCount];
	        s_Sprites[i]._fScale = &s_afSpriteScale[iActiveSpriteCount];
	        s_Sprites[i]._vColour = &s_avSprColours[iActiveSpriteCount];
	        s_Sprites[i]._vPosition = &s_avSprPositions[iActiveSpriteCount];
		}
	}




	void DeallocateSpriteArrays()
	{
        DELETEX [] s_afHalfWidth;
        DELETEX [] s_afHalfHeight;
        DELETEX [] s_afSpriteScale;
        DELETEX [] s_afSpriteRotRadians;
        DELETEX [] s_avSprColours;
        DELETEX [] s_avSprPositions;
		DELETEX [] s_Sprites;
	}




	Sprite_t* GetActiveSprite()
	{
		Sprite_t* pRet = NULL;

		if ( iActiveSpriteCount < iMaxSpriteCount)
		{
			pRet = &s_Sprites[iActiveSpriteCount]; 
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
