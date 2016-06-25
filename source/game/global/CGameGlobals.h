#pragma once
#include "../../framework/Log.h"
#include "../../framework/Singleton.h"

class CGameGlobals : public CSingleton<CGameGlobals>
{
	public:
		bool m_bShowDebug = false;


		CGameGlobals() {LOG_INFO("Game Globals created");};
};
