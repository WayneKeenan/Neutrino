#include "Box2D_DebugDraw.h"
#include "../Assert.h"
#include "../Colour.h"
#include "../Types.h"
#include "../imgui/imgui.h"

using namespace Neutrino;
using namespace GLUtils;


// TODO: Get rid of these #defines once this class is finished and the physics world stuff has been implemented
// This is all test stuff for implementation... This wont be compiled into release builds at all...
#if defined DEBUG
namespace BOX2D_Debug
{
	Box2D_DebugDraw::Box2D_DebugDraw()
	{
		SetBox2DVBOPtrs(&m_iBox2DDebugLineVBO, &m_iBox2DDebugPointVBO, &m_iBox2DDebugTriangleVBO);
		m_iDebugLinesVertexCount = 0; m_iDebugPointCount = 0; m_iDebugTrianglesCount = 0;
	}

	Box2D_DebugDraw::~Box2D_DebugDraw()
	{

	}

	void Box2D_DebugDraw::DrawPolygon(const b2Vec2 * pVertices, int32 vertexCount, const b2Color & colour)
	{
		ASSERT(m_iDebugLinesVertexCount < _iMAX_BOX2D_SPRITES, "Call to Box2D Draw polygon when VBO is full");

		glBindBuffer(GL_ARRAY_BUFFER, m_iBox2DDebugLineVBO);
		ASSERT(!LogGlError(__FILE__, __LINE__), );

		Box2D_Vertex_t* pVertex = (Box2D_Vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		ASSERT(NULL != pVertex, "Unable to map to a buffer. Have you passed the correct VBO Index");
		pVertex += m_iDebugLinesVertexCount;

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
		// Draw triangles from the passed verts
		ASSERT(m_iDebugTrianglesCount < _iMAX_BOX2D_SPRITES, "Call to Box2D DrawSolidPolygon when VBO is full");

		glBindBuffer(GL_ARRAY_BUFFER, m_iBox2DDebugTriangleVBO);
		ASSERT(!LogGlError(__FILE__, __LINE__), );

		Box2D_Vertex_t* pVertex = (Box2D_Vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		ASSERT(NULL != pVertex, "Unable to map to a buffer. Have you passed the correct VBO Index");
		pVertex += m_iDebugTrianglesCount;

 		uint32 iColour = GetPackedColour(colour.r, colour.g, colour.b, 0.5f);
		for (int32 i = 1; i < iVertexCount - 1; ++i)
		{
			pVertex->_position[0] = pVertices[0].x;
			pVertex->_position[1] = pVertices[0].y;
			pVertex->_position[2] = 0.0f;
			pVertex->_colour = iColour;
			++pVertex;

			pVertex->_position[0] = pVertices[i].x;
			pVertex->_position[1] = pVertices[i].y;
			pVertex->_position[2] = 0.0f;
			pVertex->_colour = iColour;
			++pVertex;

			pVertex->_position[0] = pVertices[i + 1].x;
			pVertex->_position[1] = pVertices[i + 1].y;
			pVertex->_position[2] = 0.0f;
			pVertex->_colour = iColour;
			++pVertex;
			++m_iDebugTrianglesCount;
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

		ASSERT(m_iDebugLinesVertexCount < _iMAX_BOX2D_SPRITES, "Call to Box2D Draw polygon when VBO is full");

		glBindBuffer(GL_ARRAY_BUFFER, m_iBox2DDebugLineVBO);
		ASSERT(!LogGlError(__FILE__, __LINE__), );

		Box2D_Vertex_t* pVertex = (Box2D_Vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		ASSERT(NULL != pVertex, "Unable to map to a buffer. Have you passed the correct VBO Index");
		pVertex += m_iDebugLinesVertexCount;

		uint32 iColour = GetPackedColour(colour.r, colour.g, colour.b, colour.a);

		for (int32 i = 0; i < k_segments; ++i)
		{
			// Perform rotation to avoid additional trigonometry.
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

	void Box2D_DebugDraw::DrawSolidCircle(const b2Vec2 & center, float32 radius, const b2Vec2 & axis, const b2Color & color)
	{
	}

	void Box2D_DebugDraw::DrawSegment(const b2Vec2 & p1, const b2Vec2 & p2, const b2Color & color)
	{
	}

	void Box2D_DebugDraw::DrawTransform(const b2Transform & xf)
	{
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
	}

	void Box2D_DebugDraw::DrawAABB(b2AABB * aabb, const b2Color & color)
	{
	}

	void Box2D_DebugDraw::DrawPhysicsWorld()
	{
		RenderBox2DWorld(m_iDebugLinesVertexCount, m_iDebugPointCount, m_iDebugTrianglesCount);
		m_iDebugLinesVertexCount = 0; m_iDebugPointCount = 0; m_iDebugTrianglesCount = 0;
	}

	void Box2D_DebugDraw::TestLineDraw()
	{
		b2Vec2* pVertices = NEWX b2Vec2[4];
		pVertices[0] = b2Vec2(0.5f, 0.5f);
		pVertices[1] = b2Vec2(0.7f, 0.5f);
		pVertices[2] = b2Vec2(0.7f, 0.7f);
		pVertices[3] = b2Vec2(0.5f, 0.7f);
		this->DrawPolygon(pVertices, 4, b2Color(1.0f, 1.0f, 1.0f, 1.0f));
	}

	void Box2D_DebugDraw::TestFilledPolyDraw()
	{
		b2Vec2* pVertices = NEWX b2Vec2[6];
		pVertices[0] = b2Vec2(0.1f, 0.3f);
		pVertices[1] = b2Vec2(0.3f, 0.3f);
		pVertices[2] = b2Vec2(0.3f, 0.1f);
		pVertices[3] = b2Vec2(0.1f, 0.1f);
		this->DrawSolidPolygon(pVertices, 4, b2Color(0.0f, 1.0f, 0.0f, 1.0f));
	}

	void Box2D_DebugDraw::TestCircleDraw()
	{
		b2Vec2 vPos = b2Vec2(0.6f, 0.3f);
		this->DrawCircle(vPos, 0.05f, b2Color(1.0f, 1.0f, 0.0f, 1.0f));
	}

	void Box2D_DebugDraw::TestPoint()
	{
		this->DrawPoint(b2Vec2(0.8f, 0.3f),10.0f, b2Color(1.0f, 1.0f, 0.0f, 1.0f));
	}
}
#endif