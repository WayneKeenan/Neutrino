#include "CSplashScreenEnter.h"
#include "CSplashScreenIn.h"
#include "../../framework/GameStateManager.h"

CSplashScreenEnter::CSplashScreenEnter()
{
	m_pStateName = "Splash Screen Enter";
	LOG_INFO(">>>> %s", this->m_pStateName);
}

CSplashScreenEnter::~CSplashScreenEnter()
{

}

void CSplashScreenEnter::Init()
{
	// Check for existance of save games
	// Initialise Steam Manager
	// Set Localisation territory
	// Do any pre-flight checks for existence of level data, etc. 
}

void CSplashScreenEnter::Update()
{
	CGameState* pNextGameState = NEWX CSplashScreenIn();
	Neutrino::GameStateChange(pNextGameState);
}

void CSplashScreenEnter::Kill()
{
	// Nothing to do...
}


