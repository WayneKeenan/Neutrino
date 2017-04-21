#include "CGameGlobals.h"
#include "../../framework/Log.h"


void CGameGlobals::SetGameDefaults(const NeutrinoPreferences_t* pPreferences)
{
	// The framework will pass in the NeutrinoPreferences structure during CoreInit
	// further calls to SetGamedefaults won't have access to this pointer, and the 
	// defaults will be set anyway. 
	if(NULL != pPreferences)
	{
		m_iInternalWidth = pPreferences->_iInternalWidth;
		m_iInternalHeight = pPreferences->_iInternalHeight;
		m_fPixelWidth = pPreferences->_InternalPixelWidth;
		m_fPixelHeight = pPreferences->_InternalPixelHeight;
	}

	m_iLevelNumber = 0;
	m_iPlayer1Score = 0;

	LOG_INFO("Game Defaults have been set.");
}
