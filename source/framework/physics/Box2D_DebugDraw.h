#pragma once
#include "Box2D/Box2D.h"
#include "../GLUtils.h"

namespace BOX2D_Debug
{
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
		void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) override;

		void DrawString(int x, int y, const char* string, ...);
		void DrawString(const b2Vec2& p, const char* string, ...);
		void DrawAABB(b2AABB* aabb, const b2Color& color);

		void DrawPhysicsWorld();

		void TestLineDraw();
		void TestFilledPolyDraw();
		void TestCircleDraw();
		void TestPoint();

	private:
		GLuint m_iBox2DDebugLineVBO;
		GLuint m_iBox2DDebugPointVBO;
		GLuint m_iBox2DDebugTriangleVBO;
		uint16 m_iDebugLinesVertexCount;
		uint16 m_iDebugPointCount;
		uint16 m_iDebugTrianglesCount;
	};
}
