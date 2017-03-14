#include "File.h"
#include <sys/stat.h>
#include "Log.h"
#include "Memory.h"
#include "Assert.h"
#include "physfs.h"
#include "tinydir/tinydir.h"
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

#if defined _WIN32
		FILE *pFile;
		fopen_s(&pFile, pFilename, "r");
#else
		FILE *pFile = fopen(pFilename, "r");
#endif
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
		char* pLoadedResource = NEWX char[ (int)iSize + 1 ];
		memset(pLoadedResource, '\0', sizeof(char)*((int)iSize+1));

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

#if defined DEBUG
	bool SaveTileMapData(const TileMapData_t* pData)
	{
		// Check filepath is sane
		tinydir_dir dir;
		if(tinydir_open(&dir, pData->_sFilepathBuf) == -1)
		{
			LOG_ERROR("Output directory is invalid!");
			return false;
		}		

	#if defined _WIN32
		// Create the full path
		size_t iSize = strlen(pData->_sFilepathBuf) + strlen(pData->_sFilenameBuf) + 8;
		char* sFile = (char*)malloc(iSize);
		strcpy_s(sFile, iSize, pData->_sFilepathBuf);
		strcat_s(sFile, iSize, "\\");
		strcat_s(sFile, iSize, pData->_sFilenameBuf);
		strcat_s(sFile, iSize, ".ltdi" );
	#else
		// Create the full path
		char* sFile = (char*) malloc (strlen(pData->_sFilepathBuf)+strlen(pData->_sFilenameBuf)+7); 
		strcpy(sFile, pData->_sFilepathBuf);
		strcat(sFile, "/");
		strcat(sFile, pData->_sFilenameBuf);
		strcat(sFile, ".ltdi\0" );
	#endif

		// Check if file already exists and warn that we're going to clobber it
		if( FileExists(sFile)) LOG_WARNING("%s already exists, overwriting.", sFile);

		const TileMapData_t Data = *pData;
		int iFilenameLength = (int)strlen(pData->_sTextureFilename);
	
		// Save the data
		// 
	#if defined _WIN32
		FILE *pFile;
		fopen_s(&pFile, sFile, "w+");	
	#else 
		FILE* pFile = fopen(sFile, "w+");
	#endif
		if(NULL != pFile)
		{
			fwrite(&Data._fVersion, sizeof(float), 1, pFile);
			fwrite(&iFilenameLength, sizeof(int), 1, pFile);
			fwrite(&Data._sTextureFilename, sizeof(char), strlen(Data._sTextureFilename), pFile);
			fwrite(&Data._LevelWidth, sizeof(uint16), 1, pFile);
			fwrite(&Data._LevelHeight, sizeof(uint16), 1, pFile);
			fwrite(&Data._aTileMap[0], sizeof(int16),  Data._LevelWidth * Data._LevelHeight, pFile);
		}
		fclose(pFile);

		// Check the data saved correctly
		//
		const TileMapData_t* pDataCheck = LoadTileMapData(sFile);
		bool bIntegrityPass = true;
		if( pDataCheck->_fVersion != Data._fVersion ) bIntegrityPass = false;
		if( pDataCheck->_sTextureFilename != Data._sTextureFilename ) bIntegrityPass = false;
		if( pDataCheck->_LevelWidth != Data._LevelWidth ) bIntegrityPass = false;
		if( pDataCheck->_LevelHeight != Data._LevelHeight ) bIntegrityPass = false;
		for(int i = 0; i < Data._LevelWidth * Data._LevelHeight; ++i)
			if(pDataCheck->_aTileMap[i] != Data._aTileMap[i]) bIntegrityPass = false;


		free(sFile);
		// And we're done
		return bIntegrityPass;
	}
#endif

	const TileMapData_t* LoadTileMapData(const char* sFilePathAndName, const bool bFromResourceBundle)
	{
		// TODO: Check the length of the file. There should be a minimum filesize that'll indicate a broken file...?
	#if defined _WIN32
		FILE *pFile;
		fopen_s(&pFile, sFilePathAndName, "r");	
	#else	
		FILE* pFile = fopen(sFilePathAndName, "r");
	#endif
		if(NULL == pFile)
		{
			LOG_ERROR("LoadTileMapData: unable to open %s", sFilePathAndName);
			return NULL;
		}

		TileMapData_t* pDataCheck = NEWX(TileMapData_t);
		int iFilenameLength = 0;

		if(bFromResourceBundle)
		{
			// TODO
			return NULL;
		}
		else
		{
			// TODO: check version against that in the header so we know we don't have a conflict'
			fread(&(pDataCheck->_fVersion), sizeof(float), 1, pFile);
			fread(&iFilenameLength, sizeof(int), 1, pFile);
			pDataCheck->_sTextureFilename = NEWX char[iFilenameLength];
			fread(&(pDataCheck->_sTextureFilename), sizeof(char), iFilenameLength, pFile);
			fread(&(pDataCheck->_LevelWidth), sizeof(uint16), 1, pFile);
			fread(&(pDataCheck->_LevelHeight), sizeof(uint16), 1, pFile);
			pDataCheck->_aTileMap = NEWX int16[pDataCheck->_LevelWidth * pDataCheck->_LevelHeight];
			fread(&(pDataCheck->_aTileMap[0]), sizeof(int16), pDataCheck->_LevelWidth * pDataCheck->_LevelHeight, pFile);
			fclose(pFile);
		}

		return pDataCheck;
	}
}
