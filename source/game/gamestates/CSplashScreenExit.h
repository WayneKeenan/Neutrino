#pragma once
#include "../../framework/GameState.h"


// Splash screen game state. 
// Display company logo and handle transition into the menu
// 
// Should handle checks for save game and other params prior to menu?
// Initialise GameGlobals?

class CSplashScreenExit : public CGameState
{
	public:

		CSplashScreenExit();
		~CSplashScreenExit();

		void Init();
		void Update();
		void Kill();
};
