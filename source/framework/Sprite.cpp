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
	static UntexturedSpriteRenderInfo_t* s_aUntexturedSpriteRenderInfo = NULL;	// Only used in DEBUG builds
	static uint8 s_iAllocatedSets = 0;


	void AllocateSpriteArrays(const GLuint iTextureID)
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

	Sprite_t* GetActiveSprite(const GLuint iTextureID)
	{
		Sprite_t* pRet = NULL;
		ASSERT(s_aUntexturedSpriteRenderInfo->_iActiveSpriteCount < iMAX_SPRITES, "GetActiveSprite iMAX_SPRITES limit reached");
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

		if(NULL == pRet) LOG_ERROR("GetActiveSprite was unable to find the TextureID %d, or sprite limit reached for texture: %d", iTextureID);
		return pRet;		
	}

	Sprite_t* NewSprite(const GLuint iTextureID, const uint16 iSprIndex)
	{
		// Try and get the next available sprite or bail out
		Sprite_t* pSprite = GetActiveSprite(iTextureID);
		if( NULL == pSprite)
			return NULL;

		// Try and populate it from the tpage info
		{
			const TPageSpriteInfo_t* pSpriteInfo = GetSpriteInfo(GetTextureSet(iTextureID), iSprIndex);
			*(pSprite->_vColour) = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			*(pSprite->_vPosition) = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			*(pSprite->_fHalfWidth) = pSpriteInfo->_fHalfWidth;
			*(pSprite->_fHalfHeight) = pSpriteInfo->_fHalfHeight;
			*(pSprite->_fScale) = 1.0f;
			*(pSprite->_fRotDegrees) = 0.0f;		
			*(pSprite->_fX_S) = pSpriteInfo->_fX_S;
			*(pSprite->_fY_T) = pSpriteInfo->_fY_T;
			*(pSprite->_fX_SnS) = pSpriteInfo->_fX_SnS;
			*(pSprite->_fY_TnT) = pSpriteInfo->_fY_TnT;
		}

		return pSprite;
	}

	void ResetSpriteCount()
	{
		for(int i = 0; i < s_iAllocatedSets; i ++)
		{
			s_aSpriteRenderInfo[i]->_iActiveSpriteCount = 0;
		}
	#if defined DEBUG
		s_aUntexturedSpriteRenderInfo->_iActiveSpriteCount = 0;
	#endif
	}


	void DrawSprites()
	{
		// Render the main VBOs if we have any active sprites in any of the sets allocated per texture
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
			}				
			
			GLUtils::RenderVBO(
					s_aSpriteRenderInfo[i]->_iActiveSpriteCount, 
					s_aSpriteRenderInfo[i]->_iTextureID,
					i );
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
		*(mySprite->_fScale) = (float)fabs(sin(fAngle));
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
		*(mySprite->_fScale) = (float)fabs(sin(fAngle));
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
		*(mySprite->_fScale) = (float)fabs(sin(fAngle));
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
		*(mySprite->_fScale) = (float)fabs(sin(fAngle));
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
		*(mySprite->_fScale) = (float)fabs(sin(fAngle));
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
		*(mySprite->_fScale) = (float)fabs(sin(fAngle));
		*(mySprite->_fRotDegrees) = fAngle;		
		*(mySprite->_fX_S) = pSpriteInfo->_fX_S;
		*(mySprite->_fY_T) = pSpriteInfo->_fY_T;
		*(mySprite->_fX_SnS) = pSpriteInfo->_fX_SnS;
		*(mySprite->_fY_TnT) = pSpriteInfo->_fY_TnT;
	}


