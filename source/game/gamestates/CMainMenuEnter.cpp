#include "CMainMenuEnter.h"
#include "CMainMenuIn.h"
#include "../../framework/GameStateManager.h"



CMainMenuEnter::CMainMenuEnter()
{
	m_pStateName = "Main Menu Enter";
	LOG_INFO(">>>> %s", this->m_pStateName);
}

CMainMenuEnter::~CMainMenuEnter()
{

}

void CMainMenuEnter::Init()
{
	Neutrino::GLUtils::SetClearColour( 0.25f, 0.25f, 0.0f, 1.0f );
}

void CMainMenuEnter::Update()
{	
	CGameState* pNextGameState = NEWX CMainMenuIn();
	Neutrino::GameStateChange(pNextGameState);
}

void CMainMenuEnter::Kill()
{
	// Nothing to do...
}
