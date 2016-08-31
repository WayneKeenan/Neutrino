#include "Texture.h"
#include "sdl/sdl_wrapper.h"
#include "File.h"
#include "Memory.h"
#include "libconfig.h"
#include "Log.h"

namespace Neutrino {

	static const int s_iMaxNumTextures = 4;
	static uint8 s_iLoadedTextureCount = 0;
	static TPage_t* s_aTexturePages;



	GLuint GetTextureID(int iCount)
	{
		return s_aTexturePages[iCount].iTextureID;
	}



	bool LoadTexture( const char* pFilename, const char* pTPageFilename, int iCount )
	{
		if (!ResourceFileExists(pFilename))
		{
			LOG_ERROR("Unable to find %s, exiting...", pFilename);
			return false;
		}


		if (!ResourceFileExists(pTPageFilename))
		{
			LOG_ERROR("Unable to find %s, exiting...", pTPageFilename);
			return false;
		}


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

  			bLoadStatus = GLTextureFromSDLSurface(&s_aTexturePages[iCount].iTextureID, pSurf, true);

  			if( bLoadStatus )
  				s_iLoadedTextureCount++;

			SDL_FreeSurface(pSurf);
  			DELETEX [] pFileBytes;
		}


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
			s_aTexturePages = NEWX TPage_t[s_iMaxNumTextures];
			LOG_INFO("Allocated %d bytes [%dK] for texture page parameters", sizeof(TPage_t) * s_iMaxNumTextures, (sizeof(TPage_t) * s_iMaxNumTextures) / 1024 );	
			s_iLoadedTextureCount = 0;				
		}


		config_t cfg;
		config_init(&cfg);


		//
		// Parse the config from memory
		// 
		if(! config_read_string(&cfg, pFileBytes)) 
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

						s_iLoadedTextureCount++;
					}
				}										
			}
		}

		config_destroy(&cfg);
		LOG_INFO("Loaded %d textures.", s_iLoadedTextureCount);

		return true;
	}
}