#include "CGameGlobals.h"
#include "../../framework/Log.h"
#include "../../framework/Types.h"
#include "AudioFiles.h"

#if defined _SDL_MIXER_AUDIO
	#include "../../framework/audio/CSDLMixer.h"
#endif



CGameGlobals::CGameGlobals() 
{
	if(_SDL_MIXER_AUDIO)
	{
		m_pAudioInterface = NEWX Neutrino::CSDLMixer();
	}
	LOG_INFO("Game Globals created"); 
};

CGameGlobals::~CGameGlobals()
{
	if (_SDL_MIXER_AUDIO)
	{
		DELETEX m_pAudioInterface;
	}
	LOG_INFO("Game Globals destroyed");
}

void CGameGlobals::SetPlayerPreferences(const NeutrinoPreferences_t* const pPreferences)
{
	ASSERT(NULL != pPreferences, "SetPlayerPreferences called with NULL pointer");

	// TODO: Shader preferences should be added to this?
	m_iInternalWidth = (uint16)pPreferences->_iInternalWidth;
	m_iInternalHeight = (uint16)pPreferences->_iInternalHeight;
	m_fPixelWidth = pPreferences->_InternalPixelWidth;
	m_fPixelHeight = pPreferences->_InternalPixelHeight;
	m_fGlobalVolume = pPreferences->_fMasterVolume;
	m_fSampleVolume = pPreferences->_fSampleVolume;
	m_fMusicVolume = pPreferences->_fMusicVolume;

	m_pAudioInterface->SetGlobalVolume(m_fGlobalVolume);
	m_pAudioInterface->SetMusicVolume(m_fMusicVolume);
	m_pAudioInterface->SetSampleVolume(m_fSampleVolume);
}



void CGameGlobals::SetGameDefaults()
{
	m_iLevelNumber = 0;
	m_iPlayer1Score = 0;

	LOG_INFO("Game Defaults have been set.");
}



// Output a debug inspector for the game globals. 
#if defined DEBUG
#include "../../framework/imgui/imgui.h"


static const ImVec2* s_pPosition = NEWX ImVec2(1620.0f, 10.0f);
static const ImVec2* s_pDimensions = NEWX ImVec2(280.0f, 380.0f);
static const ImGuiTreeNodeFlags iFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed;
static bool s_bDebugOverlayActive = true;

// Nice names of the music
static const char* s_aMusicNames[] = { "Game Over", "Title", "Level A", "Level B", "Level C", "Loading", "Completion" };

void CGameGlobals::DebugUpdate() 
{
	ImGui::SetNextWindowPos(*s_pPosition, ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(*s_pDimensions, ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Game Globals Inspector", &s_bDebugOverlayActive);
	if (ImGui::CollapsingHeader("Audio:", iFlags))
	{
		static int iActiveMusic = eMusic::_TITLE;
		if (ImGui::Combo("Music Selector", &iActiveMusic, s_aMusicNames, ((int)(sizeof(s_aMusicNames) / sizeof(*s_aMusicNames))))) m_pAudioInterface->PlayMusic(iActiveMusic);
		if (ImGui::SliderFloat("Master Vol.", &m_fGlobalVolume, 0.0f, 1.0f, "%2f")) m_pAudioInterface->SetGlobalVolume(m_fGlobalVolume);
		if (ImGui::SliderFloat("Sample Vol.", &m_fSampleVolume, 0.0f, 1.0f, "%2f")) m_pAudioInterface->SetSampleVolume(m_fSampleVolume);
		if (ImGui::SliderFloat("Music Vol.", &m_fMusicVolume, 0.0f, 1.0f, "%2f")) m_pAudioInterface->SetMusicVolume(m_fMusicVolume);
		if (ImGui::Button("Play Test Sample", ImVec2(260.0f, 0.0f))) m_pAudioInterface->PlaySample(eSFX::_STARTUP_CHIME);
	}
	ImGui::End();
}
#endif