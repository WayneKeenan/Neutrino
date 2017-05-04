#include "CTestLevelIn.h"
#include "../../framework/GameStateManager.h"
#include "../../framework/Physics.h"

using namespace Neutrino;
using namespace GLUtils;

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
	SetClearColour(0.0f, 0.0f, 0.1f, 1.0f);
	PhysicsInit();
}

void CTestLevelIn::Update()
{
	PhysicsUpdate();
}

void CTestLevelIn::Kill()
{
	PhysicsKill();
}

bool CTestLevelIn::ForceKill()
{
	// No clean up to do atm...
	LOG_INFO(">>>> %s, FORCEKILL called!", this->m_pStateName);
	return true;
}
