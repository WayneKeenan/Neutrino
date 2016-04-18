#include "File.h"
#include <sys/stat.h>

namespace Neutrino {


	bool FileExists( const char * pFilename )
	{
		struct stat fileBuf;
		return stat(pFilename, &fileBuf) == 0;
	}



	uint32 GetFileByteSize( const char * pFilename )
	{
    	struct stat fileBuf; 

    	if (stat(pFilename, &fileBuf) == 0)
        	return fileBuf.st_size;

    	return 0; 
	}
}