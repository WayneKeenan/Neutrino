#include "CMapEditorIn.h"
#include "../GameStateManager.h"
#include "../Sprite.h"
#include "../GLUtils.h"

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
	// Do window here...

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


