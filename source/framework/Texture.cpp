#include "Texture.h"
#include "sdl/sdl_wrapper.h"
#include "File.h"
#include "Memory.h"
#include "Log.h"
#include "Types.h"
#include "Sprite.h"
#include "IniFile.h"

#if defined _WIN32
#define sprintf sprintf_s
#endif

namespace Neutrino {
	static uint8 s_iLoadedTextureCount = 0;
	static TPage_t* s_aTexturePages;


	const TPageSpriteInfo_t* GetSpriteInfo(const uint16 iTextureSet, const uint16 iSpriteCount)
	{
		ASSERT(iTextureSet < s_iLoadedTextureCount, "GetSpriteInfo number of textures loaded: %d, iTextureSet asked for: %d", s_iLoadedTextureCount, iTextureSet);

		TPage_t* pTpage = &s_aTexturePages[iTextureSet];

		ASSERT(iSpriteCount < pTpage->_iMaxSprites, "GetSpriteInfo called for a Sprite that doesn't exist. Max: %d, iSpriteCount: %d", pTpage->_iMaxSprites, iSpriteCount );

		return &pTpage->aSprintInfo[iSpriteCount];
	}


	uint16 GetTextureSet( const GLuint iTextureID )
	{
		for(uint16 i=0; i < s_iLoadedTextureCount; ++i)
		{
			if (s_aTexturePages[i]._iTextureID == iTextureID)
			{
				return i;
			}
		}
		ASSERT(true, "GetTextureSet: unable to find a loaded texture with this id: %d", iTextureID);
		return 0;
	}


	GLuint GetTextureID(const char* sTextureFilename)
	{
		GLuint iTextureID = iU32_BAD;
		for (int i = 0; i < s_iLoadedTextureCount; ++i)
		{
			if (strcmp(s_aTexturePages[i]._sTextureFilename, sTextureFilename) == 0)
			{
				iTextureID = s_aTexturePages[i]._iTextureID;
				LOG_INFO("Texture ID for %s is: %d", sTextureFilename, iTextureID);
				return iTextureID;
			}
		}

		LOG_ERROR("Unable to find Texture ID for %s!", sTextureFilename);
		return iTextureID;
	}
		
	
	uint8 GetLoadedTextureCount()
	{
		return s_iLoadedTextureCount;
	}


	uint16 GetSpriteCountForTexture(const uint8 iTextureSet)
	{
		ASSERT(iTextureSet < s_iLoadedTextureCount, "GetSpriteCountForTexture iTextureSetIndex out of range!");
		return s_aTexturePages[iTextureSet]._iMaxSprites;
	}


	const TPage_t* GetTPage(const uint8 iTextureSet)
	{
		ASSERT(iTextureSet < s_iLoadedTextureCount, "GetTPage iTextureSetIndex out of range!");
		return &s_aTexturePages[iTextureSet];
	}


