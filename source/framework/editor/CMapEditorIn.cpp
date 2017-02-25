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
#include "../tinydir/tinydir.h"

using namespace Neutrino;

#if defined _WIN32
#define sprintf sprintf_s
#endif

static const ImVec2* s_pWindowPosition = NEWX ImVec2(1450.0f, 50.0f);
static const float s_iRowPixels = 400.0f;
static bool s_bSnapToGrid = true;
static int s_iSelectedTexture = 0;
static int s_iSelectedFile = 0;
static int s_iSelectedGridSize = 4;
static Sprite_t* s_pSelectedTile;
static uint16 s_iTileIndex = 0;
static bool s_bSpriteSelected;
static bool s_bLevelNameEmpty = true;
// Level Details statics
static const int s_iFilenameLength = 64;
static const int s_iFilepathLength = 1024;
static int s_iScreenWidth = 480;
static int s_iScreenHeight = 270;
static int s_iScreensWide = 8;
static int s_iLevelWidth = s_iScreenWidth / 32;
static int s_iLevelHeight = s_iScreenHeight / 32;
static bool s_bLevelCreated = false;
static bool s_bFilepathValid = false;
static char s_pFilenameBuf[s_iFilenameLength] = "\0";
static std::vector<std::string> s_aFileList;

#if defined _WIN32 
static char s_pFilepathBuf[s_iFilepathLength] = "E:\\BitBucket\\Neutrino\\resources\\tilemaps\\\0";
#else
static char s_pFilepathBuf[s_iFilepathLength] = "/home/kor/Development/Neutrino/resources/tilemaps\0";
#endif 

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
	int16 _iPrevContent;
	int16 _iNewContent;
	eCommandType _iAction;
}Command_t;
typedef std::vector<Command_t*> CommandList;

// Vector of all actions the editor has performed. Move through this vector
// To replay / undo editor actions...
// 
static CommandList s_aCommandList;
static int s_iCommandListIndex = 0;


// This array holds the Tilemap values. Just records the sprite index. 
// TODO: This needs to be part of a struct that contains the TPage_t info
static std::vector<int16> s_aTileMap;

//-------------------------------------------------------------------------------------------------------------------------------

static void RenderEmptyTile(const float iXPos, const float iYPos)
{
	UntexturedSprite_t* pSprite = NewDebugSprite();
	*pSprite->_fHalfHeight = 4.0f;
	*pSprite->_fHalfWidth = 1.0f;
	*(pSprite->_vPosition) = glm::vec3((float)iXPos + 0.35f, (float)iYPos + 0.35f, 1.0f);
	*(pSprite->_vColour) = glm::vec4(1.0f, 1.0f, 1.0f, 1);

	pSprite = NewDebugSprite();
	*pSprite->_fHalfHeight = 1.0f;
	*pSprite->_fHalfWidth = 4.0f;
	*(pSprite->_vPosition) = glm::vec3((float)iXPos + 0.35f, (float)iYPos + 0.35f, 1.0f);
	*(pSprite->_vColour) = glm::vec4(1.0f, 1.0f, 1.0f, 1);
}

static void RenderTile(const float fXPos, const float fYPos, const GLuint iTextureID, const uint16 iSprIndex)
{
	Sprite_t * pSprite = NewSprite(iTextureID, iSprIndex);
	*(pSprite->_vPosition) = glm::vec3(fXPos, fYPos, 1.0f);
	*(pSprite->_vColour) = glm::vec4(1.0f, 1.0f, 1.0f, 1);
}

static void ResizeTilemap()
{
	uint32 iSize = (uint32)s_aTileMap.size();
	s_aTileMap.resize(s_iLevelHeight*s_iLevelWidth);
	if (s_aTileMap.size() > iSize)
	{
		for (int i = iSize; i < s_iLevelHeight * s_iLevelWidth; ++i)
		{
			s_aTileMap[i] = -1;
		}
	}
}

