#include "File.h"
#include <sys/stat.h>
#include "Log.h"
#include "Memory.h"
#include "Assert.h"
#include "physfs.h"
#include <string.h>

namespace Neutrino {

	bool FileExists( const char * pFilename )
	{
		struct stat fileBuf;
		return stat(pFilename, &fileBuf) == 0;
	}

	bool ResourceFileExists( const char* pFilename)
	{
		int iExists = PHYSFS_exists(pFilename);
		return (iExists!=0);
	}

	bool MountResources( const char* pFilename )
	{
		PHYSFS_init(NULL);
		PHYSFS_mount( pFilename, NULL, 1);
	
		const char* err = PHYSFS_getLastError();
	
		if ( err != NULL )
		{
			LOG_ERROR("Data file mount error: %s", err );		
			return false;		
		}

		LOG_INFO("Mounted resource bundle: %s", pFilename);
		return true;
	}


	bool UnmountResources( const char* pFilename)
	{
		PHYSFS_removeFromSearchPath( pFilename );
		const char* err = PHYSFS_getLastError();
	
		if ( err != NULL )
		{
			LOG_ERROR("Unable to unmount: %s", err );		
			return false;		
		}

		PHYSFS_deinit();
		LOG_INFO("Unmounted resource bundle: %s", pFilename);
		return true;
	}


	uint32 GetFileSizeBytes( const char * pFilename )
	{
		ASSERT( PHYSFS_exists(pFilename), "Filename doesn't exist in resource bundle: %s", pFilename  );

		PHYSFS_file *pFileHandle = PHYSFS_openRead( pFilename );
		uint32 iLength = (uint32)PHYSFS_fileLength( pFileHandle );
		PHYSFS_close(pFileHandle);

		return iLength;
	}



	const char* CStyleLoadResourceBytes(const char* pFilename)
	{
		if (!FileExists(pFilename))
			return NULL;

		FILE *pFile = fopen(pFilename, "r");
		ASSERT( NULL != pFile, "Unable to load resource: %s", pFilename);

		fseek(pFile, 0, SEEK_END);
		long fSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		char *pResource = NEWX char[fSize + 1];
		memset(pResource, '\0', sizeof(char) * (fSize+1));
		ASSERT( fSize == (long)fread(pResource, fSize, 1, pFile), "Possible read error or unexpected EOF hit during fread of: ", pFilename);

		fclose(pFile);

		//pResource[fSize] = 0;

		return pResource;
	}

	const char* LoadResourceBytes( const char * pFilename) 
	{
		ASSERT( PHYSFS_exists(pFilename), "Filename: %s doesn't exist in resource bundle", pFilename );

		PHYSFS_file *pFileHandle = PHYSFS_openRead( pFilename );
		PHYSFS_sint64 iSize = PHYSFS_fileLength( pFileHandle );
		char* pLoadedResource = NEWX char[ iSize + 1 ];
		memset(pLoadedResource, '\0', sizeof(char)*(iSize+1));

		LOG_INFO("Loading: %s, Bytes: %d", pFilename, iSize);

		int i = (int)PHYSFS_read(pFileHandle, pLoadedResource, (PHYSFS_uint32)iSize, 1);
		PHYSFS_close(pFileHandle);

		if( i < 1)
		{
			LOG_ERROR("PHYSFS file read failed for: %s", pFilename);
			DELETEX [] pLoadedResource;
			pLoadedResource = NULL;
		}

		return pLoadedResource;
	}
}