#if defined DEBUG
	void AllocateUntexturedSpriteArrays()
	{
		ASSERT(s_iAllocatedSets < iMAX_TEXTURES, "Call to AllocateSpriteArrays made when max textures has been reached.");
		s_aUntexturedSpriteRenderInfo = NEWX(UntexturedSpriteRenderInfo_t);
		s_aUntexturedSpriteRenderInfo->_iActiveSpriteCount = 0;

		{
			s_aUntexturedSpriteRenderInfo->_SprArrayBase._afHalfWidth = NEWX float[iMAX_SPRITES];
			s_aUntexturedSpriteRenderInfo->_SprArrayBase._afHalfHeight = NEWX float[iMAX_SPRITES];
			s_aUntexturedSpriteRenderInfo->_SprArrayBase._afSpriteRotDegrees = NEWX float[iMAX_SPRITES];
			s_aUntexturedSpriteRenderInfo->_SprArrayBase._afSpriteScale = NEWX float[iMAX_SPRITES];
			s_aUntexturedSpriteRenderInfo->_SprArrayBase._avSprColours = NEWX glm::vec4[iMAX_SPRITES];
			s_aUntexturedSpriteRenderInfo->_SprArrayBase._avSprPositions = NEWX glm::vec3[iMAX_SPRITES];
			s_aUntexturedSpriteRenderInfo->_SpriteBasePointers = NEWX UntexturedSprite_t[iMAX_SPRITES];
		}

		for (int i = 0; i < iMAX_SPRITES; i++)
		{
			s_aUntexturedSpriteRenderInfo->_SpriteBasePointers[i]._fHalfWidth = &s_aUntexturedSpriteRenderInfo->_SprArrayBase._afHalfWidth[i];
			s_aUntexturedSpriteRenderInfo->_SpriteBasePointers[i]._fHalfHeight = &s_aUntexturedSpriteRenderInfo->_SprArrayBase._afHalfHeight[i];
			s_aUntexturedSpriteRenderInfo->_SpriteBasePointers[i]._fRotDegrees = &s_aUntexturedSpriteRenderInfo->_SprArrayBase._afSpriteRotDegrees[i];
			s_aUntexturedSpriteRenderInfo->_SpriteBasePointers[i]._fScale = &s_aUntexturedSpriteRenderInfo->_SprArrayBase._afSpriteScale[i];
			s_aUntexturedSpriteRenderInfo->_SpriteBasePointers[i]._vColour = &s_aUntexturedSpriteRenderInfo->_SprArrayBase._avSprColours[i];
			s_aUntexturedSpriteRenderInfo->_SpriteBasePointers[i]._vPosition = &s_aUntexturedSpriteRenderInfo->_SprArrayBase._avSprPositions[i];
		}
	}


	void DeallocateUntexturedSpriteArrays()
	{
		DELETEX s_aUntexturedSpriteRenderInfo->_SprArrayBase._afHalfWidth;
		DELETEX s_aUntexturedSpriteRenderInfo->_SprArrayBase._afHalfHeight;
		DELETEX s_aUntexturedSpriteRenderInfo->_SprArrayBase._afSpriteScale;
		DELETEX s_aUntexturedSpriteRenderInfo->_SprArrayBase._afSpriteRotDegrees;
		DELETEX s_aUntexturedSpriteRenderInfo->_SprArrayBase._avSprColours;
		DELETEX s_aUntexturedSpriteRenderInfo->_SprArrayBase._avSprPositions;
		DELETEX s_aUntexturedSpriteRenderInfo->_SpriteBasePointers;
		LOG_INFO("Untextured Sprite Arrays deallocated.");
	}


	UntexturedSprite_t* GetActiveDebugSprite()
	{
		UntexturedSprite_t* pRet = NULL;
		ASSERT(s_aUntexturedSpriteRenderInfo->_iActiveSpriteCount < iMAX_SPRITES, "GetActiveDebugSprite iMAX_SPRITES limit reached");
		if ( s_aUntexturedSpriteRenderInfo->_iActiveSpriteCount < iMAX_SPRITES)
		{
			pRet = &s_aUntexturedSpriteRenderInfo->_SpriteBasePointers[ s_aUntexturedSpriteRenderInfo->_iActiveSpriteCount ]; 
			s_aUntexturedSpriteRenderInfo->_iActiveSpriteCount++;
		}

		if(NULL == pRet) LOG_ERROR("GetActiveDebugSprite was unable to find the TextureID %d, or sprite limit reached");
		return pRet;		
	}
		

	UntexturedSprite_t* NewDebugSprite()
	{
		// Try and get the next available sprite or bail out
		UntexturedSprite_t* pSprite = GetActiveDebugSprite();
		if( NULL == pSprite)
			return NULL;

		// Populate defaults
		// TODO: Delete this after you've tested it. '
		{
			*(pSprite->_vColour) = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			*(pSprite->_vPosition) = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			*(pSprite->_fHalfWidth) = 32.0f;
			*(pSprite->_fHalfHeight) = 32.0f;
			*(pSprite->_fScale) = 1.0f;
			*(pSprite->_fRotDegrees) = 0.0f;		
		}

		return pSprite;
	}

	void DrawDebugSprites()
	{
		// If there are Untextured DEBUG sprites, then pass this on to GLUtils and Render the DEBUG VBO
		if (s_aUntexturedSpriteRenderInfo->_iActiveSpriteCount > 0)
		{
			GLUtils::PopulateDebugVBO(s_aUntexturedSpriteRenderInfo->_SpriteBasePointers[0]._fHalfWidth,
				s_aUntexturedSpriteRenderInfo->_SpriteBasePointers[0]._fHalfHeight,
				s_aUntexturedSpriteRenderInfo->_SpriteBasePointers[0]._fRotDegrees,
				s_aUntexturedSpriteRenderInfo->_SpriteBasePointers[0]._fScale,
				s_aUntexturedSpriteRenderInfo->_SpriteBasePointers[0]._vColour,
				s_aUntexturedSpriteRenderInfo->_SpriteBasePointers[0]._vPosition,
				s_aUntexturedSpriteRenderInfo->_iActiveSpriteCount);

			GLUtils::RenderDebugVBO(s_aUntexturedSpriteRenderInfo->_iActiveSpriteCount);
		}
	}
#endif


}
