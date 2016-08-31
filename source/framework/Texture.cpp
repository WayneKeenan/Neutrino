#include "Texture.h"
#include "sdl/sdl_wrapper.h"
#include "File.h"
#include "Memory.h"
#include "libconfig.h"
#include "Log.h"
#include "Types.h"
#include "Sprite.h"

namespace Neutrino {
	static uint8 s_iLoadedTextureCount = 0;
	static TPage_t* s_aTexturePages;


	GLuint GetTextureID(int iCount)
	{
		return s_aTexturePages[iCount]._iTextureID;
	}

	GLUtils::VBO_t* GetTextureVBO(int iCount)
	{
		return &s_aTexturePages[iCount]._pVBO;
	}


	bool LoadTexture( const char* pFilename, const char* pTPageFilename, int iCount )
	{
		if (!ResourceFileExists(pFilename)) { LOG_ERROR("Unable to find %s, exiting...", pFilename); return false; }
		if (!ResourceFileExists(pTPageFilename)) { LOG_ERROR("Unable to find %s, exiting...", pTPageFilename); return false; }


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
				config_destroy(&cfg);

				LOG_ERROR("Unable to parse tpage config: \'%s\' - exiting...", pErr);
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
  			// Allocate the TPageSpriteInfo array for this texture page and populate with each sprite's info
  			// 
    		int iSprs = config_setting_length(pSprSetting);
    		s_aTexturePages[iCount].aSprintInfo = NEWX TPageSpriteInfo_t[iSprs]; 
    		s_aTexturePages[iCount]._iMaxSprites = (uint16)(iSprs-1);   		   
    		LOG_INFO("Texture page contains %d sprites", iSprs-1); 			
    		LOG_INFO("Allocated %d bytes [%dK] for sprite definitions", sizeof(TPageSpriteInfo_t) * iSprs, (sizeof(TPageSpriteInfo_t) * iSprs) / 1024 );

			for(int i = 0; i < iSprs-1; i++)	// -1 as there's always an empty group at the end of the list in tpagex.txt
			{
  				config_setting_t* pSprite = config_setting_get_elem(pSprSetting, i);
  				int iWidth, iHeight;
  				const char* sFilename;

		      	if(!(config_setting_lookup_int(pSprite, "x", (int*)&s_aTexturePages[iCount].aSprintInfo[i]._iX) &&
		      		 config_setting_lookup_int(pSprite, "y", (int*)&s_aTexturePages[iCount].aSprintInfo[i]._iY) &&
		      		 config_setting_lookup_int(pSprite, "width", &iWidth) && 
		      		 config_setting_lookup_int(pSprite, "height", &iHeight) && 
		      		 config_setting_lookup_string(pSprite, "filename", &sFilename)))	
		      	{
		      		LOG_ERROR("Parsing sprite %d failed", i);
		      		return false;
		      	}

		      	s_aTexturePages[iCount].aSprintInfo[i]._fHalfWidth = (float)iWidth/2.0f;
		      	s_aTexturePages[iCount].aSprintInfo[i]._fHalfHeight = (float)iHeight/2.0f;
		      	LOG_INFO("Added \'%s\': %d/%d @ [%d,%d]", sFilename, iWidth, iHeight, s_aTexturePages[iCount].aSprintInfo[i]._iX, s_aTexturePages[iCount].aSprintInfo[i]._iY );
		    }

		    config_destroy(&cfg);


		    //
		    // Generate the 3 VBOs assigned to this texture
		    //
		    (s_aTexturePages[iCount]._pVBO) = *(NEWX(GLUtils::VBO_t));
		    GLUtils::CreateVBOs(&s_aTexturePages[iCount]._pVBO);
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

  			bLoadStatus = GLTextureFromSDLSurface(&s_aTexturePages[iCount]._iTextureID, pSurf, true);

  			if( bLoadStatus )
  				s_iLoadedTextureCount++;

			SDL_FreeSurface(pSurf);
  			DELETEX [] pFileBytes;
		}



		//
		// Allocate the sprite arrays for this texture...
		// 
		AllocateSpriteArrays(s_aTexturePages[iCount]._iTextureID);
		return bLoadStatus;
	}





	bool LoadTexturesFromConfigFile()
	{
		if( !ResourceFileExists("GameConfig.txt"))
		{
			LOG_ERROR("Unable to find GameConfig.txt, exiting...");
			return false;	
		}


		const char* pFileBytes = LoadResourceBytes("GameConfig.txt");
		if( NULL == pFileBytes)
		{
			LOG_ERROR("Unable to read GameConfig.txt, exiting.");
			return false;					
		}


		// Allocate the TPage_t array
		// TPageSpriteInfo_t arrays inside each of these will need to be allocated after parsing the tpage<n>.txt file
		//  
		{
			s_aTexturePages = NEWX TPage_t[iMAX_TEXTURES];
			LOG_INFO("Allocated %d bytes [%dK] for texture page parameters", sizeof(TPage_t) * iMAX_TEXTURES, (sizeof(TPage_t) * iMAX_TEXTURES) / 1024 );	
			s_iLoadedTextureCount = 0;				
		}


		config_t cfg;
		config_init(&cfg);


		//
		// Parse the config from memory
		// 
		if(!config_read_string(&cfg, pFileBytes)) 
		{
			const char* pErr =  config_error_text(&cfg);
			config_destroy(&cfg);

			LOG_ERROR("Unable to parse game config: \'%s\' - exiting...", pErr);
			return false;
		}


		//
		// Iterate over the possible textures
		// 
		{
			int iTextureCount = 0;
			if (config_lookup_int(&cfg, "textures.count", &iTextureCount))
			{
				for(int i = 0; i<iTextureCount; i++)
				{
					const char* pFilename;
					const char* pTPageFilename;

					char sID[64]={'\0'};
					char sTP[64]={'\0'};
					sprintf(sID, "textures.texture%d", i);
					sprintf(sTP, "textures.tpageinfo%d", i);

					if ( config_lookup_string(&cfg, sID, &pFilename) == CONFIG_TRUE && config_lookup_string(&cfg, sTP, &pTPageFilename) == CONFIG_TRUE)
					{
						LOG_INFO("Found texture: %s\nINF: Found info: %s", pFilename, pTPageFilename);
						if( !LoadTexture(pFilename, pTPageFilename, s_iLoadedTextureCount) )
						{
							LOG_ERROR("Failed to load %s, exiting...", pFilename);
							return false;
						}
					}
				}										
			}
		}

		config_destroy(&cfg);
		LOG_INFO("Loaded %d textures.", s_iLoadedTextureCount);

		return true;
	}




	void DrawTextures()
	{
		for( int i = 0; i < s_iLoadedTextureCount; i++)
		{
				Sprite_t* s_pBPtr = GetBasePointers();

				GLUtils::PopulateVBO(
								s_pBPtr->_fHalfWidth, 
								s_pBPtr->_fHalfHeight, 
								s_pBPtr->_fRotDegrees, 
								s_pBPtr->_fScale, 
								s_pBPtr->_vColour, 
								s_pBPtr->_vPosition, 
								GetSpriteCount(),
								GetTextureVBO(i)
							);

			// Bind and draw the active VBO
			GLUtils::RenderVBO(GetSpriteCount(), GetTextureID(i), GetTextureVBO(i));

		}
		// Populate the VBOs


	}
}