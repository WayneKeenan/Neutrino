#pragma once
#include "../../framework/GameState.h"


// Main Menu In
// Main menu intro animation...
//
class CMainMenuEnter : public CGameState
{
	public:

		CMainMenuEnter();
		~CMainMenuEnter();

		void Init();
		void Update();
		void Kill();
};
