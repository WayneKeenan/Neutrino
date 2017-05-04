#include "File.h"
#include <sys/stat.h>
#include "Log.h"
#include "Memory.h"
#include "Assert.h"
#include "physfs.h"
#include "Types.h"
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

	const TileMapData_t* LoadTileMapData(const char* pFilePathAndName, const bool bFromResourceBundle)
	{
		TileMapData_t* pTileData = NEWX(TileMapData_t);
		int iFilenameLength = 0;

		if(bFromResourceBundle)
		{
		  ASSERT(PHYSFS_exists(pFilePathAndName), "Filename: %s doesn't exist in resource bundle", pFilePathAndName);
		  PHYSFS_file *pFileHandle = PHYSFS_openRead(pFilePathAndName);
			if (NULL == pFileHandle)
			{
				LOG_ERROR("Unable to open file %s from resource bundle!", pFileHandle);
				return NULL;
			}

			// This code will parse the binary files directly from the resource bundle
			//
			PHYSFS_read(pFileHandle, &(pTileData->_fVersion), sizeof(float), 1);
			if (pTileData->_fVersion < s_fTMD_Version)
			{
				LOG_ERROR("Unable to load tilemap, file format versions conflict.");
				PHYSFS_close(pFileHandle);
				return NULL;
			}
			PHYSFS_read(pFileHandle, &iFilenameLength, sizeof(int), 1);
			pTileData->_sTextureFilename = NEWX char[iFilenameLength];
			PHYSFS_sint64 iLength = PHYSFS_read(pFileHandle, pTileData->_sTextureFilename, sizeof(char), iFilenameLength);
			pTileData->_sTextureFilename[iFilenameLength] = '\0';
			if (iLength != iFilenameLength)
			{
				LOG_INFO("%s", PHYSFS_getLastError());
				return NULL;
			}
			PHYSFS_read(pFileHandle, &(pTileData->_fGridSize), sizeof(float), 1);
			PHYSFS_read(pFileHandle, &(pTileData->_iLevelWidth), sizeof(uint16), 1);
			PHYSFS_read(pFileHandle, &(pTileData->_iLevelHeight), sizeof(uint16), 1);
			pTileData->_aTileMap = NEWX int16[pTileData->_iLevelWidth * pTileData->_iLevelHeight];
			iLength = PHYSFS_read(pFileHandle, &(pTileData->_aTileMap[0]), sizeof(int16), pTileData->_iLevelWidth * pTileData->_iLevelHeight);
			if (iLength != pTileData->_iLevelWidth * pTileData->_iLevelHeight)
			{
				LOG_INFO("%s", PHYSFS_getLastError());
				return NULL;
			}

			// This block will load the whole file into a memory buffer. 

// 			char* pBuffer;
// 			PHYSFS_sint64 iLength = PHYSFS_fileLength(pFileHandle);
// 			pBuffer = new char[iLength];
// 			int length_read = PHYSFS_read(pFileHandle, pBuffer, 1, iLength);

			PHYSFS_close(pFileHandle);
		}
		else
		{
#if defined _WIN32
			FILE *pFile;
			fopen_s(&pFile, pFilePathAndName, "r");	
#else	
			FILE* pFile = fopen(pFilePathAndName, "r");
#endif
			if(NULL == pFile)
			{
				LOG_ERROR("LoadTileMapData: unable to open %s", pFilePathAndName);
				return NULL;
			}

			fread(&(pTileData->_fVersion), sizeof(float), 1, pFile);
			if (pTileData->_fVersion < s_fTMD_Version)
			{
				LOG_ERROR("Unable to load tilemap, file format versions conflict.");
				fclose(pFile);
				return NULL;
			}
			fread(&iFilenameLength, sizeof(int), 1, pFile);
			pTileData->_sTextureFilename = NEWX char[iFilenameLength];
			fread(pTileData->_sTextureFilename, sizeof(char), iFilenameLength, pFile);
			pTileData->_sTextureFilename[iFilenameLength] = '\0';
			fread(&(pTileData->_fGridSize), sizeof(float), 1, pFile); 
			fread(&(pTileData->_iLevelWidth), sizeof(uint16), 1, pFile);
			fread(&(pTileData->_iLevelHeight), sizeof(uint16), 1, pFile);
			pTileData->_aTileMap = NEWX int16[pTileData->_iLevelWidth * pTileData->_iLevelHeight];
			fread(&(pTileData->_aTileMap[0]), sizeof(int16), pTileData->_iLevelWidth * pTileData->_iLevelHeight, pFile);
			fclose(pFile);
		}

		return pTileData;
	}


	PostProcessSettings_t* LoadPostProcessSettings(const char* pFilePathAndName, const bool bFromResourceBundle)
	{
		PostProcessSettings_t* pData = NEWX(PostProcessSettings_t);
		float fVersion;

		if (bFromResourceBundle)
		{
			if (!PHYSFS_exists(pFilePathAndName))
			{
				LOG_ERROR("Filename: %s doesn't exist in resource bundle", pFilePathAndName);
				return NULL;
			}

			PHYSFS_file *pFileHandle = PHYSFS_openRead(pFilePathAndName);
			if (NULL == pFileHandle)
			{
				LOG_ERROR("Unable to open file %s from resource bundle!", pFileHandle);
				return NULL;
			}

			// This code will parse the binary files directly from the resource bundle

			PHYSFS_read(pFileHandle, &fVersion, sizeof(float), 1);
			if (fVersion < s_fPPS_Version)
			{
				LOG_ERROR("Unable to load post process settings, file format versions conflict.");
				PHYSFS_close(pFileHandle);
				return NULL;
			}

			PHYSFS_read(pFileHandle, &(pData->_fScanlineDark), sizeof(float), 1);
			PHYSFS_read(pFileHandle, &(pData->_fVScanlineDark), sizeof(float), 1);
			PHYSFS_read(pFileHandle, &(pData->_fPixelBias), sizeof(float), 1);
			PHYSFS_read(pFileHandle, &(pData->_fBrightness), sizeof(float), 1);
			PHYSFS_read(pFileHandle, &(pData->_fContrast), sizeof(float), 1);
			PHYSFS_read(pFileHandle, &(pData->_fAdditiveStrength), sizeof(float), 1);
			PHYSFS_read(pFileHandle, &(pData->_fThresholdBrightScaler), sizeof(float), 1);
			PHYSFS_read(pFileHandle, &(pData->_fThreshold), sizeof(float), 1);
			PHYSFS_read(pFileHandle, &(pData->_fBloomAlpha), sizeof(float), 1);
			PHYSFS_read(pFileHandle, &(pData->_fBloomContrast), sizeof(float), 1);
			PHYSFS_read(pFileHandle, &(pData->_fBloomBright), sizeof(float), 1);
			PHYSFS_read(pFileHandle, &(pData->_bDoScanlines), sizeof(bool), 1);
			PHYSFS_read(pFileHandle, &(pData->_bDoBloom), sizeof(bool), 1);
			PHYSFS_close(pFileHandle);
		}
		else
		{
#if defined _WIN32
			FILE *pFile;
			fopen_s(&pFile, pFilePathAndName, "r");
#else	
			FILE* pFile = fopen(pFilePathAndName, "r");
#endif
			if (NULL == pFile)
			{
				LOG_ERROR("LoadPostProcessSettings: unable to open %s", pFilePathAndName);
				return NULL;
			}

			fread(&fVersion, sizeof(float), 1, pFile);
			if (fVersion < s_fPPS_Version)
			{
				LOG_ERROR("Unable to load post process settings, file format versions conflict.");
				fclose(pFile);
				return NULL;
			}

			fread(&(pData->_fScanlineDark), sizeof(float), 1, pFile);
			fread(&(pData->_fVScanlineDark), sizeof(float), 1, pFile);
			fread(&(pData->_fPixelBias), sizeof(float), 1, pFile);
			fread(&(pData->_fBrightness), sizeof(float), 1, pFile);
			fread(&(pData->_fContrast), sizeof(float), 1, pFile);
			fread(&(pData->_fAdditiveStrength), sizeof(float), 1, pFile);
			fread(&(pData->_fThresholdBrightScaler), sizeof(float), 1, pFile);
			fread(&(pData->_fThreshold), sizeof(float), 1, pFile);
			fread(&(pData->_fBloomAlpha), sizeof(float), 1, pFile);
			fread(&(pData->_fBloomContrast), sizeof(float), 1, pFile);
			fread(&(pData->_fBloomBright), sizeof(float), 1, pFile);
			fread(&(pData->_bDoScanlines), sizeof(bool), 1, pFile);
			fread(&(pData->_bDoBloom), sizeof(bool), 1, pFile);
			fclose(pFile);
		}

		return pData;
	}

	void WriteToFile(const char* const pFilePathAndName, const char* const pBytes, const int iSize)
	{
#if defined _WIN32
		FILE *pFile;
		fopen_s(&pFile, pFilePathAndName, "w");
#else	
		FILE* pFile = fopen(pFilePathAndName, "w");
#endif

		fwrite(pBytes, 1, iSize, pFile);
		fclose(pFile);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 


#if defined DEBUG
	bool SaveTileMapData(const TileMapData_t* pData)
	{
		// Check filepath is sane
		tinydir_dir dir;
		if (tinydir_open(&dir, pData->_sFilepathBuf) == -1)
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
		strcat_s(sFile, iSize, ".ltdi");
#else
		// Create the full path
		char* sFile = (char*)malloc(strlen(pData->_sFilepathBuf) + strlen(pData->_sFilenameBuf) + 7);
		strcpy(sFile, pData->_sFilepathBuf);
		strcat(sFile, "/");
		strcat(sFile, pData->_sFilenameBuf);
		strcat(sFile, ".ltdi\0");
#endif

		// Check if file already exists and warn that we're going to clobber it
		if (FileExists(sFile)) LOG_WARNING("%s already exists, overwriting.", sFile);

		const TileMapData_t Data = *pData;
		int iFilenameLength = (int)strlen(pData->_sTextureFilename);
		Data._sTextureFilename[iFilenameLength] = '\0';

		// Save the data
		// 
#if defined _WIN32
		FILE *pFile;
		fopen_s(&pFile, sFile, "w+");
#else 
		FILE* pFile = fopen(sFile, "w+");
#endif
		if (NULL != pFile)
		{
			fwrite(&Data._fVersion, sizeof(float), 1, pFile);
			fwrite(&iFilenameLength, sizeof(int), 1, pFile);
			fwrite(Data._sTextureFilename, sizeof(char), strlen(Data._sTextureFilename), pFile);
			fwrite(&Data._fGridSize, sizeof(float), 1, pFile);
			fwrite(&Data._iLevelWidth, sizeof(uint16), 1, pFile);
			fwrite(&Data._iLevelHeight, sizeof(uint16), 1, pFile);
			fwrite(&Data._aTileMap[0], sizeof(int16), Data._iLevelWidth * Data._iLevelHeight, pFile);
		}
		fclose(pFile);

		// Check the data saved correctly
		//
		const TileMapData_t* pDataCheck = LoadTileMapData(sFile);
		bool bIntegrityPass = true;
		if (pDataCheck->_fVersion != Data._fVersion) bIntegrityPass = false;
		if (strcmp(pDataCheck->_sTextureFilename, Data._sTextureFilename) != 0) bIntegrityPass = false;
		if (pDataCheck->_fGridSize != Data._fGridSize) bIntegrityPass = false;
		if (pDataCheck->_iLevelWidth != Data._iLevelWidth) bIntegrityPass = false;
		if (pDataCheck->_iLevelHeight != Data._iLevelHeight) bIntegrityPass = false;
		for (int i = 0; i < Data._iLevelWidth * Data._iLevelHeight; ++i)
			if (pDataCheck->_aTileMap[i] != Data._aTileMap[i]) bIntegrityPass = false;

		free(sFile);
		// And we're done
		return bIntegrityPass;
	}





	bool SavePostProcessSettings(const PostProcessSettings_t* pData, const char* pFilepath)
	{
		// Check filepath is sane
		tinydir_dir dir;
		if (tinydir_open(&dir, pFilepath) == -1)
		{
			LOG_ERROR("Post Process Settings output directory is invalid!");
			return false;
		}

#if defined _WIN32
		// Create the full path
		size_t iSize = strlen(pFilepath) + strlen(s_pPostProcessSettingsFilename) + 8;
		char* sFile = (char*)malloc(iSize);
		strcpy_s(sFile, iSize, pFilepath);
		strcat_s(sFile, iSize, "\\");
		strcat_s(sFile, iSize, s_pPostProcessSettingsFilename);
#else
		// Create the full path
		char* sFile = (char*)malloc(strlen(pFilepath) + strlen(s_pPostProcessSettingsFilename) + 7);
		strcpy(sFile, pFilepath);
		strcat(sFile, "/");
		strcat(sFile, s_pPostProcessSettingsFilename);
		strcat(sFile, "\0");
#endif

		// Check if file already exists and warn that we're going to clobber it
		if (FileExists(sFile)) LOG_WARNING("%s already exists, overwriting.", sFile);

#if defined _WIN32
		FILE *pFile;
		fopen_s(&pFile, sFile, "w+");
#else 
		FILE* pFile = fopen(sFile, "w+");
#endif

		const PostProcessSettings_t Data = *pData;
		if (NULL != pFile)
		{
			fwrite(&s_fPPS_Version, sizeof(float), 1, pFile);
			fwrite(&Data._fScanlineDark, sizeof(float), 1, pFile);
			fwrite(&Data._fVScanlineDark, sizeof(float), 1, pFile);
			fwrite(&Data._fPixelBias, sizeof(float), 1, pFile);
			fwrite(&Data._fBrightness, sizeof(float), 1, pFile);
			fwrite(&Data._fContrast, sizeof(float), 1, pFile);
			fwrite(&Data._fAdditiveStrength, sizeof(float), 1, pFile);
			fwrite(&Data._fThresholdBrightScaler, sizeof(float), 1, pFile);
			fwrite(&Data._fThreshold, sizeof(float), 1, pFile);
			fwrite(&Data._fBloomAlpha, sizeof(float), 1, pFile);
			fwrite(&Data._fBloomContrast, sizeof(float), 1, pFile);
			fwrite(&Data._fBloomBright, sizeof(float), 1, pFile);
			fwrite(&Data._bDoScanlines, sizeof(bool), 1, pFile);
			fwrite(&Data._bDoBloom, sizeof(bool), 1, pFile);
		}
		else
		{
			LOG_ERROR("Unable to open file to write the post process settings...");
			return false;
		}
		fclose(pFile);

		// Check the data saved correctly
		//
		const PostProcessSettings_t* pDataCheck = LoadPostProcessSettings(sFile);
		bool bIntegrityPass = true;
		if (pDataCheck->_fScanlineDark != Data._fScanlineDark) bIntegrityPass = false;
		if (pDataCheck->_fVScanlineDark != Data._fVScanlineDark) bIntegrityPass = false;
		if (pDataCheck->_fPixelBias != Data._fPixelBias) bIntegrityPass = false;
		if (pDataCheck->_fBrightness != Data._fBrightness) bIntegrityPass = false;
		if (pDataCheck->_fContrast != Data._fContrast) bIntegrityPass = false;
		if (pDataCheck->_fAdditiveStrength != Data._fAdditiveStrength) bIntegrityPass = false;
		if (pDataCheck->_fThresholdBrightScaler != Data._fThresholdBrightScaler) bIntegrityPass = false;
		if (pDataCheck->_fThreshold != Data._fThreshold) bIntegrityPass = false;
		if (pDataCheck->_fBloomAlpha != Data._fBloomAlpha) bIntegrityPass = false;
		if (pDataCheck->_fBloomContrast != Data._fBloomContrast) bIntegrityPass = false;
		if (pDataCheck->_fBloomBright != Data._fBloomBright) bIntegrityPass = false;
		if (pDataCheck->_bDoScanlines != Data._bDoScanlines) bIntegrityPass = false;
		if (pDataCheck->_bDoBloom != Data._bDoBloom) bIntegrityPass = false;

		free(sFile);
		return bIntegrityPass;
	}

#endif

}
