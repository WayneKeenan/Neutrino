#include "ConfigFile.h"
#include "libconfig.h"
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
			config_destroy(&s_Config);

			LOG_ERROR("Unable to parse game config: \'%s\' - exiting...", pErr);
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
			LOG_INFO("Found: %s - %d", pParam, *iValueStore);
			return true;
		}
		else
		{
			LOG_ERROR("GameConfigGetInt failed: %s");
			return false;
		}
	}


	const char* GameConfigGetString(const char* pParam)
	{
		ASSERT(s_bConfigLoaded, "Attempting to parse details out of game config file before it is loaded");
		const char* pValueStore;

		if ( config_lookup_string(&s_Config, pParam, &pValueStore) == CONFIG_TRUE )
		{
			LOG_INFO("Found: %s - %s", pParam, pValueStore);
		}
		else
		{
			LOG_ERROR("GameConfigGetString failed: %s");
			pValueStore = NULL;
		}
	
		return pValueStore;
	}
}
