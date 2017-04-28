#pragma once
#include "../../framework/Log.h"
#include "../../framework/Singleton.h"
#include "../../framework/Framework.h"
#include "../../framework/audio/CAudioInterface.h"

class CGameGlobals : public CSingleton<CGameGlobals>
{
public:


	// Game specific public members
	//
	uint8 m_iLevelNumber = 0;
	uint32 m_iPlayer1Score = 0;
	
	CGameGlobals();
	~CGameGlobals();

	// Getters
	//
	inline float GetPixelWidth() const { return m_fPixelWidth; };
	inline float GetPixelHeight() const { return m_fPixelHeight; };
	inline uint16 GetInternalWidth() const { return m_iInternalWidth; };
	inline uint16 GetInternalHeight() const { return m_iInternalHeight; };
	inline Neutrino::CAudioInterface* GetAudioInterface() const { ASSERT(NULL != m_pAudioInterface, "Audio interface has not been initialised"); return m_pAudioInterface; };

	// Setters
	//
	void SetPlayerPreferences(const NeutrinoPreferences_t* const pPreferences);
	void SetGameDefaults();

	// TODO:
	// void WriteGameDefaults()		-> Save to the player's preferences file (This needs to be added to the framework)
	// void WriteSaveSlot()				-> Save game etc. 
	

private:
	// Internal dimensions, used for rendering to the low res texture and clamping movements
	//
	uint16 m_iInternalWidth = 0;
	uint16 m_iInternalHeight = 0;
	float m_fPixelWidth = 0.0f;
	float m_fPixelHeight = 0.0f;

	float m_fGlobalVolume = 1.0f;
	float m_fSampleVolume = 1.0f;
	float m_fMusicVolume = 1.0f;

	// Game 'managers'
	Neutrino::CAudioInterface* m_pAudioInterface = NULL;


	// In Debug builds GameGlobals can output an ImGUI inspector for its settings, DebugOverlayUpdate() will 
	// call it as part of the update loop;
#if defined DEBUG
public:
	void DebugUpdate();
#else
public:
	void DebugUpdate() {};
#endif
};

