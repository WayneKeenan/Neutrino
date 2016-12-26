#include "CMainMenuIn.h"
#include "../../framework/Sprite.h"



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
	Neutrino::TestSprite();
}

void CMainMenuIn::Kill()
{
	// Nothing to do...
}


bool CMainMenuIn::ForceKill()
{
	// No clean up to do atm...
	LOG_INFO(">>>> %s, FORCEKILL called!", this->m_pStateName );
	return true;
}
