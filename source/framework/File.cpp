#include "File.h"
#include <sys/stat.h>
#include "Log.h"

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



	const char* LoadResource(const char* filename)
	{
		LOG_INFO("Trying to load file:");
		return NULL;
	}
}