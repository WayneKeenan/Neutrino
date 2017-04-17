#include "CTestLevelIn.h"
#include "../../framework/GameStateManager.h"


CTestLevelIn::CTestLevelIn()
{
	m_pStateName = "Test Level In";
	LOG_INFO(">>>> %s", this->m_pStateName);
}

CTestLevelIn::~CTestLevelIn()
{

}

void CTestLevelIn::Init()
{
	Neutrino::GLUtils::SetClearColour(0.0f, 0.0f, 0.1f, 1.0f);
}

void CTestLevelIn::Update()
{
//	CGameState* pNextGameState = NEWX CSplashScreenExit();
//	Neutrino::GameStateChange(pNextGameState);
}

void CTestLevelIn::Kill()
{
	// Nothing to do...
}

bool CTestLevelIn::ForceKill()
{
	// No clean up to do atm...
	LOG_INFO(">>>> %s, FORCEKILL called!", this->m_pStateName);
	return true;
}
