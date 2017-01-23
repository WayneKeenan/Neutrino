#include "CMapEditorIn.h"
#include "../GameStateManager.h"
#include "../Sprite.h"
#include "../GLUtils.h"
#include "../imgui/imgui.h"
#include "../Texture.h"
#include "../Sprite.h"
#include "../Memory.h"
#include "../Input.h"
#include <string>
#include <glm/glm.hpp> 


#if defined _WIN32
#define sprintf sprintf_s
#endif

#define ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR) / sizeof(*_ARR)))

static const ImVec2* s_pWindowPosition = NEWX ImVec2(1300.0f, 50.0f);
static const float s_iRowPixels = 400.0f;
static bool s_bSnapToGrid = false;


CMapEditorIn::CMapEditorIn()
{
	m_pStateName = "Map Editor In";
	LOG_INFO(">>>> %s", this->m_pStateName);
}



CMapEditorIn::~CMapEditorIn()
{

}



void CMapEditorIn::Init()
{
	// Do any cleanup from previous frame here. 
	// Note: we'll be skipping the Exit state of where-ever we are (MenuExit, LevelExit etc) so this could get hairy...
	Neutrino::ResetSpriteCount();
	Neutrino::GLUtils::SetClearColour(0.0f, 0.05f, 0.0f, 1.0f);
}

static int s_iSelectedTexture = 0;
static int s_iSelectedGridSize = 4;

void CMapEditorIn::Update()
{
	glm::vec2* pMouseCoords = Neutrino::GetMouseCoords();
	// Draw the window
	ImGui::SetNextWindowPos(*s_pWindowPosition, ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(s_iRowPixels, 200.0f), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Tilemap Editor");
	ImGui::LabelText("", "Grid Settings:");
	ImGui::SameLine(240);
	ImGui::Text("[%.1f, %.1f]", pMouseCoords->x, pMouseCoords->y);

	{
		// Display Grid Settings 
		{
			ImGui::PushItemWidth(150); 
			ImGui::Combo("Grid Size", &s_iSelectedGridSize, " 2x2 \0 4x4 \0 8x8 \0 16x16 \0 32x32 \0 64x64 \0", 6); 
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Checkbox("Snap", &s_bSnapToGrid); 
			ImGui::Separator();
		}

		// Display framework texture count
		static uint8 iTextureCount = Neutrino::GetLoadedTextureCount();
		ImGui::LabelText("", "Texture Settings:"); ImGui::SameLine(165); ImGui::LabelText("", "[%d Loaded]", iTextureCount);

		// Build a string of Texture Identifiers for a drop down list
		{
			std::string sBuff;
			for (int i = 0; i < iTextureCount; ++i)
				sBuff += " Texture: " + std::to_string(i) + " \0 ";

			ImGui::Combo("Select Texture", &s_iSelectedTexture, sBuff.c_str(), iTextureCount + 1);
		}


		// Display selected texture info
		static const Neutrino::TPage_t* pTpage = Neutrino::GetTPage((uint8)s_iSelectedTexture);
		ImGui::Separator();

		// Display all the sprites in the texture page
		{
			ImGui::LabelText("", "Available Tiles:");
			ImVec2* vDim = NEWX ImVec2(0.0f, 0.0f);
			ImVec2* vUV0 = NEWX ImVec2(0.0f, 0.0f);
			ImVec2* vUV1 = NEWX ImVec2(0.0f, 0.0f);

			float iPixelsUsed = 0.0f;
			for (int i = 0; i < pTpage->_iMaxSprites; ++i)
			{
				vDim->x = pTpage->aSprintInfo[i]._fHalfWidth *2.0f;
				vDim->y = pTpage->aSprintInfo[i]._fHalfHeight * 2.0f;
				vUV0->x = pTpage->aSprintInfo[i]._fX_S;
				vUV0->y = pTpage->aSprintInfo[i]._fY_T;
				vUV1->x = pTpage->aSprintInfo[i]._fX_SnS;
				vUV1->y = pTpage->aSprintInfo[i]._fY_TnT;

				iPixelsUsed += vDim->x;

				// Note to future Gareth: GCC warns about casting to pointer from int of different size here.
				// The intptr_t cast removes that, but this was the first time that you saw that warning. 
				// Step through with a debugger to see where this goes, and read a book. 
				ImGui::ImageButton((void*)(intptr_t)pTpage->_iTextureID, *vDim, *vUV0, *vUV1, 1);
				if (iPixelsUsed < s_iRowPixels) ImGui::SameLine(); else iPixelsUsed = 0;

			}
			DELETEX vDim;
			DELETEX vUV0;
			DELETEX vUV1;
		}
	}
	ImGui::End();
}



void CMapEditorIn::Kill()
{
	// Nothing to do...
}



bool CMapEditorIn::ForceKill()
{
	LOG_INFO(">>>> %s, FORCEKILL called!", this->m_pStateName );
	return true;
}


