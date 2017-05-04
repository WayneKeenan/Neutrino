#pragma once
#include "physics/Box2D_DebugDraw.h"

namespace Neutrino
{

	void PhysicsInit();

	void PhysicsUpdate();

	void PhysicsKill();

#if defined DEBUG
	bool* GetPhysicsDebugDrawState();
	void PhysicsDrawWorld();
#endif
}