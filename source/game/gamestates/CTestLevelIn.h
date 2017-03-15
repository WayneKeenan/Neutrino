#pragma once
#include "../../framework/GameState.h"


// Splash screen IN game state. 
// Perform animation and pub specific stuff...

class CTestLevelIn: public CGameState
{
	public:

		CTestLevelIn();
		~CTestLevelIn();

		void Init();
		void Update();
		void Kill();
};
