#include "Box2D_DebugDraw.h"
#include "../Assert.h"
#include "../Colour.h"
#include "../Types.h"
#include "../imgui/imgui.h"

using namespace Neutrino;
using namespace GLUtils;

#if defined DEBUG
namespace BOX2D_Debug
{
	static Box2D_Vertex_t* MapVertex(const int iCount, const GLuint VBOID)
	{
		ASSERT(iCount < _iMAX_BOX2D_SPRITES, "Call to Box2D Draw polygon when VBO is full");

		glBindBuffer(GL_ARRAY_BUFFER, VBOID);
		ASSERT(!LogGlError(__FILE__, __LINE__), );

		Box2D_Vertex_t* pVertex = (Box2D_Vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		ASSERT(NULL != pVertex, "Unable to map to a buffer. Have you passed the correct VBO Index");
		pVertex += iCount;

		return pVertex;
	}




	Box2D_DebugDraw::Box2D_DebugDraw()
	{
		SetBox2DVBOPtrs(&m_iBox2DDebugLineVBO, &m_iBox2DDebugPointVBO, &m_iBox2DDebugTriangleVBO);
		m_iDebugLinesVertexCount = 0; m_iDebugPointCount = 0; m_iDebugTriangleVertexCount = 0;
	}

	Box2D_DebugDraw::~Box2D_DebugDraw()
	{

	}

	void Box2D_DebugDraw::DrawPolygon(const b2Vec2 * pVertices, int32 vertexCount, const b2Color & colour)
	{
		Box2D_Vertex_t* pVertex = MapVertex(m_iDebugLinesVertexCount, m_iBox2DDebugLineVBO);
		b2Vec2 p1 = pVertices[vertexCount - 1];
		uint32 iColour = GetPackedColour(colour.r, colour.g, colour.b, colour.a);
		for (int32 i = 0; i < vertexCount; ++i)
		{
			b2Vec2 p2 = pVertices[i];
			pVertex->_colour = iColour;
			pVertex->_position[0] = p1.x;
			pVertex->_position[1] = p1.y;
			pVertex->_position[2] = 0.0f;
			++pVertex;
			++m_iDebugLinesVertexCount;
			pVertex->_colour = iColour;
			pVertex->_position[0] = p2.x;
			pVertex->_position[1] = p2.y;
			pVertex->_position[2] = 0.0f;
			++pVertex;
			p1 = p2;
			++m_iDebugLinesVertexCount;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	void Box2D_DebugDraw::DrawSolidPolygon(const b2Vec2 * pVertices, int32 iVertexCount, const b2Color & colour)
	{
		Box2D_Vertex_t* pVertex = MapVertex(m_iDebugTriangleVertexCount, m_iBox2DDebugTriangleVBO);
 		uint32 iColour = GetPackedColour(colour.r, colour.g, colour.b, 0.5f);
		for (int32 i = 1; i < iVertexCount - 1; ++i)
		{
			pVertex->_position[0] = pVertices[0].x;
			pVertex->_position[1] = pVertices[0].y;
			pVertex->_position[2] = 0.0f;
			pVertex->_colour = iColour;
			++pVertex;
			++m_iDebugTriangleVertexCount;

			pVertex->_position[0] = pVertices[i].x;
			pVertex->_position[1] = pVertices[i].y;
			pVertex->_position[2] = 0.0f;
			pVertex->_colour = iColour;
			++pVertex;
			++m_iDebugTriangleVertexCount;

			pVertex->_position[0] = pVertices[i + 1].x;
			pVertex->_position[1] = pVertices[i + 1].y;
			pVertex->_position[2] = 0.0f;
			pVertex->_colour = iColour;
			++pVertex;
			++m_iDebugTriangleVertexCount;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		DrawPolygon(pVertices, iVertexCount, colour);
	}

	void Box2D_DebugDraw::DrawCircle(const b2Vec2 & center, float32 radius, const b2Color & colour)
	{
		const float32 k_segments = 16.0f;
		const float32 k_increment = 2.0f * b2_pi / k_segments;
		float32 sinInc = sinf(k_increment);
		float32 cosInc = cosf(k_increment);
		b2Vec2 r1(1.0f, 0.0f);
		b2Vec2 v1 = center + radius * r1;

		Box2D_Vertex_t* pVertex = MapVertex(m_iDebugLinesVertexCount, m_iBox2DDebugLineVBO);
		uint32 iColour = GetPackedColour(colour.r, colour.g, colour.b, colour.a);

		for (int32 i = 0; i < (int)k_segments; ++i)
		{
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
			pVertex->_colour = iColour;
			pVertex->_position[0] = v1.x;
			pVertex->_position[1] = v1.y;
			pVertex->_position[2] = 0.0f;
			++pVertex;
			++m_iDebugLinesVertexCount;
			pVertex->_colour = iColour;
			pVertex->_position[0] = v2.x;
			pVertex->_position[1] = v2.y;
			pVertex->_position[2] = 0.0f;
			++pVertex;
			++m_iDebugLinesVertexCount;
			r1 = r2;
			v1 = v2;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	void Box2D_DebugDraw::DrawSolidCircle(const b2Vec2 & center, float32 radius, const b2Vec2 & axis, const b2Color & colour)
	{
		const float32 k_segments = 16.0f;
		const float32 k_increment = 2.0f * b2_pi / k_segments;
		float32 sinInc = sinf(k_increment);
		float32 cosInc = cosf(k_increment);
		b2Vec2 v0 = center;
		b2Vec2 r1(cosInc, sinInc);
		b2Vec2 v1 = center + radius * r1;

		Box2D_Vertex_t* pVertex = MapVertex(m_iDebugTriangleVertexCount, m_iBox2DDebugTriangleVBO);
		uint32 iColour = GetPackedColour(colour.r, colour.g, colour.b, 0.5f);
		for (int32 i = 0; i < k_segments; ++i)
		{
			// Perform rotation to avoid additional trigonometry.
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
			pVertex->_position[0] = v0.x;
			pVertex->_position[1] = v0.y;
			pVertex->_position[2] = 0.0f;
			pVertex->_colour = iColour;
			++pVertex;
			++m_iDebugTriangleVertexCount;

			pVertex->_position[0] = v1.x;
			pVertex->_position[1] = v1.y;
			pVertex->_position[2] = 0.0f;
			pVertex->_colour = iColour;
			++pVertex;
			++m_iDebugTriangleVertexCount;

			pVertex->_position[0] = v2.x;
			pVertex->_position[1] = v2.y;
			pVertex->_position[2] = 0.0f;
			pVertex->_colour = iColour;
			++pVertex;
			++m_iDebugTriangleVertexCount;

			r1 = r2;
			v1 = v2;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		r1.Set(1.0f, 0.0f);
		v1 = center + radius * r1;
		pVertex = MapVertex(m_iDebugLinesVertexCount, m_iBox2DDebugLineVBO);
		iColour = GetPackedColour(colour.r, colour.g, colour.b, colour.a);
		for (int32 i = 0; i < k_segments; ++i)
		{
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
			pVertex->_colour = iColour;
			pVertex->_position[0] = v1.x;
			pVertex->_position[1] = v1.y;
			pVertex->_position[2] = 0.0f;
			++pVertex;
			++m_iDebugLinesVertexCount;
			pVertex->_colour = iColour;
			pVertex->_position[0] = v2.x;
			pVertex->_position[1] = v2.y;
			pVertex->_position[2] = 0.0f;
			++pVertex;
			++m_iDebugLinesVertexCount;
			r1 = r2;
			v1 = v2;
		}

		// Draw a line fixed in the circle to animate rotation.
		b2Vec2 p = center + radius * axis;
		pVertex->_colour = iColour;
		pVertex->_position[0] = center.x;
		pVertex->_position[1] = center.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;
		pVertex->_colour = iColour;
		pVertex->_position[0] = p.x;
		pVertex->_position[1] = p.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	void Box2D_DebugDraw::DrawSegment(const b2Vec2 & p1, const b2Vec2 & p2, const b2Color & colour)
	{
		Box2D_Vertex_t* pVertex = MapVertex(m_iDebugLinesVertexCount, m_iBox2DDebugLineVBO);
		uint32 iColour = GetPackedColour(colour.r, colour.g, colour.b, colour.a);

		pVertex->_colour = iColour;
		pVertex->_position[0] = p1.x;
		pVertex->_position[1] = p1.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;

		pVertex->_colour = iColour;
		pVertex->_position[0] = p2.x;
		pVertex->_position[1] = p2.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	void Box2D_DebugDraw::DrawTransform(const b2Transform & xf)
	{
		const float32 k_axisScale = 0.4f;
		uint32 red = GetPackedColour(1.0f, 0.0f, 0.0f, 1.0f);
		uint32 green = GetPackedColour(0.0f, 1.0f, 0.0f, 1.0f); (0.0f, 1.0f, 0.0f);
		b2Vec2 p1 = xf.p, p2;
		Box2D_Vertex_t* pVertex = MapVertex(m_iDebugLinesVertexCount, m_iBox2DDebugLineVBO);

		pVertex->_colour = red;
		pVertex->_position[0] = p1.x;
		pVertex->_position[1] = p1.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;

		p2 = p1 + k_axisScale * xf.q.GetXAxis();

		pVertex->_colour = red;
		pVertex->_position[0] = p2.x;
		pVertex->_position[1] = p2.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;

		pVertex->_colour = green;
		pVertex->_position[0] = p1.x;
		pVertex->_position[1] = p1.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;

		p2 = p1 + k_axisScale * xf.q.GetYAxis();
		
		pVertex->_colour = green;
		pVertex->_position[0] = p2.x;
		pVertex->_position[1] = p2.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	void Box2D_DebugDraw::DrawPoint(const b2Vec2 & p, float32 size, const b2Color & colour)
	{
		ASSERT(m_iDebugPointCount < _iMAX_BOX2D_SPRITES, "Call to Box2D Draw polygon when VBO is full");
		glBindBuffer(GL_ARRAY_BUFFER, m_iBox2DDebugPointVBO);
		ASSERT(!LogGlError(__FILE__, __LINE__), );
		Box2D_DebugPoint_t* pVertex = (Box2D_DebugPoint_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		ASSERT(NULL != pVertex, "Unable to map to a buffer. Have you passed the correct VBO Index");
		pVertex += m_iDebugPointCount;
		uint32 iColour = GetPackedColour(colour.r, colour.g, colour.b, colour.a);
		pVertex->_colour = iColour;
		pVertex->_position[0] = p.x;
		pVertex->_position[1] = p.y;
		pVertex->_position[2] = 0.0f;
		pVertex->_size = size;
		++m_iDebugPointCount;
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	void Box2D_DebugDraw::DrawString(int x, int y, const char * string, ...)
	{
		va_list arg;
		va_start(arg, string);
		ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
		ImGui::SetCursorPos(ImVec2(float(x), float(y)));
		ImGui::TextColoredV(ImColor(230, 153, 153, 255), string, arg);
		ImGui::End();
		va_end(arg);
	}

	void Box2D_DebugDraw::DrawString(const b2Vec2 & p, const char * string, ...)
	{
		// Not implemented, requires scaling to and from screen space to the physics world space. 
	}

	void Box2D_DebugDraw::DrawAABB(b2AABB * aabb, const b2Color & colour)
	{
		b2Vec2 p1 = aabb->lowerBound;
		b2Vec2 p2 = b2Vec2(aabb->upperBound.x, aabb->lowerBound.y);
		b2Vec2 p3 = aabb->upperBound;
		b2Vec2 p4 = b2Vec2(aabb->lowerBound.x, aabb->upperBound.y);

		Box2D_Vertex_t* pVertex = MapVertex(m_iDebugLinesVertexCount, m_iBox2DDebugLineVBO);
		uint32 iColour = GetPackedColour(colour.r, colour.g, colour.b, colour.a);

		pVertex->_colour = iColour;
		pVertex->_position[0] = p1.x;
		pVertex->_position[1] = p1.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;
		
		pVertex->_colour = iColour;
		pVertex->_position[0] = p2.x;
		pVertex->_position[1] = p2.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;

		pVertex->_colour = iColour;
		pVertex->_position[0] = p2.x;
		pVertex->_position[1] = p2.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;

		pVertex->_colour = iColour;
		pVertex->_position[0] = p3.x;
		pVertex->_position[1] = p3.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;

		pVertex->_colour = iColour;
		pVertex->_position[0] = p3.x;
		pVertex->_position[1] = p3.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;

		pVertex->_colour = iColour;
		pVertex->_position[0] = p4.x;
		pVertex->_position[1] = p4.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;

		pVertex->_colour = iColour;
		pVertex->_position[0] = p4.x;
		pVertex->_position[1] = p4.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;

		pVertex->_colour = iColour;
		pVertex->_position[0] = p1.x;
		pVertex->_position[1] = p1.y;
		pVertex->_position[2] = 0.0f;
		++pVertex;
		++m_iDebugLinesVertexCount;
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	void Box2D_DebugDraw::DrawPhysicsWorld()
	{
		RenderBox2DWorld(m_iDebugLinesVertexCount, m_iDebugPointCount, m_iDebugTriangleVertexCount);
		m_iDebugLinesVertexCount = 0; 
		m_iDebugPointCount = 0; 
		m_iDebugTriangleVertexCount = 0;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Noddy test functions, that'll render a random example of each of the types to the screen. 
	// These are no use for anything, but I've left them in should I need to debug anything. 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

	void Box2D_DebugDraw::TestLineDraw()
	{
		b2Vec2* pVertices = NEWX b2Vec2[4];
		b2Color colour = b2Color(1.0f, 1.0f, 1.0f, 1.0f);
		pVertices[0] = b2Vec2(0.5f, 0.5f);
		pVertices[1] = b2Vec2(0.7f, 0.5f);
		pVertices[2] = b2Vec2(0.7f, 0.7f);
		pVertices[3] = b2Vec2(0.5f, 0.7f);
		this->DrawPolygon(pVertices, 4, colour);
		DELETEX pVertices;
	}

	void Box2D_DebugDraw::TestFilledPolyDraw()
	{
		b2Vec2* pVertices = NEWX b2Vec2[6];
		b2Color colour = b2Color(1.0f, 1.0f, 1.0f, 1.0f);
		pVertices[0] = b2Vec2(0.1f, 0.3f);
		pVertices[1] = b2Vec2(0.3f, 0.3f);
		pVertices[2] = b2Vec2(0.3f, 0.1f);
		pVertices[3] = b2Vec2(0.1f, 0.1f);
		this->DrawSolidPolygon(pVertices, 4, colour);
		DELETEX pVertices;

		pVertices = NEWX b2Vec2[6];
		pVertices[0] = b2Vec2(0.5f, 0.3f);
		pVertices[1] = b2Vec2(0.7f, 0.3f);
		pVertices[2] = b2Vec2(0.7f, 0.1f);
		pVertices[3] = b2Vec2(0.5f, 0.1f);
		this->DrawSolidPolygon(pVertices, 4, colour);
		DELETEX pVertices;
	}

	void Box2D_DebugDraw::TestCircleDraw()
	{
		b2Vec2 vPos = b2Vec2(0.6f, 0.3f);
		b2Color colour = b2Color(1.0f, 1.0f, 1.0f, 1.0f);
		this->DrawCircle(vPos, 0.05f, colour);
	}

	void Box2D_DebugDraw::TestFilledCircleDraw()
	{
		b2Vec2 p1 = b2Vec2(0.7f, 0.3f);
		b2Vec2 p2 = b2Vec2(0.0f, 1.0f);
		b2Color colour = b2Color(1.0f, 1.0f, 1.0f, 1.0f);
		this->DrawSolidCircle(p1, 0.05f, p2, colour);
	}

	void Box2D_DebugDraw::TestPoint()
	{
		b2Color colour = b2Color(1.0f, 1.0f, 1.0f, 1.0f);
		b2Vec2 p1 = b2Vec2(0.8f, 0.3f);
		this->DrawPoint(p1,10.0f, colour);
	}

	void Box2D_DebugDraw::TestDrawSegment()
	{
		b2Vec2 p1 = b2Vec2(0.7f, 0.9f);
		b2Vec2 p2 = b2Vec2(0.7f, 1.0f);
		b2Color colour = b2Color(1.0f, 1.0f, 1.0f, 1.0f);
		this->DrawSegment(p1, p2, colour);
	}

	void Box2D_DebugDraw::TestDrawTransform()
	{
		b2Vec2 p1 = b2Vec2(0.8f, 0.5f);
		b2Rot r1 = b2Rot(45.0f);
		b2Transform bt = b2Transform(p1, r1);
		this->DrawTransform(bt);
	}
}
#endif