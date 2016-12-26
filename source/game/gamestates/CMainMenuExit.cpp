#include "CMainMenuExit.h"



CMainMenuExit::CMainMenuExit()
{
	m_pStateName = "Main Menu Exit";
	LOG_INFO(">>>> %s", this->m_pStateName);
}

CMainMenuExit::~CMainMenuExit()
{

}

void CMainMenuExit::Init()
{
	// Check for existance of save games
	// Initialise Steam Manager
	// Set Localisation territory
	// Do any pre-flight checks for existence of level data, etc. 
}

void CMainMenuExit::Update()
{
	// Immediate change of state to splash screen in (animation for company logo)
}

void CMainMenuExit::Kill()
{
	// Nothing to do...
}


