#include "CSplashScreenExit.h"
#include "CMainMenuEnter.h"
#include "../../framework/GameStateManager.h"


CSplashScreenExit::CSplashScreenExit()
{
	m_pStateName = "Splash Screen Exit ";
	LOG_INFO(">>>> %s", this->m_pStateName);
}

CSplashScreenExit::~CSplashScreenExit()
{

}

void CSplashScreenExit::Init()
{
	// Check for existance of save games
	// Initialise Steam Manager
	// Set Localisation territory
	// Do any pre-flight checks for existence of level data, etc. 
}

void CSplashScreenExit::Update()
{
	CGameState* pNextGameState = NEWX CMainMenuEnter();
	Neutrino::GameStateChange(pNextGameState);
}

void CSplashScreenExit::Kill()
{
	// Nothing to do...
}