static void CheckFilepath()
{
	tinydir_dir dir;
	if (tinydir_open_sorted(&dir, s_pFilepathBuf) == -1)
	{
		LOG_ERROR("Filepath invalid: %s", s_pFilepathBuf);
		s_bFilepathValid = false;
	}
	else
	{
		LOG_INFO("Filepath is valid: %s", s_pFilepathBuf);
		LOG_INFO("Directory contains %d files...", dir.n_files);

		s_aFileList.clear();
		for(uint i = 0; i < dir.n_files; ++i)
		{
			tinydir_file file;
			if (tinydir_readfile_n(&dir, &file, i) == -1)
			{
				LOG_ERROR("Error getting file...");
			}
			else if(strcmp(file.name,".") != 0  && strcmp(file.name, "..") != 0 && strcmp(file.extension, ".ltdi")) 
			{
				// TODO: this needs to be fixed to a C_str array, rather than standard string...
				std::string sName(file.name); 
				sName.push_back('\0');
				s_aFileList.push_back(sName);
			}
		}
		s_bFilepathValid = true;
		LOG_INFO("%d of those files have the .ltdi file extension and have been added to the drop down list.", s_aFileList.size());
	}
}


//-------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------- 

CMapEditorIn::CMapEditorIn()
{
	m_pStateName = "Map Editor In";
	LOG_INFO(">>>> %s", this->m_pStateName);
	CheckFilepath();
}


CMapEditorIn::~CMapEditorIn()
{

}


void CMapEditorIn::Init()
{
	// Do any cleanup from previous frame here. 
	// Note: we'll be skipping the Exit state of where-ever we are (MenuExit, LevelExit etc) so this could get hairy...
	ResetSpriteCount();
	GLUtils::SetClearColour(0.0f, 0.05f, 0.0f, 1.0f);
}


