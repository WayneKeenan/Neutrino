#include "Texture.h"
#include "sdl/sdl_wrapper.h"
#include "File.h"
#include "Memory.h"
#include "libconfig.h"
#include "Log.h"

namespace Neutrino {

	static const int s_iMaxNumTextures = 64;
	static GLuint s_iTextureIDs[s_iMaxNumTextures];
	static uint8 s_iLoadedTextureCount = 0;

	bool LoadTexture( const char* pFilename )
	{
		if (ResourceFileExists(pFilename))
		{
			uint32 iFileSize = GetFileSizeBytes( pFilename );
			ASSERT(iFileSize > 0, "LoadGLTexture: %s zero bytes in size?", pFilename);

			const char* pFileBytes = LoadResourceBytes(pFilename);
			ASSERT(NULL != pFileBytes, "LoadGLTexture: loading file failed: %s", pFilename );

			SDL_RWops* pOps = SDL_RWFromConstMem(pFileBytes, iFileSize);
			ASSERT(pOps, "LoadGLTexture: SDL_RWFromMem failed");

  			SDL_Surface* pSurf = IMG_Load_RW(pOps, 1);
  			ASSERT(pSurf, "LoadGLTexture: SDL Surface conversion failed");

  			bool bLoadStatus = GLTextureFromSDLSurface(s_iTextureIDs, s_iLoadedTextureCount, pSurf, true);

  			if( bLoadStatus )
  				s_iLoadedTextureCount++;

			SDL_FreeSurface(pSurf);
  			DELETEX [] pFileBytes;

  			return bLoadStatus;
		}

	 	return false;
	}


	GLuint GetTextureID(int iCount)
	{
		return s_iTextureIDs[iCount];
	}


	bool LoadTexturesFromConfigFile()
	{
		if( ResourceFileExists("GameConfig.txt"))
		{
			const char* pFileBytes = LoadResourceBytes("GameConfig.txt");

			if( NULL == pFileBytes)
			{
				LOG_ERROR("Unable to read GameConfig.txt, exiting.");
				return false;					
			}

			config_t cfg;
			config_init(&cfg);

			// Parse the config from memory
			if(! config_read_string(&cfg, pFileBytes)) 
			{
				const char* pErr =  config_error_text(&cfg);
				config_destroy(&cfg);

				LOG_ERROR("Unable to parse game config: \'%s\' - exiting...", pErr);
				return false;
			}

			// Iterate over the possible textures
			{
				int iTextureCount = 0;
				if (config_lookup_int(&cfg, "textures.count", &iTextureCount))
				{
					for(int i = 0; i<iTextureCount; i++)
					{
						char sID[64]={'\0'};
						const char* pFilename;
						sprintf(sID, "textures.texture%d", i);
						if ( config_lookup_string(&cfg, sID, &pFilename) == CONFIG_TRUE)
						{
							LOG_INFO("Found texture: %s", pFilename);
							if( !LoadTexture(pFilename) )
							{
								LOG_ERROR("Failed to load %s, exiting...", pFilename);
							}
						}
					}										
				}
			}

			config_destroy(&cfg);
		}
		else
		{
			LOG_ERROR("Unable to find GameConfig.txt, exiting...");
			return false;	
		}


		return true;
	}
}