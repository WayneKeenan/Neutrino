#include "CGameGlobals.h"
#include "../../framework/Log.h"


void CGameGlobals::SetGameDefaults()
{
	m_iLevelNumber = 0;
	m_iPlayer1Score = 0;
	LOG_INFO("Game Defaults have been set.");
}
