#include "GameStateManager.h"
#include "Assert.h"
#include "Memory.h"

#include "../game/gamestates/CSplashScreenEnter.h"

namespace Neutrino
{

	CGameState* pActiveGameState = NULL;

	bool GameStateInit()
	{
		pActiveGameState = NEWX CSplashScreenEnter();
		return true;
	}

	void GameStateUpdate()
	{
		ASSERT(pActiveGameState, "GameStateUpdate called without an active game state");
		pActiveGameState->Update();
	}

	void GameStateChange( CGameState* pNextState )
	{
		ASSERT( pNextState != pActiveGameState, "GameStateChange called with identical state" );
		ASSERT( pNextState != NULL, "GameStateChange called with null" );

		pActiveGameState->Kill();
		DELETEX pActiveGameState;

		pActiveGameState = pNextState;
	}

	void GameStateKill()
	{
		pActiveGameState->Kill();
		DELETEX pActiveGameState;
	}
};
