#pragma once
#include "Types.h"
#include <array>
#include "Assert.h"
#include <glm/glm.hpp> 
#include "imgui/imgui.h"
#include "FPSCounter.h"
#include "DebugOutputLog.h"

namespace Neutrino
{
	
	// DebugOverlayInit()
	// 		Creates the struct holding overlay params and sets defaults. Returns a pointer to the static ImGUI 
	// 		debug log overlay. Called by GetSystemLog()
	DebugLog* DebugOverlayInit();

	// ToggleDebugOverlay()
	// 		Turn the overlay on / off. No formal key assigned to this, although SDL wrapper currently checks for F1	
	void ToggleDebugOverlay();

	//	DrawDebugOverlay()
	// 		Creates and new ImGUI window and does all the processing necessary for our debug output. 
	void DebugOverlayUpdate();

	// DebugOverlayKill()
	// 		Cleans up and removes param struct
	void DebugOverlayKill();


	// GetFlyCamOffset()
	// 		Returns the current vec3 position for the flycam, this will be an additive to the game camera position
	//   	when the framework generates new camera matrices for the frame. 
	glm::vec3* GetFlyCamOffset();
}
