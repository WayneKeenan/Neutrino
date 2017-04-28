#include "CSplashScreenIn.h"
#include "CSplashScreenExit.h"
#include "../../framework/GameStateManager.h"
#include "../global/AudioFiles.h"


CSplashScreenIn::CSplashScreenIn()
{
	m_pStateName = "Splash Screen In";
	LOG_INFO(">>>> %s", this->m_pStateName);

	m_pAudioInterface = CGameGlobals::Instance().GetAudioInterface();
}

CSplashScreenIn::~CSplashScreenIn()
{

}

void CSplashScreenIn::Init()
{
	// TODO: move this into the update loop with a loading bar...
	m_pAudioInterface->LoadSample(eSFX::_STARTUP_CHIME, s_sStartUpChime);
	m_pAudioInterface->LoadMusic(eMusic::_TITLE, s_sTITLEMUSIC);
	m_pAudioInterface->PlaySample(eSFX::_STARTUP_CHIME);
	m_pAudioInterface->PlayMusic(eMusic::_TITLE);
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
