#include "CMainMenuIn.h"



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
}

void CMainMenuIn::Kill()
{
	// Nothing to do...
}
