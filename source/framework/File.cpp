#include "File.h"
#include <sys/stat.h>
#include "Log.h"
#include "Memory.h"
#include "Assert.h"

namespace Neutrino {




	bool FileExists( const char * pFilename )
	{
		struct stat fileBuf;
		return stat(pFilename, &fileBuf) == 0;
	}



	uint32 GetFileSizeBytes( const char * pFilename )
	{
    	struct stat fileBuf; 

    	if (stat(pFilename, &fileBuf) == 0)
        	return fileBuf.st_size;

    	return 0; 
	}



	const char* LoadResourceBytes(const char* pFilename)
	{
		if (!FileExists(pFilename))
			return NULL;

		FILE *pFile = fopen(pFilename, "r");
		ASSERT( NULL != pFile, "Unable to load resource: %s", pFilename);

		fseek(pFile, 0, SEEK_END);
		long fSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		char *pResource = NEWX char[fSize + 1];
		fread(pResource, fSize, 1, pFile);
		fclose(pFile);

		pResource[fSize] = 0;

		return pResource;
	}
}