	bool LoadTexture( const char* pFilename, const char* pTPageFilename, int iCount )
	{
		if (!ResourceFileExists(pFilename)) { LOG_ERROR("%s does not exist in the resources bundle!", pFilename); return false; }
		if (!ResourceFileExists(pTPageFilename)) { LOG_ERROR("%s does not exist in the resources bundle!", pTPageFilename); return false; }

		//
		// Load the Ini file 
		// 
		void* pIni_t = LoadTPageIni(pTPageFilename);
		if (NULL == pIni_t)
		{
			LOG_ERROR("Loading the TPage info ini file failed: %s", pTPageFilename);
			return false;
		}

		//
		// Store the texture and tpage info filenames, so we can double check the tilemap data on load
		//
		s_aTexturePages[iCount]._sTextureFilename = NEWX char[strlen(pFilename) + 1];
		s_aTexturePages[iCount]._sTPageFilename = NEWX char[strlen(pTPageFilename) + 1];
		memcpy(s_aTexturePages[iCount]._sTextureFilename, pFilename, strlen(pFilename) + 1);
		memcpy(s_aTexturePages[iCount]._sTPageFilename, pTPageFilename, strlen(pTPageFilename) + 1);

		// Get the texture dimensions and sprite count
		int iWidth = GetTPageWidth(pIni_t);
		int iHeight = GetTPageHeight(pIni_t);
		if (iWidth < 0 || iHeight < 0) 
			return false;
		s_aTexturePages[iCount]._iWidth = (uint16)iWidth;
		s_aTexturePages[iCount]._iHeight = (uint16)iHeight;
		int iSprs = GetTPageSpriteCount(pIni_t);

		// Allocate the TPageSpriteInfo array for this texture page and populate with each sprite's info
		s_aTexturePages[iCount].aSprintInfo = NEWX TPageSpriteInfo_t[iSprs];
		s_aTexturePages[iCount]._iMaxSprites = (uint16)(iSprs);
		LOG_INFO("Texture page contains %d sprites", iSprs);


		//
		// Iterate over the sprites and parse their details
		//
		for (int i = 0; i < iSprs; i++)	// -1 as there's always an empty group at the end of the list in tpagex.txt
		{
			int iWidth = -1,iHeight = -1, iX = -1, iY = -1;
			const char* sFilename="";

			if(!GetSpriteDetailsFromTPageIni(pIni_t, i, &iWidth, &iHeight, &iX, &iY, &sFilename))
			{
				LOG_ERROR("Parsing sprite %d failed", i);
				return false;
			}

			// Calculate this sprite's UV coords and dimensions
			{
				// Assume non power of 2 textures
				float fTexelW = 1.0f / (float)s_aTexturePages[iCount]._iWidth;
				float fTexelH = 1.0f / (float)s_aTexturePages[iCount]._iHeight;

				// OpenGL samples from the middle of a texel, so we'll need to add an offset
				//float fTexelMovementW = (fTexelW / 2.0f);
				//float fTexelMovementH = (fTexelH / 2.0f);

				// Store the UV coords
				s_aTexturePages[iCount].aSprintInfo[i]._fX_S = ((float)iX * fTexelW);// -fTexelMovementW;
				s_aTexturePages[iCount].aSprintInfo[i]._fY_T = ((float)iY * fTexelH);// - fTexelMovementH;
				s_aTexturePages[iCount].aSprintInfo[i]._fX_SnS = (s_aTexturePages[iCount].aSprintInfo[i]._fX_S + ((float)iWidth * fTexelW));// + fTexelMovementW;
				s_aTexturePages[iCount].aSprintInfo[i]._fY_TnT = (s_aTexturePages[iCount].aSprintInfo[i]._fY_T + ((float)iHeight * fTexelH));// + fTexelMovementH;

			 // And half dimensions (Quad's origin is always at the centre)
				s_aTexturePages[iCount].aSprintInfo[i]._fHalfWidth = (float)iWidth / 2.0f;
				s_aTexturePages[iCount].aSprintInfo[i]._fHalfHeight = (float)iHeight / 2.0f;
			}

			LOG_INFO("Added \'%s\': %d/%d @ [%d,%d]", sFilename, iWidth, iHeight, iX, iY);
		}


		// 
		// Load the texture binary and setup for OGL
		// 
		bool bLoadStatus = false;
		{
			uint32 iFileSize = GetFileSizeBytes( pFilename );
			ASSERT(iFileSize > 0, "LoadGLTexture: %s zero bytes in size?", pFilename);

			const char* pFileBytes = LoadResourceBytes(pFilename);
			ASSERT(NULL != pFileBytes, "LoadGLTexture: loading file failed: %s", pFilename );

			SDL_RWops* pOps = SDL_RWFromConstMem(pFileBytes, iFileSize);
			ASSERT(pOps, "LoadGLTexture: SDL_RWFromMem failed");

			SDL_Surface* pSurf = IMG_Load_RW(pOps, 1);
			ASSERT(pSurf, "LoadGLTexture: SDL Surface conversion failed");

			bLoadStatus = GLTextureFromSDLSurface(&s_aTexturePages[iCount]._iTextureID, pSurf, false);

			SDL_FreeSurface(pSurf);
			DELETEX [] pFileBytes;		// NEWX in LoadResourceBytes()
		}

		return bLoadStatus;
	}





	bool LoadTexturesFromConfigFile()
	{ 
		// Allocate the TPage_t array
		// TPageSpriteInfo_t arrays inside each of these will need to be allocated after parsing the tpage<n>.txt file
		{
			s_aTexturePages = NEWX TPage_t[_iMAX_TEXTURES];
			s_iLoadedTextureCount = 0;				
		}

		// Iterate over the possible textures
		// 
		{
			int iTextureCount = GetNumTextures();
			for(int i = 0; i<iTextureCount; i++)
			{
				const char* pFilename = GetTextureFilename(i);
				const char* pTPageFilename = GetTPageFilename(i);
				if ( NULL == pFilename || NULL == pTPageFilename )
				{
					LOG_ERROR("Possible error in GameConfig.ini, one of the tpage parameters is null.");
					return false;
				}

				LOG_INFO("Found texture: %s\nINF: Found texture info: %s", pFilename, pTPageFilename);
				if (LoadTexture(pFilename, pTPageFilename, s_iLoadedTextureCount))
				{
					AllocateSpriteArrays(s_aTexturePages[s_iLoadedTextureCount]._iTextureID);
					GLUtils::AllocateDynamicVBOSet();
					++s_iLoadedTextureCount;
				}
				else
				{
					LOG_ERROR("Failed to load %s, exiting...", pFilename);
					return false;
				}
			}
		}


		LOG_INFO("Loaded %d textures.", s_iLoadedTextureCount);
		return true;
	}				

	void DeallocateTextures()
	{
		for (int i =0; i < s_iLoadedTextureCount; i++)
		{
			glDeleteTextures(1, &s_aTexturePages[i]._iTextureID);
			DELETEX [] s_aTexturePages[i].aSprintInfo;
		}

		DELETEX [] s_aTexturePages;
		LOG_INFO("Textures Deallocated.");
	}
}
