#pragma once

#include "../../framework/GameState.h"

class CSplashScreenEnter : public CGameState
{
	public:

		CSplashScreenEnter();
		~CSplashScreenEnter();

		void Init();
		void Update();
		void Kill();
};