void CMapEditorIn::Update()
{
	const glm::vec2* pMouseCoords = GetMouseCoords();
	const glm::vec3* pCamerCoords = GetFlyCamOffset();
	const ImGuiTreeNodeFlags iFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed;
	float fGridSize = (float)pow(2, s_iSelectedGridSize + 1);	// Plus one as the grid starts at 2x2

	// calculate the mouse position in world space
	glm::vec2* vMouseWorldPosition = NEWX glm::vec2(0.0f, 0.0f);
	const glm::vec2 vPixelScale = GLUtils::GetViewportPixelScale();
	vMouseWorldPosition->x = pMouseCoords->x - (pCamerCoords->x / vPixelScale.x);
	vMouseWorldPosition->y = pMouseCoords->y - (pCamerCoords->y / vPixelScale.y);

	// Alert user if the filepath is invalid
	if (!s_bFilepathValid)
	{
		ImGui::OpenPopup("filepath");
		if (ImGui::BeginPopupModal("filepath", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("The file path for saving / loading tile maps is invalid!\n\n\"%s\"\n\nPlease enter a valid path below:\n\n", s_pFilepathBuf);
			ImGui::PushItemWidth(340); 
			ImGui::InputText("Filepath", s_pFilepathBuf, s_iFilepathLength, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll);

			if (ImGui::Button("OK", ImVec2(400,0))) 
			{ 
				CheckFilepath();
				ImGui::CloseCurrentPopup(); 
			}
			ImGui::PopItemWidth();
			ImGui::EndPopup();
		}
	}

	// Window Heading
	{
		ImGui::SetNextWindowPos(*s_pWindowPosition, ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(s_iRowPixels, 750.0f), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Tilemap Editor");
		ImGui::Text("Mouse Position: [%.1f, %.1f]", pMouseCoords->x, pMouseCoords->y);
		ImGui::Text(" Camera Offset: [%.1f, %.1f]", pCamerCoords->x, pCamerCoords->y);
		ImGui::Text("World Position: [%.1f, %.1f]", vMouseWorldPosition->x, vMouseWorldPosition->y);
		ImGui::Spacing();
	}


	// Define the settings for this level
	{
		if (ImGui::CollapsingHeader("Level Details:", iFlags))
		{
			ImGui::InputText("Filepath", s_pFilepathBuf, s_iFilepathLength, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll);
			ImGui::InputText("New Level Name", s_pFilenameBuf, s_iFilenameLength, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll);
			std::string sBuff;
			for(uint i = 0; i < s_aFileList.size(); ++i)
				sBuff += s_aFileList[i];
			ImGui::Combo("Existing Level", &s_iSelectedFile, sBuff.c_str(), (int)s_aFileList.size());
			ImGui::InputInt("Screen Width", &s_iScreenWidth, 1, 5); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Pixel width of the screen for the final output");
			ImGui::InputInt("Screen Height", &s_iScreenHeight, 1, 5); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Pixel height of the screen for the final output");
			ImGui::InputInt("Screens Wide", &s_iScreensWide, 1, 5); if (ImGui::IsItemHovered()) ImGui::SetTooltip("How many screens long is this level?");
			bool bWidthChanged = ImGui::InputInt("Level Width", &s_iLevelWidth, 1, 5); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Width of the level, in tiles");
			bool bHeightChanged = ImGui::InputInt("Level Height", &s_iLevelHeight, 1, 5); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Height of the level, in tiles");
			if (bWidthChanged || bHeightChanged) ResizeTilemap();
		}
	}


	// Respond to "New Level" button being pressed
	{
		if (ImGui::Button("Create"))
		{
			if (s_pFilenameBuf[0] == '\0')
			{
				ImGui::OpenPopup("levelname");
			}
			else
			{
				s_bLevelNameEmpty = false;
				LOG_INFO("Generating new level...");
				s_aTileMap.clear();
				for (int i = 0; i < (s_iLevelWidth *s_iLevelHeight); ++i)
				{
					s_aTileMap.push_back(-1);
				}
				s_bLevelCreated = true;
			}
		}

		if (ImGui::BeginPopupModal("levelname", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("You must enter a name for the new level in the properties window!");
			if (ImGui::Button("OK", ImVec2(460,0))) 
			{ 
				ImGui::CloseCurrentPopup(); 
			}
			ImGui::EndPopup();
		}
	}


	// Respond to "Clear Level" button being pressed
	{
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			s_aTileMap.clear();
			s_bLevelCreated = false;
		}
	}


	// Respond to "Calc Dimensions" button
	{
		ImGui::SameLine();
		if (ImGui::Button("Calc") && !s_bLevelCreated)
		{
			s_iLevelHeight = s_iScreenHeight / (int)fGridSize;
			s_iLevelWidth = (s_iScreenWidth*s_iScreensWide) / (int)fGridSize;
			if (s_iScreenHeight % (int)fGridSize > 0) LOG_WARNING("Level Height dimensions are %d pixels short", s_iScreenHeight % (int)fGridSize);
			if ((s_iScreenWidth*s_iScreensWide) % (int)fGridSize > 0) LOG_WARNING("Level Width dimensions are %d pixels short", s_iScreenHeight % (int)fGridSize);
		}
	}


	// Add a button for loading a file from the drop down 
	{
		ImGui::SameLine(200);
		if (ImGui::Button("Load"))
		{
		}
	}



	// Add a button for saving the current level 
	{
		ImGui::SameLine();
		if (ImGui::Button("Save") && s_bLevelCreated)
		{
		}
	}



	// Display Grid Settings 
	{
		if (ImGui::CollapsingHeader("Editor Grid Settings:", iFlags))
		{
			ImGui::PushItemWidth(150);
			ImGui::Combo("Grid Size", &s_iSelectedGridSize, " 2x2 \0 4x4 \0 8x8 \0 16x16 \0 32x32 \0 64x64 \0 128x128 \0", -1);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Checkbox("Snap To Grid", &s_bSnapToGrid);
			ImGui::Spacing();
		}
	}


	// Build a string of Texture Identifiers for a drop down list
	{
		uint8 iTextureCount = GetLoadedTextureCount();
		if (ImGui::CollapsingHeader("Editor Texture Settings:", iFlags))
		{
			ImGui::Text("Available Textures:"); ImGui::SameLine(165); ImGui::Text("%d", iTextureCount);
			std::string sBuff;
			for (int i = 0; i < iTextureCount; ++i)
				sBuff += " Texture: " + std::to_string(i) + " \0";

			ImGui::Combo("Select Texture", &s_iSelectedTexture, sBuff.c_str(), iTextureCount);
			ImGui::Spacing();
		}
	}


	// Display selected texture info
	const TPage_t* pTpage = GetTPage((uint8)s_iSelectedTexture);	
	{
		if (ImGui::CollapsingHeader("Available Tiles", iFlags))
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
				// The intptr_t cast removes the warning. 
				ImGui::PushID(i);
				if (ImGui::ImageButton((void*)(intptr_t)pTpage->_iTextureID, *vDim, *vUV0, *vUV1, 1))
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
	}

	


	// Bounds check mouse to this window. We'll hide the sprite while we're over the window. 
	bool bMouseOverWindow = false; 
	{
		ImVec2 s_pWindowDim = ImGui::GetWindowSize();
		ImVec2 s_pWindowPos = ImGui::GetWindowPos();

		if ((pMouseCoords->x >= s_pWindowPos.x) && (pMouseCoords->y >= s_pWindowPos.y) &&
			  (pMouseCoords->x <= (s_pWindowPos.x + s_pWindowDim.x)) && (pMouseCoords->y <= (s_pWindowPos.y + s_pWindowDim.y))) 
			bMouseOverWindow = true;

		ImGui::End();
	}

	// Set Sprite Location, to nearest grid if set
	// NOTE: Currently we only save tiles if Snap To Grid is set ;D
	{
		if (s_bSpriteSelected && !bMouseOverWindow)
		{
			// ESC key will turn off the sprite selection next Tick
			s_bSpriteSelected = !GetKeyState(eKeyboard_EditorInputs::_ESC);

			s_pSelectedTile = NewSprite(pTpage->_iTextureID, s_iTileIndex);
			ASSERT(NULL != s_pSelectedTile, "NewSprite returned null for tile: %d on texture: %d", s_iTileIndex, pTpage->_iTextureID);

			s_pSelectedTile->_vPosition->x = vMouseWorldPosition->x;
			s_pSelectedTile->_vPosition->y = vMouseWorldPosition->y;

			// Update for the grid setting
			if (s_bSnapToGrid)
			{
				s_pSelectedTile->_vPosition->x -= (float)((int)(vMouseWorldPosition->x) % (int)fGridSize);
				s_pSelectedTile->_vPosition->y -= (float)((int)(vMouseWorldPosition->y) % (int)fGridSize);
				s_pSelectedTile->_vPosition->x += fGridSize * 0.5f;
				s_pSelectedTile->_vPosition->y += fGridSize * 0.5f;
			}
		}
	}


	// If we have an active level to edit, iterate over tile positions and render either the tile or the empty tile indicator
	{
		if (s_bLevelCreated)
		{
			float fXPos = 0.0f;
			float fYPos = 0.0f;
			float fHalfGridSize = fGridSize*0.5f;

			for (uint16 i = 0; i < s_iLevelHeight; ++i)
			{
				for (uint16 j = 0; j < s_iLevelWidth; ++j)
				{
					// Render the tile
					ASSERT((uint32)(i*j) < (uint32)s_aTileMap.size(), "s_aTilemapArray hasn't been resized for current dimensions");
					if (s_aTileMap[(i*s_iLevelWidth)+j] == -1)
						RenderEmptyTile(fXPos + fHalfGridSize, fYPos + fHalfGridSize);
					else
						RenderTile(fXPos + fHalfGridSize, fYPos + fHalfGridSize, pTpage->_iTextureID, s_aTileMap[(i*s_iLevelWidth)+j]);
					fXPos += fGridSize;
				}
				fXPos = 0;
				fYPos += fGridSize;
			}
		}
	}


	// Respond to Mouse Down event and Add/Remove a tile to the map....
	// NOTE: See above, we're only saving tiles in grid mode. 
	static bool bDebounceMouse = false;
	if (!bMouseOverWindow && !bDebounceMouse)
	{
		int iTilesX = (int)(vMouseWorldPosition->x / fGridSize);
		int iTilesY = (int)(vMouseWorldPosition->y / fGridSize);
		int iIndex = (iTilesY * s_iLevelWidth) + iTilesX;
		bool bMouseOverMapArea = (vMouseWorldPosition->x >= 0.0f && vMouseWorldPosition->x <= ((float)s_iLevelWidth * fGridSize)) &&
			                       (vMouseWorldPosition->y >= 0.0f && vMouseWorldPosition->y <= ((float)s_iLevelHeight * fGridSize));

		// Insert a tile...
		if (GetMouseLB())
		{
			if (bMouseOverMapArea && s_bSpriteSelected && s_bSnapToGrid)
			{
			  bDebounceMouse = true;
				Command_t* pNewCommand = NULL;
				if( s_iCommandListIndex == (int)s_aCommandList.size())
				{
					pNewCommand = NEWX(Command_t);
					s_aCommandList.push_back(pNewCommand);
				}
				else
				{
					pNewCommand = s_aCommandList[s_iCommandListIndex];
					for(uint i = s_iCommandListIndex+1; i < s_aCommandList.size(); ++i)
						DELETEX(s_aCommandList[i]);
					s_aCommandList.resize(s_iCommandListIndex+1);
				}
				pNewCommand->_iAction = eCommandType::_TileAdd;
				pNewCommand->_iNewContent = s_iTileIndex;
				pNewCommand->_iPrevContent = s_aTileMap[iIndex];
				pNewCommand->_iTilemapIndex = (uint16)iIndex;
				++s_iCommandListIndex;
				s_aTileMap[iIndex] = s_iTileIndex;
			}
		}

		// Remove a Tile
		if (GetMouseRB())
		{
			if (bMouseOverMapArea && s_bSnapToGrid && s_aTileMap[iIndex] != -1)
			{
				bDebounceMouse = true;
				Command_t* pNewCommand = NULL;
				if( s_iCommandListIndex == (int)s_aCommandList.size())
				{
					pNewCommand = NEWX(Command_t);
					s_aCommandList.push_back(pNewCommand);
				}
				else
				{
					pNewCommand = s_aCommandList[s_iCommandListIndex];
					for(uint i = s_iCommandListIndex+1; i < s_aCommandList.size(); ++i)
						DELETEX(s_aCommandList[i]);
					s_aCommandList.resize(s_iCommandListIndex+1);
				}
				pNewCommand->_iAction = eCommandType::_TileRem;
				pNewCommand->_iNewContent = -1;
				pNewCommand->_iPrevContent = s_aTileMap[iIndex];
				pNewCommand->_iTilemapIndex = (uint16)iIndex;
				++s_iCommandListIndex;
				s_aTileMap[iIndex] = -1;
			}
		}
	}

	// Clear bDebounceMouse
	if( bDebounceMouse && !GetMouseLB() && !GetMouseRB() ) bDebounceMouse = false;


	// Respond to Undo / Redo
	static bool bDebounceKey= false;
	{
		if( !bDebounceKey )
		{
			if(GetKeyState(eKeyboard_EditorInputs::_UNDO) && s_iCommandListIndex > 0 )
			{
				Command_t* pCmd = s_aCommandList[s_iCommandListIndex-1];
				s_aTileMap[pCmd->_iTilemapIndex] = pCmd->_iPrevContent; 
				--s_iCommandListIndex;
				bDebounceKey = true;
			}

			if(GetKeyState(eKeyboard_EditorInputs::_REDO) && s_iCommandListIndex < (int)s_aCommandList.size())
			{
				Command_t* pCmd = s_aCommandList[s_iCommandListIndex];
				s_aTileMap[pCmd->_iTilemapIndex] = pCmd->_iNewContent; 
				++s_iCommandListIndex;
				bDebounceKey=true;
			}
		}
	}

	if( bDebounceKey && !GetKeyState(eKeyboard_EditorInputs::_UNDO) && !GetKeyState(eKeyboard_EditorInputs::_REDO) ) bDebounceKey = false;

	DELETEX vMouseWorldPosition;
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


