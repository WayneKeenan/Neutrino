#pragma once
#include "../../framework/Log.h"
#include "../../framework/Singleton.h"

class CGameGlobals : public CSingleton<CGameGlobals>
{
	public:
		uint8 m_iLevelNumber = 0;
		uint32 m_iPlayer1Score = 0;

		CGameGlobals() {LOG_INFO("Game Globals created");};
		~CGameGlobals() {LOG_INFO("Game Globals destroyed");};

		void SetGameDefaults();
private:
};
