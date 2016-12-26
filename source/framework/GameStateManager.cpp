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
		// AttemptForceKill the most likely culprit to leave this hanging
		ASSERT(pActiveGameState, "GameStateUpdate called without an active game state");
		pActiveGameState->Update();
	}

	void GameStateChange( CGameState* pNextState )
	{
		ASSERT( pNextState != pActiveGameState, "GameStateChange called with identical state" );
		ASSERT( pNextState != NULL, "GameStateChange called with null" );

		// This might be NULL if AttemptForceKill has been called...
		if(NULL != pActiveGameState)
		{
			pActiveGameState->Kill();
			DELETEX pActiveGameState;
		}	
			
		pActiveGameState = pNextState;
		pActiveGameState->Init();
	}

	void GameStateKill()
	{
		pActiveGameState->Kill();
		DELETEX pActiveGameState;
	}

	bool GameStateAttemptForceKill()
	{
		ASSERT( NULL != pActiveGameState, "GameStateAttemptForceKill called with no active gamestate!" );
		if(pActiveGameState->ForceKill())
		{
			DELETEX pActiveGameState;
			pActiveGameState = NULL;
			return true;
		}
		else
			LOG_ERROR("Force Kill Failed!");

		return false;
	}
};
