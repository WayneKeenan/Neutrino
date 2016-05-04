#pragma once
#include "Log.h"
#include "Assert.h"

class CGameState
{
public:
	CGameState();
	virtual ~CGameState();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Kill() = 0;

protected:
	const char* m_pStateName;
};
