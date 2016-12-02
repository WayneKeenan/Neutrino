#pragma once
#include "../../framework/GameState.h"


// Main Menu In
// Main menu intro animation...
//
class CMainMenuExit : public CGameState
{
	public:

		CMainMenuExit();
		~CMainMenuExit();

		void Init();
		void Update();
		void Kill();
};
