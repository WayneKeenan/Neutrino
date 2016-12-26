#pragma once
#include "Log.h"
#include "Assert.h"
#include "Memory.h"
#include "GLUtils.h"

class CGameState
{
public:
	CGameState();
	virtual ~CGameState();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Kill() = 0;

	// Editor modes won't wait for the Enter/In/Exit pattern to complete, instead entry to an editor mode
	// will cause the framework to call this function for an immediate clean-up and exit 
	//
	// If true, the framework will enter the new editor mode, if false it'll ignore the attempt and 
	// the existing gamestate continues ticking.  
	virtual bool ForceKill() {return false;}

protected:
	const char* m_pStateName;
};
