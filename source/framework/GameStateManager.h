#pragma once
#include "GameState.h"

namespace Neutrino
{

	extern CGameState* pActiveGameState;

	// GameStateInit()
	// 		Should be called after engine bootstrap. 
	//   	Initialiases the first gameState (cSplashScreen) ready for CoreUpdate game ticks
	bool GameStateInit();

	//	GameStateUpdate()
	// 		Called once per frame, ticks current game state...
	void GameStateUpdate();

	//	GameStateChange
	// 		Handles deletion of active game state and safe creation and entry of next game state
	void GameStateChange( CGameState* pNextState );

	//	GameStateKill
	// 		Safe termination of active gamestate for engine termination
	void GameStateKill();

	// GameStateAttemptForceKill()
	// 		Call the forcekill method as the framework wants to enter one of the editor modes. 
	// 		will return false if the gamestate does not support immediate cleanup and exit
	bool GameStateAttemptForceKill();
}
