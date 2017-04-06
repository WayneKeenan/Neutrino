#include "CMainMenuIn.h"
#include "CMainMenuExit.h"
#include "../../framework/GameStateManager.h"
#include "../../framework/Sprite.h"
#include "../../framework/Input.h"



CMainMenuIn::CMainMenuIn()
{
	m_pStateName = "Main Menu In";
	LOG_INFO(">>>> %s", this->m_pStateName);
}

CMainMenuIn::~CMainMenuIn()
{
}

void CMainMenuIn::Init()
{
}

void CMainMenuIn::Update()
{
//	Neutrino::TestSprite();

//	if( Neutrino::GetInputAction(Neutrino::eInputActions::_P1_ACTION1))
//	{
//		CGameState* pNextGameState = NEWX CMainMenuExit();
//		Neutrino::GameStateChange(pNextGameState);
//	}
	CGameState* pNextGameState = NEWX CMainMenuExit();
	Neutrino::GameStateChange(pNextGameState);
}

void CMainMenuIn::Kill()
{
}


bool CMainMenuIn::ForceKill()
{
	// No clean up to do atm...
	LOG_INFO(">>>> %s, FORCEKILL called!", this->m_pStateName );
	return true;
}
