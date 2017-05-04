#include "Physics.h"
#include "Box2D/Box2D.h"
#include "Memory.h"

#if defined DEBUG
	#include "physics/Box2D_DebugDraw.h"
#endif

namespace Neutrino
{
#if defined DEBUG
	static BOX2D_Debug::Box2D_DebugDraw* s_pPhysicsDebug = NULL;
#endif

	static bool s_bDebugDrawWorld = false;
	static bool s_bWorldIsDefined = false;

	void PhysicsInit()
	{
		#if defined DEBUG
			s_pPhysicsDebug = NEWX BOX2D_Debug::Box2D_DebugDraw();
			s_bWorldIsDefined = true;
		#endif
	}

	void PhysicsUpdate()
	{
		if (s_bWorldIsDefined)
		{
			#if defined DEBUG
				s_pPhysicsDebug->TestLineDraw();
				s_pPhysicsDebug->TestFilledPolyDraw();
				s_pPhysicsDebug->TestCircleDraw();
				s_pPhysicsDebug->TestPoint();
			#endif
		}
	}

	void PhysicsKill()
	{
		#if defined DEBUG
			SAFE_DELETE(s_pPhysicsDebug);
		#endif
	}

#if defined DEBUG
	bool* GetPhysicsDebugDrawState()
	{
		return &s_bDebugDrawWorld;
	}

	void PhysicsDrawWorld()
	{
		if(s_bWorldIsDefined && s_bDebugDrawWorld) s_pPhysicsDebug->DrawPhysicsWorld();
	}
#endif
}
