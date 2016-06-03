#include "CSplashScreenExit.h"



CSplashScreenExit::CSplashScreenExit()
{
	m_pStateName = "Splash Screen Enter";
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
	// Immediate change of state to splash screen in (animation for company logo)
}

void CSplashScreenExit::Kill()
{
	// Nothing to do...
}
