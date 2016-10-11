#include "Debug.h"
#include "imgui/imgui.h"
#include "Memory.h"

namespace Neutrino
{
	static bool s_bDebugOverlayActive = true;
	static const ImVec2* s_pOverlayPosition = NEWX ImVec2(50.0f, 50.0f);

	void ToggleDebugOverlay()
	{
		s_bDebugOverlayActive = !s_bDebugOverlayActive;
	}

	void DrawDebugOverlay()
	{
		if (s_bDebugOverlayActive)
        {
        	ImGui::SetNextWindowPos(*s_pOverlayPosition, ImGuiSetCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Debug Overlay", &s_bDebugOverlayActive, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::Text("Hello");
            ImGui::End();
        }
	}	
}