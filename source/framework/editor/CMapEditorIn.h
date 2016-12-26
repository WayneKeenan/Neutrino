#pragma once
#include "../GameState.h"


// Splash screen game state. 
// Display company logo and handle transition into the menu
// 
// Should handle checks for save game and other params prior to menu?
// Initialise GameGlobals?

class CMapEditorIn: public CGameState
{
	public:

		CMapEditorIn();
		~CMapEditorIn();

		void Init();
		void Update();
		void Kill();
		virtual bool ForceKill() override;

};
