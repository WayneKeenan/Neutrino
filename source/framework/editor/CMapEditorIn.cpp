#include "CMapEditorIn.h"
#include "../GameStateManager.h"
#include "../Sprite.h"
#include "../GLUtils.h"
#include "../imgui/imgui.h"
#include "../Texture.h"
#include "../Memory.h"
#include "../Input.h"
#include "../Debug.h"
#include <string>
#include <math.h>
#include <glm/glm.hpp> 
#include <vector>


#if defined _WIN32
#define sprintf sprintf_s
#endif

static const ImVec2* s_pWindowPosition = NEWX ImVec2(1300.0f, 50.0f);
static const float s_iRowPixels = 400.0f;
static bool s_bSnapToGrid = true;
static int s_iSelectedTexture = 0;
static int s_iSelectedGridSize = 4;
static Neutrino::Sprite_t* s_pSelectedTile;
static uint16 s_iTileIndex = 0;
static bool s_bSpriteSelected;

// Level Details statics
static const int s_iFilenameLength = 64;
static int s_iLevelWidth = 180 / 32;
static int s_iLevelHeight = 320 / 32;
static bool s_bCentreLevel = false;


// Command type enumerates the actions the editor can perform
enum eCommandType
{
	_TileAdd,
	_TileRem,
};

// Each action will generate a Command_t action that's stored in a vector. 
// We can use this to Undo/Redo editor actions...
// 
// _iTilemapIndex = Where did this command happen?
// _iPrevContent  = Previous contents of the tile array at this index
// _iNewContent   = New tile to be stored at this index, if any
// _iAction		  = What action did this command perform?
// 
typedef struct Command_t
{
	uint16 _iTilemapIndex;
	uint16 _iPrevContent;
	uint16 _iNewContent;
	eCommandType _iAction;
}Command_t;
typedef std::vector<Command_t*> CommandList;

// Vector of all actions the editor has performed. Move through this vector
// To replay / undo editor actions...
// 
static CommandList s_aCommandList;


// This array holds the Tilemap values. Just records the sprite index. 
// TODO: This needs to be part of a struct that contains the TPage_t info
static std::vector<uint16> s_aTileMapIndex;

//-------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------- 

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
	const glm::vec2* pMouseCoords = Neutrino::GetMouseCoords();
	const glm::vec3* pCamerCoords = Neutrino::GetFlyCamOffset();
	const ImGuiTreeNodeFlags iFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed;

	// Draw the window
	ImGui::SetNextWindowPos(*s_pWindowPosition, ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(s_iRowPixels, 250.0f), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Tilemap Editor");
	ImGui::Text("Mouse Position: [%.1f, %.1f]", pMouseCoords->x, pMouseCoords->y);
	ImGui::Text(" Camera Offset: [%.1f, %.1f]\n", pCamerCoords->x, pCamerCoords->y);
	ImGui::Text("");

	// Define the settings for this level
	static char s_FilenameBuf[s_iFilenameLength] = "\0";
	if (ImGui::CollapsingHeader("Level Details:", iFlags))
	{
		ImGui::TextWrapped("To generate a new level enter a filename, dimensions (in tiles) and click \"New\". To save your changes click \"Save\".");
		ImGui::InputText("Level Name", s_FilenameBuf, s_iFilenameLength, ImGuiInputTextFlags_CharsNoBlank|ImGuiInputTextFlags_AutoSelectAll);
		ImGui::InputInt("Level Width", &s_iLevelWidth, 1, 5);
		ImGui::InputInt("Level Height", &s_iLevelHeight, 1, 5);
		ImGui::Checkbox("Centre Level", &s_bCentreLevel);
	}

	// Respond to "New Level" button being pressed
	if (ImGui::Button("New Level") )
	{
		if (s_FilenameBuf[0] == '\0') 
			LOG_ERROR("Filename buffer is empty, unable to generate a new level without a filename being set");
		else
		{
			LOG_INFO("Generating new level...");
			s_aTileMapIndex.clear();
		}
	}

	// Display Grid Settings 
	if (ImGui::CollapsingHeader("Editor Grid Settings:", iFlags))
	{
		ImGui::PushItemWidth(150);
		ImGui::Combo("Grid Size", &s_iSelectedGridSize, " 2x2 \0 4x4 \0 8x8 \0 16x16 \0 32x32 \0 64x64 \0 128x128 \0", -1); 
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Checkbox("Snap To Grid", &s_bSnapToGrid); 
		ImGui::Text("");
	}

	// Build a string of Texture Identifiers for a drop down list
	static uint8 iTextureCount = Neutrino::GetLoadedTextureCount();
	if (ImGui::CollapsingHeader("Editor Texture Settings:", iFlags))
	{
		ImGui::Text("Available Textures:"); ImGui::SameLine(165); ImGui::Text("%d", iTextureCount);		
		std::string sBuff;
		for (int i = 0; i < iTextureCount; ++i)
			sBuff += " Texture: " + std::to_string(i) + " \0";

		ImGui::Combo("Select Texture", &s_iSelectedTexture, sBuff.c_str(), iTextureCount);
		ImGui::Text("");
	}

	// Display selected texture info
	static const Neutrino::TPage_t* pTpage = Neutrino::GetTPage((uint8)s_iSelectedTexture);	
	if(ImGui::CollapsingHeader("Available Tiles", iFlags))
	{
		ImVec2* vDim = NEWX ImVec2(0.0f, 0.0f);
		ImVec2* vUV0 = NEWX ImVec2(0.0f, 0.0f);
		ImVec2* vUV1 = NEWX ImVec2(0.0f, 0.0f);

		float iPixelsUsed = 0.0f;
		for (uint16 i = 0; i < pTpage->_iMaxSprites; ++i)
		{
			// Set params ImageButton needs
			vDim->x = pTpage->aSprintInfo[i]._fHalfWidth*2.0f;
			vDim->y = pTpage->aSprintInfo[i]._fHalfHeight*2.0f;
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
		
		DELETEX vDim;
		DELETEX vUV0;
		DELETEX vUV1;
	}


	// Bounds check mouse to this window. We'll hide the sprite while we're over the window. 
	bool bMouseOverWindow = false; 
	{
		static ImVec2 s_pWindowDim = ImGui::GetWindowSize();
		static ImVec2 s_pWindowPos = ImGui::GetWindowPos();
		// 		ImGui::Text("[%.1f, %.1f]", s_pWindowDim.x, s_pWindowDim.y);
		// 		ImGui::Text("[%.1f, %.1f]", s_pWindowPos.x, s_pWindowPos.y);

		if ((pMouseCoords->x >= s_pWindowPos.x) && (pMouseCoords->y >= s_pWindowPos.y) &&
			(pMouseCoords->x <= (s_pWindowPos.x + s_pWindowDim.x)) && (pMouseCoords->y <= (s_pWindowPos.y + s_pWindowDim.y)))
			bMouseOverWindow = true;
	}


	// Set Sprite correct location, nearest the mouse pointer 
	if (s_bSpriteSelected && !bMouseOverWindow)
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
			float fGridSize = (float)pow(2, s_iSelectedGridSize+1);	// Plus one as the grid starts at 2x2
			s_pSelectedTile->_vPosition->x -= ((int)(pMouseCoords->x) % (int)fGridSize);
			s_pSelectedTile->_vPosition->y -= ((int)(pMouseCoords->y) % (int)fGridSize);
			s_pSelectedTile->_vPosition->x += fGridSize * 0.5f;
			s_pSelectedTile->_vPosition->y += fGridSize * 0.5f;
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


