#pragma once

#include "sdl/sdl_wrapper.h"
#include "Log.h"
#include "Types.h"
#include "Assert.h"
#include "Memory.h"
#include "Time.h"
#include "GL/glew.h"
#include "GLUtils.h"
#include "Sprite.h"
#include "File.h"
#include "ShaderUtils.h"
#include "GameStateManager.h"
#include "../game/global/CGameGlobals.h"
#include "Texture.h"
#include "Input.h"
#include "Level.h"
#include "ConfigFile.h"
#include "Level.h"

namespace Neutrino {
	bool CoreInit(const char* const pGameName);
	bool CoreUpdate();
	bool CoreKill();
	void EnterEditorMode();
}


