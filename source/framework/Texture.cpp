#include "Texture.h"
#include "sdl/sdl_wrapper.h"
#include "GLUtils.h"
#include "File.h"
#include "Memory.h"

namespace Neutrino {

	static const int s_iMaxNumTextures = 64;
	static GLuint s_iTextureIDs[s_iMaxNumTextures];
	static uint8 s_iLoadedTextureCount = 0;

	bool LoadTexture( const char* pFilename )
	{
		if (FileExists(pFilename))
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

			SDL_FreeSurface(pSurf);
  			DELETEX [] pFileBytes;

  			return bLoadStatus;
		}

	 	return false;
	}
}