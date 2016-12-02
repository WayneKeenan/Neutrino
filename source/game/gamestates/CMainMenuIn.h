#pragma once
#include "../../framework/GameState.h"


// Main Menu In
// Main menu intro animation...
//
class CMainMenuIn : public CGameState
{
	public:

		CMainMenuIn();
		~CMainMenuIn();

		void Init();
		void Update();
		void Kill();
};
