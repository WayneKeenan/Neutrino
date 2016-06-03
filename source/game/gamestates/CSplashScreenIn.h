#pragma once
#include "../../framework/GameState.h"


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
};
