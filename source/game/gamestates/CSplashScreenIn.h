#pragma once
#include "../../framework/GameState.h"
#include "../../framework/audio/CAudioInterface.h"
#include "../global/CGameGlobals.h"
#include "../global/AudioFiles.h"


// Splash screen IN game state. 
// Perform animation and pub specific stuff...

class CSplashScreenIn : public CGameState
{
	public:

		CSplashScreenIn();
		~CSplashScreenIn();

		void Init();
		void Update();
		void Kill();

private:
	Neutrino::CAudioInterface* m_pAudioInterface = NULL;
};
