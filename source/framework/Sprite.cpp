#include "Sprite.h"
#include "Log.h"
#include "Memory.h"
#include "Colour.h"
#include <string.h>
#include <math.h>
#include "Time.h"
#include "Types.h"

namespace Neutrino 
{

	// Static allocated arrays for the sprite settings
	static SpriteRenderInfo_t* s_SpriteRenderInfo[iMAX_TEXTURES];

	// Counters
	static uint8 s_iAllocatedSets = 0;


	void AllocateSpriteArrays(GLuint iTextureID)
	{
		ASSERT(s_iAllocatedSets < iMAX_TEXTURES, "Call to AllocateSpriteArrays made when max textures has been reached.");

		s_SpriteRenderInfo[s_iAllocatedSets] = NEWX(SpriteRenderInfo_t);

		s_SpriteRenderInfo[s_iAllocatedSets]->_iActiveSpriteCount = 0;
		s_SpriteRenderInfo[s_iAllocatedSets]->_iTextureID = iTextureID;


		// Allocate the memory for our sprite settings arrays...
		{
			s_SpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afHalfWidth = NEWX float[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for Half Width", sizeof(float) * iMAX_SPRITES, (sizeof(float) * iMAX_SPRITES) / 1024 );		

	        s_SpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afHalfHeight = NEWX float[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for Half Height", sizeof(float) * iMAX_SPRITES, (sizeof(float) * iMAX_SPRITES) / 1024 );		

	        s_SpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afSpriteRotDegrees = NEWX float[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for sprite rotations", sizeof(float) * iMAX_SPRITES, (sizeof(float) * iMAX_SPRITES) / 1024 );		

	        s_SpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afSpriteScale = NEWX float[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for sprite scale", sizeof(float) * iMAX_SPRITES, (sizeof(float) * iMAX_SPRITES) / 1024 );		
					
	        s_SpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._avSprColours = NEWX glm::vec4[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for sprite colours", sizeof(glm::vec4) * iMAX_SPRITES, (sizeof(glm::vec4) * iMAX_SPRITES) / 1024 );		

	        s_SpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._avSprPositions = NEWX glm::vec3[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for sprite positions", sizeof(glm::vec3) * iMAX_SPRITES, (sizeof(glm::vec3) * iMAX_SPRITES) / 1024);

			s_SpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers = NEWX Sprite_t[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for sprite positions", sizeof(Sprite_t) * iMAX_SPRITES, (sizeof(Sprite_t) * iMAX_SPRITES) / 1024);
		}

		// Setup the pointers in the sprite structs to correct locations in the sprite settings arrays. 
		for(int i = 0; i < iMAX_SPRITES; i ++)
		{
	        s_SpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._fHalfWidth = &s_SpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afHalfWidth[i];
	        s_SpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._fHalfHeight = &s_SpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afHalfHeight[i];
	        s_SpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._fRotDegrees = &s_SpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afSpriteRotDegrees[i];
	        s_SpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._fScale = &s_SpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afSpriteScale[i];
	        s_SpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._vColour = &s_SpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._avSprColours[i];
	        s_SpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._vPosition = &s_SpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._avSprPositions[i];
		}

		s_iAllocatedSets++;
	}



	// TO_DO: Need to pass in a texture ID 
	void DeallocateSpriteArrays()
	{
		for(int i = 0; i < s_iAllocatedSets; i ++)
		{
	        DELETEX [] s_SpriteRenderInfo[i]->_SprArrayBase._afHalfWidth;
	        DELETEX [] s_SpriteRenderInfo[i]->_SprArrayBase._afHalfHeight;
	        DELETEX [] s_SpriteRenderInfo[i]->_SprArrayBase._afSpriteScale;
	        DELETEX [] s_SpriteRenderInfo[i]->_SprArrayBase._afSpriteRotDegrees;
	        DELETEX [] s_SpriteRenderInfo[i]->_SprArrayBase._avSprColours;
	        DELETEX [] s_SpriteRenderInfo[i]->_SprArrayBase._avSprPositions;
			DELETEX [] s_SpriteRenderInfo[i]->_SpriteBasePointers;			
		}
	}



	// TO_DO: Need to pass in a texture ID 
	Sprite_t* GetActiveSprite()
	{
		Sprite_t* pRet = NULL;

		if ( s_SpriteRenderInfo[0]->_iActiveSpriteCount < iMAX_SPRITES)
		{
			pRet = &s_SpriteRenderInfo[0]->_SpriteBasePointers[s_SpriteRenderInfo[0]->_iActiveSpriteCount]; 
			s_SpriteRenderInfo[0]->_iActiveSpriteCount++;
		}
		else
		{
			ASSERT(s_SpriteRenderInfo[0]->_iActiveSpriteCount < iMAX_SPRITES, "Sprite count limit reached! Raise maximum number of sprites");
			// Exit cleanly...
		}

		return pRet;
		
	}

	void ResetSpriteCount()
	{
		for(int i = 0; i < s_iAllocatedSets; i ++)
		{
			s_SpriteRenderInfo[i]->_iActiveSpriteCount = 0;
		}
	}

	// TO_DO: This can be removed
	uint16 GetSpriteCount()
	{
		return s_SpriteRenderInfo[0]->_iActiveSpriteCount;
	}

	// TO_DO: This can be removed
	Sprite_t* GetBasePointers()
	{
		return &s_SpriteRenderInfo[0]->_SpriteBasePointers[0];
	}

	// Temp vars
	static float fAngle=0.0f;

	void TestSprite()
	{
		Sprite_t* mySprite = GetActiveSprite();
		ASSERT(mySprite, "TestSprite, GetActiveSprite returned NULL");

		fAngle += 1.0f * GetGameMSDelta();

		glm::vec4* vColour = NEWX glm::vec4(1.0f, 1.0f, 1.0f, fabs(sin(fAngle)));
		glm::vec3* vPos = NEWX glm::vec3(320.0f/2,180.0f/2, 1.0f);

		*(mySprite->_vColour) = *vColour;
		*(mySprite->_vPosition) = *vPos;
		*(mySprite->_fHalfWidth) = 16.0f;
		*(mySprite->_fHalfHeight) = 16.0f;
		*(mySprite->_fScale) = (float)fabs(sin(fAngle)) * 2.0f;
		*(mySprite->_fRotDegrees) = fAngle  * 2.0f;

/*
		LOG_INFO("Active sprite count: %d", iActiveSpriteCount);
		LOG_INFO("x %f", (mySprite->_vColour->x));
		LOG_INFO("y %f", (mySprite->_vColour->y));
		LOG_INFO("z %f", (mySprite->_vColour->z));
		LOG_INFO("w %f", (mySprite->_vColour->w));

		LOG_INFO("Pos: %f, %f, %f", (mySprite->_vPosition->x),(mySprite->_vPosition->y),(mySprite->_vPosition->z));
*/

		DELETEX vColour;
		DELETEX vPos;
	}
}
