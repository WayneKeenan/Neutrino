#include "CMapEditorIn.h"
#include "../GameStateManager.h"
#include "../Sprite.h"
#include "../GLUtils.h"
#include "../imgui/imgui.h"
#include "../Texture.h"
#include "../Memory.h"
#include "../Input.h"
#include <string>
#include <math.h>
#include <glm/glm.hpp> 


#if defined _WIN32
#define sprintf sprintf_s
#endif

#define ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR) / sizeof(*_ARR)))

static const ImVec2* s_pWindowPosition = NEWX ImVec2(1300.0f, 50.0f);
static const float s_iRowPixels = 400.0f;
static bool s_bSnapToGrid = true;
static int s_iSelectedTexture = 0;
static int s_iSelectedGridSize = 5;
static Neutrino::Sprite_t* s_pSelectedTile;
static uint16 s_iTileIndex = 0;
static bool s_bSpriteSelected;

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
			sBuff += " Texture: " + std::to_string(i) + " \0";

		ImGui::Combo("Select Texture", &s_iSelectedTexture, sBuff.c_str(), iTextureCount);
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
		for (uint16 i = 0; i < pTpage->_iMaxSprites; ++i)
		{
			// Set params ImageButton needs
			vDim->x = pTpage->aSprintInfo[i]._fHalfWidth *2.0f;
			vDim->y = pTpage->aSprintInfo[i]._fHalfHeight * 2.0f;
			vUV0->x = pTpage->aSprintInfo[i]._fX_S;
			vUV0->y = pTpage->aSprintInfo[i]._fY_T;
			vUV1->x = pTpage->aSprintInfo[i]._fX_SnS;
			vUV1->y = pTpage->aSprintInfo[i]._fY_TnT;

			// Update horizontal distance traversed, sp we can check for a new line
			iPixelsUsed += vDim->x;

			// Note to future Gareth: GCC warns about casting to pointer from int of different size here.
			// The intptr_t cast removes that, but this was the first time that you saw that warning. 
			// Step through with a debugger to see where this goes, and read a book. 
			ImGui::PushID(i);
			if( ImGui::ImageButton((void*)(intptr_t)pTpage->_iTextureID, *vDim, *vUV0, *vUV1, 1) )
			{
					s_bSpriteSelected = true;
					s_iTileIndex = i;
			}
			ImGui::PopID();	// Note: PushID required for image buttons to work

			// New line?
			if (iPixelsUsed < s_iRowPixels) ImGui::SameLine(); else iPixelsUsed = 0;
		}


		// Bounds check mouse to this window. We'll hide the sprite while we're over the window. 
		bool bMouseOverWindow = false;
		{
			static ImVec2 s_pWindowDim = ImGui::GetWindowSize();
			static ImVec2 s_pWindowPos = ImGui::GetWindowPos();
			ImGui::Text("[%.1f, %.1f]", s_pWindowDim.x, s_pWindowDim.y);
			ImGui::Text("[%.1f, %.1f]", s_pWindowPos.x, s_pWindowPos.y);

			if ((pMouseCoords->x >= s_pWindowPos.x) && (pMouseCoords->y >= s_pWindowPos.y) &&
				(pMouseCoords->x <= (s_pWindowPos.x + s_pWindowDim.x)) && (pMouseCoords->y <= (s_pWindowPos.y + s_pWindowDim.y)))
				bMouseOverWindow = true;
		}


		// Set Sprite correct location, nearest the mouse pointer 
		if(s_bSpriteSelected && !bMouseOverWindow)
		{
			// ESC key will turn off the sprite selection next Tick
			s_bSpriteSelected = !Neutrino::GetKeyState(Neutrino::eKeyboard_EditorInputs::_ESC);

			s_pSelectedTile = Neutrino::NewSprite(pTpage->_iTextureID, s_iTileIndex);
			ASSERT(NULL != s_pSelectedTile, "NewSprite returned null for tile: %d on texture: %d", s_iTileIndex, pTpage->_iTextureID);
			s_pSelectedTile->_vPosition->x = pMouseCoords->x;
			s_pSelectedTile->_vPosition->y = pMouseCoords->y;

			// Update for the grid setting
			if (s_bSnapToGrid)
			{
				float fGridSize = (float)pow(2, s_iSelectedGridSize);
				s_pSelectedTile->_vPosition->x -= ((int)(pMouseCoords->x) % (int)fGridSize);
				s_pSelectedTile->_vPosition->y -= ((int)(pMouseCoords->y) % (int)fGridSize);
				s_pSelectedTile->_vPosition->x += fGridSize * 0.5f;
				s_pSelectedTile->_vPosition->y += fGridSize * 0.5f;
			}
		}
		
		DELETEX vDim;
		DELETEX vUV0;
		DELETEX vUV1;
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


