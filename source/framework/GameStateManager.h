#pragma once
#include "GameState.h"

namespace Neutrino
{

	extern CGameState* pActiveGameState;

	bool GameStateInit();
	void GameStateUpdate();
	void GameStateChange( CGameState* pNextState );
}