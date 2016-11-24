#include "Sprite.h"
#include "Log.h"
#include "Memory.h"
#include "Colour.h"
#include <string.h>
#include <math.h>
#include "Time.h"
#include "Types.h"
#include "Texture.h"


// TODO: Remove this, just for testing the INPUT axis...
#include "Input.h"


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
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afX_S = NEWX float[iMAX_SPRITES];
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afY_T = NEWX float[iMAX_SPRITES];
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afX_SnS = NEWX float[iMAX_SPRITES];
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afY_TnT = NEWX float[iMAX_SPRITES];
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afHalfWidth = NEWX float[iMAX_SPRITES];
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afHalfHeight = NEWX float[iMAX_SPRITES];
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afSpriteRotDegrees = NEWX float[iMAX_SPRITES];
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afSpriteScale = NEWX float[iMAX_SPRITES];
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._avSprColours = NEWX glm::vec4[iMAX_SPRITES];
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._avSprPositions = NEWX glm::vec3[iMAX_SPRITES];
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers = NEWX Sprite_t[iMAX_SPRITES];
		}

		// Setup the pointers in the sprite_t to the correct locations in the arrays. 
		// TO DO: Is this actually useful through the framework? Could easily remove this...
		// 
		for(int i = 0; i < iMAX_SPRITES; i ++)
		{
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._fX_S = &s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afX_S[i];
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._fY_T = &s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afY_T[i];
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._fX_SnS = &s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afX_SnS[i];
			s_aSpriteRenderInfo[s_iAllocatedSets]->_SpriteBasePointers[i]._fY_TnT = &s_aSpriteRenderInfo[s_iAllocatedSets]->_SprArrayBase._afY_TnT[i];
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
			DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._afX_S;
			DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._afY_T;
			DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._afX_SnS;
			DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._afY_TnT;
			DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._afHalfWidth;
			DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._afHalfHeight;
			DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._afSpriteScale;
			DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._afSpriteRotDegrees;
			DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._avSprColours;
			DELETEX [] s_aSpriteRenderInfo[i]->_SprArrayBase._avSprPositions;
			DELETEX [] s_aSpriteRenderInfo[i]->_SpriteBasePointers;			
		}
		LOG_INFO("Sprite Arrays deallocated.");
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
						s_aSpriteRenderInfo[i]->_SpriteBasePointers[0]._fX_S,
						s_aSpriteRenderInfo[i]->_SpriteBasePointers[0]._fY_T,
						s_aSpriteRenderInfo[i]->_SpriteBasePointers[0]._fX_SnS,
						s_aSpriteRenderInfo[i]->_SpriteBasePointers[0]._fY_TnT,
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
	static glm::vec3 vPos = glm::vec3(0.0f,140.0f,0.0f);

	void TestSprite()
	{
		vPos += *GetInputAxisGameDeltaScaled(0) * 100.0f;

		Sprite_t* mySprite = NULL;
		mySprite = GetActiveSprite(s_aSpriteRenderInfo[0]->_iTextureID);
		ASSERT(mySprite, "TestSprite, GetActiveSprite returned NULL");

		const TPageSpriteInfo_t* pSpriteInfo = GetSpriteInfo(0, 0);


		fAngle += 1.0f * GetGameMSDelta();

		*(mySprite->_vColour) = glm::vec4(1.0f, 1.0f, 1.0f, (float)fabs(sin(fAngle*0.75)));
		*(mySprite->_vPosition) = vPos;
		*(mySprite->_fHalfWidth) = pSpriteInfo->_fHalfWidth;
		*(mySprite->_fHalfHeight) = pSpriteInfo->_fHalfHeight;
		*(mySprite->_fScale) = (float)fabs(sin(fAngle))*0.25f;
		*(mySprite->_fRotDegrees) = fAngle;		
		*(mySprite->_fX_S) = pSpriteInfo->_fX_S;
		*(mySprite->_fY_T) = pSpriteInfo->_fY_T;
		*(mySprite->_fX_SnS) = pSpriteInfo->_fX_SnS;
		*(mySprite->_fY_TnT) = pSpriteInfo->_fY_TnT;

		pSpriteInfo = GetSpriteInfo(0, 1);
		mySprite = GetActiveSprite(s_aSpriteRenderInfo[0]->_iTextureID);

		*(mySprite->_vColour) = glm::vec4(1.0f, 1.0f, 1.0f, (float)fabs(sin(fAngle*0.75)));
		*(mySprite->_vPosition) = glm::vec3(50, 50, 1.0f);
		*(mySprite->_fHalfWidth) = pSpriteInfo->_fHalfWidth;
		*(mySprite->_fHalfHeight) = pSpriteInfo->_fHalfHeight;
		*(mySprite->_fScale) = (float)fabs(sin(fAngle))*0.25f;
		*(mySprite->_fRotDegrees) = fAngle * 0.5f;		
		*(mySprite->_fX_S) = pSpriteInfo->_fX_S;
		*(mySprite->_fY_T) = pSpriteInfo->_fY_T;
		*(mySprite->_fX_SnS) = pSpriteInfo->_fX_SnS;
		*(mySprite->_fY_TnT) = pSpriteInfo->_fY_TnT;

		pSpriteInfo = GetSpriteInfo(0, 2);
		mySprite = GetActiveSprite(s_aSpriteRenderInfo[0]->_iTextureID);

		*(mySprite->_vColour) = glm::vec4(1.0f, 1.0f, 1.0f, (float)fabs(sin(fAngle*2)));
		*(mySprite->_vPosition) = glm::vec3(250, 80, 1.0f);
		*(mySprite->_fHalfWidth) = pSpriteInfo->_fHalfWidth;
		*(mySprite->_fHalfHeight) = pSpriteInfo->_fHalfHeight;
		*(mySprite->_fScale) = (float)fabs(sin(fAngle))*0.25f;
		*(mySprite->_fRotDegrees) = fAngle * 1.5f;		
		*(mySprite->_fX_S) = pSpriteInfo->_fX_S;
		*(mySprite->_fY_T) = pSpriteInfo->_fY_T;
		*(mySprite->_fX_SnS) = pSpriteInfo->_fX_SnS;
		*(mySprite->_fY_TnT) = pSpriteInfo->_fY_TnT;


		pSpriteInfo = GetSpriteInfo(0, 3);
		mySprite = GetActiveSprite(s_aSpriteRenderInfo[0]->_iTextureID);

		*(mySprite->_vColour) = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		*(mySprite->_vPosition) = glm::vec3(100, 100, 1.0f);
		*(mySprite->_fHalfWidth) = pSpriteInfo->_fHalfWidth;
		*(mySprite->_fHalfHeight) = pSpriteInfo->_fHalfHeight;
		*(mySprite->_fScale) = (float)fabs(sin(fAngle))*0.25f;
		*(mySprite->_fRotDegrees) = fAngle;		
		*(mySprite->_fX_S) = pSpriteInfo->_fX_S;
		*(mySprite->_fY_T) = pSpriteInfo->_fY_T;
		*(mySprite->_fX_SnS) = pSpriteInfo->_fX_SnS;
		*(mySprite->_fY_TnT) = pSpriteInfo->_fY_TnT;


		pSpriteInfo = GetSpriteInfo(0, 4);
		mySprite = GetActiveSprite(s_aSpriteRenderInfo[0]->_iTextureID);

		*(mySprite->_vColour) = glm::vec4(1.0f, 1.0f, 1.0f, (float)fabs(sin(fAngle)));
		*(mySprite->_vPosition) =  glm::vec3(200, 100, 1.0f);
		*(mySprite->_fHalfWidth) = pSpriteInfo->_fHalfWidth;
		*(mySprite->_fHalfHeight) = pSpriteInfo->_fHalfHeight;
		*(mySprite->_fScale) = (float)fabs(sin(fAngle))*0.5f;
		*(mySprite->_fRotDegrees) = fAngle  * 0.2f;		
		*(mySprite->_fX_S) = pSpriteInfo->_fX_S;
		*(mySprite->_fY_T) = pSpriteInfo->_fY_T;
		*(mySprite->_fX_SnS) = pSpriteInfo->_fX_SnS;
		*(mySprite->_fY_TnT) = pSpriteInfo->_fY_TnT;


		pSpriteInfo = GetSpriteInfo(0, 5);
		mySprite = GetActiveSprite(s_aSpriteRenderInfo[0]->_iTextureID);

		*(mySprite->_vColour) = glm::vec4(1.0f, 1.0f, 1.0f, (float)fabs(sin(fAngle*0.5f)));
		*(mySprite->_vPosition) = glm::vec3(280, 150, 1.0f);
		*(mySprite->_fHalfWidth) = pSpriteInfo->_fHalfWidth;
		*(mySprite->_fHalfHeight) = pSpriteInfo->_fHalfHeight;
		*(mySprite->_fScale) = (float)fabs(sin(fAngle))*0.25f;
		*(mySprite->_fRotDegrees) = fAngle;		
		*(mySprite->_fX_S) = pSpriteInfo->_fX_S;
		*(mySprite->_fY_T) = pSpriteInfo->_fY_T;
		*(mySprite->_fX_SnS) = pSpriteInfo->_fX_SnS;
		*(mySprite->_fY_TnT) = pSpriteInfo->_fY_TnT;
	}
}
