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
	// Generate some test sprites (TO BE REMOVED)
	Neutrino::TestSprite();
}

void CMainMenuIn::Kill()
{
	// Nothing to do...
}
