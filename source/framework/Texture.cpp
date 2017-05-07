#include "Texture.h"
#include "sdl/sdl_wrapper.h"
#include "File.h"
#include "Memory.h"
#include "libconfig.h"
#include "Log.h"
#include "Types.h"
#include "Sprite.h"
#include "ConfigFile.h"
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
		// Parse the Sprite Details file
		// 
		{
			config_t cfg;
			config_init(&cfg);

			const char* pFileBytes = LoadResourceBytes(pTPageFilename);
			ASSERT(NULL != pFileBytes, "LoadTexture: loading tpage info file failed: %s", pTPageFilename );
			if(!config_read_string(&cfg, pFileBytes)) 
			{
				const char* pErr =  config_error_text(&cfg);
				int iErrLn = config_error_line(&cfg);
				config_destroy(&cfg);

				LOG_ERROR("Unable to parse tpage config: \'%s\' line: %d - exiting...", pErr, iErrLn);
				return false;
			}

			if (config_lookup_int(&cfg, "tpage.width", (int*)&s_aTexturePages[iCount]._iWidth) != CONFIG_TRUE)
			{
				LOG_ERROR("Unable to find tpage width, exiting...");
				config_destroy(&cfg);
				return false;	
			}

			if (config_lookup_int(&cfg, "tpage.height", (int*)&s_aTexturePages[iCount]._iHeight) != CONFIG_TRUE)
			{
				LOG_ERROR("Unable to find tpage height, exiting...");
				config_destroy(&cfg);
				return false;	
			}

			// Get the list of sprites in the tpage info file
			config_setting_t* pSprSetting = config_lookup(&cfg, "tpage.sprites");
			if(NULL == pSprSetting)
			{
				LOG_ERROR("Unable to find list of sprites, exiting...");
				config_destroy(&cfg);
				return false;	
			}

			//
			// Store the texture and tpage info filenames, so we can double check the tilemap data on load
			//
			s_aTexturePages[iCount]._sTextureFilename = NEWX char[strlen(pFilename)+1];
			s_aTexturePages[iCount]._sTPageFilename = NEWX char[strlen(pTPageFilename)+1];
			memcpy(s_aTexturePages[iCount]._sTextureFilename, pFilename, strlen(pFilename)+1);
			memcpy(s_aTexturePages[iCount]._sTPageFilename, pTPageFilename, strlen(pTPageFilename)+1);

			//
			// Allocate the TPageSpriteInfo array for this texture page and populate with each sprite's info
			// 
			int iSprs = config_setting_length(pSprSetting);
			s_aTexturePages[iCount].aSprintInfo = NEWX TPageSpriteInfo_t[iSprs]; 
			s_aTexturePages[iCount]._iMaxSprites = (uint16)(iSprs-1);   		   
			LOG_INFO("Texture page contains %d sprites", iSprs-1); 			
			//   	LOG_INFO("Allocated %d bytes [%dK] for sprite definitions", sizeof(TPageSpriteInfo_t) * iSprs, (sizeof(TPageSpriteInfo_t) * iSprs) / 1024 );


			// 
			// Iterate over each sprite in the tpage and parse it's meta data
			//
			for(int i = 0; i < iSprs-1; i++)	// -1 as there's always an empty group at the end of the list in tpagex.txt
			{
				config_setting_t* pSprite = config_setting_get_elem(pSprSetting, i);
				int iWidth, iHeight, iX, iY;
				const char* sFilename;

				if(!(config_setting_lookup_int(pSprite, "x", &iX) &&
							config_setting_lookup_int(pSprite, "y", &iY) &&
							config_setting_lookup_int(pSprite, "width", &iWidth) && 
							config_setting_lookup_int(pSprite, "height", &iHeight) && 
							config_setting_lookup_string(pSprite, "filename", &sFilename)))	
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
					s_aTexturePages[iCount].aSprintInfo[i]._fHalfWidth = (float)iWidth/2.0f;
					s_aTexturePages[iCount].aSprintInfo[i]._fHalfHeight = (float)iHeight/2.0f;
				}

				LOG_INFO("Added \'%s\': %d/%d @ [%d,%d]", sFilename, iWidth, iHeight, iX, iY );
			}

			config_destroy(&cfg);
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
