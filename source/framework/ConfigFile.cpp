#include "ConfigFile.h"
#include "Log.h"
#include "File.h"
#include "Assert.h"

namespace Neutrino
{
	static config_t s_Config;
	static bool s_bConfigLoaded = false;
	static const char* s_pConfigFilename = "GameConfig.txt";

	bool LoadConfigFile()
	{
		if( !ResourceFileExists(s_pConfigFilename))
		{
			LOG_ERROR("Unable to find %s, exiting...", s_pConfigFilename);
			return false;	
		}


		const char* pFileBytes = LoadResourceBytes(s_pConfigFilename);
		if( NULL == pFileBytes)
		{
			LOG_ERROR("Unable to read %s, exiting.", s_pConfigFilename);
			return false;					
		}

		config_init(&s_Config);


		if(!config_read_string(&s_Config, pFileBytes))
		{
			const char* pErr =  config_error_text(&s_Config);
			int iErrLn = config_error_line(&s_Config);

			config_destroy(&s_Config);

			LOG_ERROR("Unable to parse game config: \'%s\' line: %d - exiting...", pErr, iErrLn);
			return false;
		}

		LOG_INFO("Game Config File loaded...");
		s_bConfigLoaded = true;

		return s_bConfigLoaded;
	}


	bool GameConfigGetInt( const char* pParam, int* iValueStore )
	{
		ASSERT(s_bConfigLoaded, "Attempting to parse details out of game config file before it is loaded");
		if ( config_lookup_int(&s_Config, pParam, iValueStore) == CONFIG_TRUE )
		{
			//LOG_INFO("Found: %s - %d", pParam, *iValueStore);
			return true;
		}
		else
		{
			LOG_ERROR("GameConfigGetInt failed for param: %s");
			return false;
		}
	}
	
	bool ConfigGetInt( config_t* pCfg, const char* pParam, int* iValueStore )
	{
		if ( config_lookup_int(pCfg, pParam, iValueStore) == CONFIG_TRUE )
		{
			//LOG_INFO("Found: %s - %d", pParam, *iValueStore);
			return true;
		}
		else
		{
			LOG_ERROR("ConfigGetInt failed for param: %s");
			return false;
		}
	}



	const char* GameConfigGetString(const char* pParam)
	{
		ASSERT(s_bConfigLoaded, "Attempting to parse details out of game config file before it is loaded");
		const char* pValueStore;

		if ( config_lookup_string(&s_Config, pParam, &pValueStore) == CONFIG_TRUE )
		{
			//LOG_INFO("Found: %s - %s", pParam, pValueStore);
		}
		else
		{
			LOG_ERROR("GameConfigGetString failed for param: %s");
			pValueStore = NULL;
		}
	
		return pValueStore;
	}


	const char* ConfigGetString(config_t* pCfg, const char* pParam)
	{
		const char* pValueStore;

		if ( config_lookup_string(pCfg, pParam, &pValueStore) == CONFIG_TRUE )
		{
			//LOG_INFO("Found: %s - %s", pParam, pValueStore);
		}
		else
		{
			LOG_ERROR("ConfigGetString failed for param: %s");
			pValueStore = NULL;
		}
	
		return pValueStore;
	}


	const config_setting_t* GameConfigGetList(const char* pParam)
	{
		ASSERT(s_bConfigLoaded, "Attempting to parse details out of game config file before it is loaded");
		config_setting_t* pSetting = config_lookup(&s_Config, pParam);
		if( NULL != pSetting)
		{
			//LOG_INFO("Found: %s", pParam);
			return pSetting;
		}
		else
		{
			LOG_ERROR("GameConfigGetList failed for param: %s", pParam);
			return NULL;
		}
	}

	const config_setting_t* GameConfigGetListElement(const config_setting_t* pList, int iCount)
	{
		config_setting_t* pElem = config_setting_get_elem(pList, iCount);
		if( NULL != pElem )
		{
			return pElem;
		}
		else
		{
			LOG_ERROR("GameConfigGetListElement: failed to get element index %d",iCount);
			return NULL;
		}
	}

	const char* GameConfigGetStringFromSetting(const config_setting_t* pSetting, const char* pParam )
	{
		const char* pValueStore;

		if ( config_setting_lookup_string(pSetting, pParam, &pValueStore) != CONFIG_TRUE )
		{
			LOG_ERROR("GameConfigGetStringFromSetting failed for param: %s");
			pValueStore = NULL;
		}
	
		return pValueStore;
	}
}
