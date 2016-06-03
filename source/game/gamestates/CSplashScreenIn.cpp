#include "CSplashScreenIn.h"



CSplashScreenIn::CSplashScreenIn()
{
	m_pStateName = "Splash Screen Enter";
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
	// Immediate change of state to splash screen in (animation for company logo)
}

void CSplashScreenIn::Kill()
{
	// Nothing to do...
}
