#include "CSplashScreenIn.h"
#include "CSplashScreenExit.h"
#include "../../framework/GameStateManager.h"


CSplashScreenIn::CSplashScreenIn()
{
	m_pStateName = "Splash Screen In";
	LOG_INFO(">>>> %s", this->m_pStateName);
}

CSplashScreenIn::~CSplashScreenIn()
{

}

void CSplashScreenIn::Init()
{
	// Check for existance of save games
	// Initialise Steam Manager
	// Set Localisation territory
	// Do any pre-flight checks for existence of level data, etc. 
}

void CSplashScreenIn::Update()
{
	CGameState* pNextGameState = NEWX CSplashScreenExit();
	Neutrino::GameStateChange(pNextGameState);
}

void CSplashScreenIn::Kill()
{
	// Nothing to do...
}
