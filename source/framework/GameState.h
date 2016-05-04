#pragma once
#include "Log.h"
#include "Assert.h"

class CGameState
{
public:
	CGameState();
	virtual ~CGameState();


	// [GN 4.4.16] Question mark over the instantiation and use of these objects. 
	// Assuming that creation will not be in the same place as where initialisation 
	// is to occur (for the time being). If not be the case, constructor can do the legwork...
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Kill() = 0;

protected:
	const char* m_pStateName;
};
