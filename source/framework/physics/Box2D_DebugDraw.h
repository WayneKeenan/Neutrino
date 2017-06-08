#pragma once
#include "Box2D/Box2D.h"
#include "../GLUtils.h"

#if defined DEBUG
namespace BOX2D_Debug
{
	// Box2D_DebugDraw is a port of the reference renderer from Box2D's test bed example
	// It provides debug draw functions to Box2D that can be toggled from the DEBUG 
	// overlay that the framework provides. 
	// 
	// See Physics.h for how this is passed and to call the test render functions. 
	// 
	// Absolutely zero fucks given as to how this performs. 
	// 
	// NOT IMPLEMENTED:
	//		1.	The scaling from screen space to physics world space has not been done, 
	//				meaning one of the DrawString overrides will not function at present

	class Box2D_DebugDraw : public b2Draw
	{
	public:
		Box2D_DebugDraw();
		~Box2D_DebugDraw();

		void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
		void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
		void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;
		void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;
		void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
		void DrawTransform(const b2Transform& xf) override;
		void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);

		void DrawString(int x, int y, const char* string, ...);
		void DrawString();//const b2Vec2& p, const char* string, ...);
		void DrawAABB(b2AABB* aabb, const b2Color& color);

		void DrawPhysicsWorld();

		void TestLineDraw();
		void TestFilledPolyDraw();
		void TestCircleDraw();
		void TestFilledCircleDraw();
		void TestPoint();
		void TestDrawSegment();
		void TestDrawTransform();

	private:
		// These are grabbed from the framework on construction
		GLuint m_iBox2DDebugLineVBO;
		GLuint m_iBox2DDebugPointVBO;
		GLuint m_iBox2DDebugTriangleVBO;

		// Counters are passed to the framework when DrawPhysicsWorld is called;
		uint16 m_iDebugLinesVertexCount;
		uint16 m_iDebugPointCount;
		uint16 m_iDebugTriangleVertexCount;
	};
}
#endif