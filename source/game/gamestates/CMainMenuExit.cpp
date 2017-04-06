#include "CMainMenuExit.h"
#include "CTestLevelIn.h"
#include "../global/CGameGlobals.h"
#include "../../framework/GameStateManager.h"


CMainMenuExit::CMainMenuExit()
{
	m_pStateName = "Main Menu Exit";
	LOG_INFO(">>>> %s", this->m_pStateName);
}

CMainMenuExit::~CMainMenuExit()
{

}

void CMainMenuExit::Init()
{
}

void CMainMenuExit::Update()
{
	CGameState* pNextGameState = NEWX CTestLevelIn();
	Neutrino::GameStateChange(pNextGameState);
}

void CMainMenuExit::Kill()
{
	CGameGlobals::Instance().SetGameDefaults();
}


