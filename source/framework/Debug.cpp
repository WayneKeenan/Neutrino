#include "Debug.h"
#include "imgui/imgui.h"
#include "Memory.h"
#include "Time.h"
#include "Input.h"
#include <string.h>
#include "Input.h"


namespace Neutrino
{
	static bool s_bDebugOverlayActive = true;
	static const ImVec2* s_pOverlayPosition = NEWX ImVec2(50.0f, 50.0f);

	static CFrameRate s_FPSSampler;

	static char sFPS_Text[32] = {0};
	static char sDELTA_Text[32] = {0};

	void ToggleDebugOverlay()
	{
		s_bDebugOverlayActive = !s_bDebugOverlayActive;
	}

	void DrawDebugOverlay()
	{
		//float fFPS = (float)floorf( 1.0f / (float)fmax(fEPSILON, (float)fAvgFPS )+0.5f );
		s_FPSSampler.Tick(GetMSDelta());

		if (s_bDebugOverlayActive)
        {
        	snprintf(sFPS_Text, 32,   "FPS: %.2f [%.2f]", s_FPSSampler.GetAverageFrameRate(), s_FPSSampler.GetLatestFrameRate() );
        	snprintf(sDELTA_Text, 32, "DELTA: [%.2f]", s_FPSSampler.GetAverageDeltaTime());

        	ImGui::SetNextWindowPos(*s_pOverlayPosition, ImGuiSetCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(300,120), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Neutrino Debug", &s_bDebugOverlayActive);
            ImGui::Text("Performance:");
            ImGui::PlotLines(sDELTA_Text, s_FPSSampler.GetSamples(), (int)s_FPSSampler.GetSampleSize(), (int)s_FPSSampler.GetSampleOffset(), "", 0.0f, 0.16f, ImVec2(150,32));
            ImGui::PlotLines(sFPS_Text, s_FPSSampler.GetFPS(), (int)s_FPSSampler.GetSampleSize(), (int)s_FPSSampler.GetSampleOffset(), "", 30.0f, 60.0f, ImVec2(150,32));
            ImGui::End();
        }
	}	
}