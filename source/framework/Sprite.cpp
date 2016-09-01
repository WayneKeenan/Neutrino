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
	//
	// Static allocated arrays for the sprite settings
	// 
	static SpriteRenderInfo_t* s_aSpriteRenderInfo[iMAX_TEXTURES];
	static uint8 s_iAllocatedSets = 0;


	void AllocateSpriteArrays(GLuint iTextureID)
	{
		ASSERT(s_iAllocatedSets < iMAX_TEXTURES, "Call to AllocateSpriteArrays made when max textures has been reached.");

		s_aSpriteRenderInfo[s_iAllocatedSets] = NEWX(SpriteRenderInfo_t);

		s_aSpriteRenderInfo[s_iAllocatedSets]->_iActiveSpriteCount = 0;
		s_aSpriteRenderInfo[s_iAllocatedSets]->_iTextureID = iTextureID;


		// Allocate the memory for our sprite settings arrays...
		{
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afHalfWidth = NEWX float[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for Half Width", sizeof(float) * iMAX_SPRITES, (sizeof(float) * iMAX_SPRITES) / 1024 );		

	        s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afHalfHeight = NEWX float[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for Half Height", sizeof(float) * iMAX_SPRITES, (sizeof(float) * iMAX_SPRITES) / 1024 );		

	        s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afSpriteRotDegrees = NEWX float[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for sprite rotations", sizeof(float) * iMAX_SPRITES, (sizeof(float) * iMAX_SPRITES) / 1024 );		

	        s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afSpriteScale = NEWX float[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for sprite scale", sizeof(float) * iMAX_SPRITES, (sizeof(float) * iMAX_SPRITES) / 1024 );		
					
	        s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._avSprColours = NEWX glm::vec4[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for sprite colours", sizeof(glm::vec4) * iMAX_SPRITES, (sizeof(glm::vec4) * iMAX_SPRITES) / 1024 );		

	        s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._avSprPositions = NEWX glm::vec3[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for sprite positions", sizeof(glm::vec3) * iMAX_SPRITES, (sizeof(glm::vec3) * iMAX_SPRITES) / 1024);

			s_aSpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers = NEWX Sprite_t[iMAX_SPRITES];
			LOG_INFO("Allocated %d bytes [%dK] for sprite positions", sizeof(Sprite_t) * iMAX_SPRITES, (sizeof(Sprite_t) * iMAX_SPRITES) / 1024);
		}

		// Setup the pointers in the sprite_t to the correct locations in the arrays. 
		// TO DO: Is this actually useful through the framework? Could easily remove this...
		// 
		for(int i = 0; i < iMAX_SPRITES; i ++)
		{
	        s_aSpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._fHalfWidth = &s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afHalfWidth[i];
	        s_aSpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._fHalfHeight = &s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afHalfHeight[i];
	        s_aSpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._fRotDegrees = &s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afSpriteRotDegrees[i];
	        s_aSpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._fScale = &s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afSpriteScale[i];
	        s_aSpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._vColour = &s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._avSprColours[i];
	        s_aSpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._vPosition = &s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._avSprPositions[i];
		}

		// One set has been created...
		s_iAllocatedSets++;
	}


	void DeallocateSpriteArrays()
	{
		for(int i = 0; i < s_iAllocatedSets; i ++)
		{
	        DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._afHalfWidth;
	        DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._afHalfHeight;
	        DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._afSpriteScale;
	        DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._afSpriteRotDegrees;
	        DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._avSprColours;
	        DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._avSprPositions;
			DELETEX [] s_aSpriteRenderInfo[i]->_SpriteBasePointers;			
		}
	}

	Sprite_t* GetActiveSprite(GLuint iTextureID)
	{
		Sprite_t* pRet = NULL;

		for( int i = 0; i < s_iAllocatedSets; i++)
		{
			if ( s_aSpriteRenderInfo[i]->_iTextureID == iTextureID)
			{
				if ( s_aSpriteRenderInfo[i]->_iActiveSpriteCount < iMAX_SPRITES)
				{
					pRet = &s_aSpriteRenderInfo[i]->_SpriteBasePointers[ s_aSpriteRenderInfo[i]->_iActiveSpriteCount ]; 
					s_aSpriteRenderInfo[i]->_iActiveSpriteCount++;
				}
			}
		}

		ASSERT(NULL != pRet, "GetActiveSprite was unable to find the TextureID, or iMAX_SPRITES limit has been reached...");

		return pRet;		
	}


	void ResetSpriteCount()
	{
		for(int i = 0; i < s_iAllocatedSets; i ++)
		{
			s_aSpriteRenderInfo[i]->_iActiveSpriteCount = 0;
		}
	}


	void DrawSprites()
	{
		for( int i = 0; i < s_iAllocatedSets; i++)
		{
			if( s_aSpriteRenderInfo[i]->_iActiveSpriteCount > 0 )
			{

				GLUtils::PopulateVBO(
								s_aSpriteRenderInfo[i]->_SpriteBasePointers[0]._fHalfWidth, 
								s_aSpriteRenderInfo[i]->_SpriteBasePointers[0]._fHalfHeight, 
								s_aSpriteRenderInfo[i]->_SpriteBasePointers[0]._fRotDegrees, 
								s_aSpriteRenderInfo[i]->_SpriteBasePointers[0]._fScale, 
								s_aSpriteRenderInfo[i]->_SpriteBasePointers[0]._vColour, 
								s_aSpriteRenderInfo[i]->_SpriteBasePointers[0]._vPosition, 
								s_aSpriteRenderInfo[i]->_iActiveSpriteCount,
								i );

				// TO_DO: If we don't end up doing shader swaps in here, then merge these functions...
				GLUtils::RenderVBO(
								s_aSpriteRenderInfo[i]->_iActiveSpriteCount, 
								s_aSpriteRenderInfo[i]->_iTextureID,
								i );
			}
		}
	}

	// Temp vars
	static float fAngle=0.0f;

	void TestSprite()
	{
		Sprite_t* mySprite = NULL;
		glm::vec4* vColour = NEWX glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec3* vPos = NEWX glm::vec3(320.0f,180.0f, 1.0f);


		fAngle += 1.0f * GetGameMSDelta();

		for( int x = 0; x < 320; x+=32)
		{
			for( int y = 0; y < 180; y+=32)
			{
				mySprite = GetActiveSprite(s_aSpriteRenderInfo[0]->_iTextureID);
				ASSERT(mySprite, "TestSprite, GetActiveSprite returned NULL");

				vPos->x = (float)x+16;
				vPos->y = (float)y+16;

				*(mySprite->_vColour) = *vColour;
				*(mySprite->_vPosition) = *vPos;
				*(mySprite->_fHalfWidth) = 8.0f;
				*(mySprite->_fHalfHeight) = 8.0f;
				*(mySprite->_fScale) = (float)fabs(sin(fAngle))*2;
				*(mySprite->_fRotDegrees) = fAngle;		
			}
		}

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
