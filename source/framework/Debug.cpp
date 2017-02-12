#include "Debug.h"
#include "Time.h"
#include "Memory.h"
#include "Sprite.h"
#include "GLUtils.h"
#include <string.h>
#include <glm/glm.hpp> 
#include "Input.h"

namespace Neutrino
{
	static bool s_bDebugOverlayActive = true;
	static bool s_bOutputLogActive = true;

	static const ImVec2* s_pOverlayPosition = NEWX ImVec2(50.0f, 50.0f);
	static const ImVec2* s_pLogPosition = NEWX ImVec2(1100.0f, 850.0f);

	static CFrameRate s_FPSSampler;
	static DebugLog* s_pDebugLog;

	static char sFPS_Text[32] = {0};
	static char sDELTA_Text[32] = {0};


	// Struct to hold all the params for the default debug overlay. 
	// Suspect this will grow arms and legs, but see how it goes...
	// 
	typedef struct DebugOverlay_t
	{
		glm::vec3*	_pvCameraOffset;
		bool _bFlyCameraMode = false;
		float _fFlyCamSpeed = 0.5f;
	} DebugOverlay_t;

	static DebugOverlay_t* s_pOverlayParams;

	
	// ------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------
	
	
	
	DebugLog* DebugOverlayInit()
	{
		s_pOverlayParams = NEWX(DebugOverlay_t);
		s_pOverlayParams->_pvCameraOffset = NEWX glm::vec3(0.0f, 0.0f, 0.0f);
		
		// Log must be created after ImGUI init, or there'll be no context and buffer memory can't be allocated
		static DebugLog log;
		s_pDebugLog = &log;
		return s_pDebugLog;
	}


	void DebugOverlayKill()
	{
		DELETEX s_pOverlayParams->_pvCameraOffset;
		DELETEX s_pOverlayParams;	
	}


	void ToggleDebugOverlay()
	{
		s_bDebugOverlayActive = !s_bDebugOverlayActive;
	}

	glm::vec3* GetFlyCamOffset()
	{
		ASSERT(s_pOverlayParams, "Attempted to get FlyCamOffset before Debug Overlay Params have been initialised");
		return s_pOverlayParams->_pvCameraOffset;
	}

	void DebugOverlayUpdate()
	{
		const ImGuiTreeNodeFlags iFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed;
		//float fFPS = (float)floorf( 1.0f / (float)fmax(fEPSILON, (float)fAvgFPS )+0.5f );
		s_FPSSampler.Tick(GetMSDelta());
	
		if (s_bDebugOverlayActive)
		{
			// If fly cam is enabled, grab player 2's input and move the camera.
			if (s_pOverlayParams->_bFlyCameraMode)
			{
				glm::vec3* vTraj = GetInputAxisGameDeltaScaled(1);
				vTraj->x = -vTraj->x;
				vTraj->y = -vTraj->y;
				*s_pOverlayParams->_pvCameraOffset += *vTraj * s_pOverlayParams->_fFlyCamSpeed;
			}

			// Update performance counters
			snprintf(sFPS_Text, 32,   "FPS: %.2f [%.2f]", s_FPSSampler.GetAverageFrameRate(), s_FPSSampler.GetLatestFrameRate() );
			snprintf(sDELTA_Text, 32, "DELTA: [%.3f]", s_FPSSampler.GetAverageDeltaTime());

			ImGui::SetNextWindowPos(*s_pOverlayPosition, ImGuiSetCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(300,200), ImGuiSetCond_FirstUseEver);
			ImGui::Begin("Neutrino Debug", &s_bDebugOverlayActive);
			if (ImGui::CollapsingHeader("Performance Counters:", iFlags))
			{
				ImGui::PlotLines(sDELTA_Text, s_FPSSampler.GetSamples(), (int)s_FPSSampler.GetSampleSize(), (int)s_FPSSampler.GetSampleOffset(), "", 1.0f / 120.0f, 1.0f / 24.0f, ImVec2(150, 32));
				ImGui::PlotLines(sFPS_Text, s_FPSSampler.GetFPS(), (int)s_FPSSampler.GetSampleSize(), (int)s_FPSSampler.GetSampleOffset(), "", 0.0f, 120.0f, ImVec2(150, 32));
				ImGui::Text("T Sprites: %d", GetSpriteCount()); ImGui::SameLine(150); ImGui::Text("VBO0: %dk", (GetSpriteCount() * sizeof(GLUtils::Vertex_t)) / 1024);
				ImGui::Text("U Sprites: %d", GetDebugSpriteCount()); ImGui::SameLine(150); ImGui::Text("VBO1: %dk", (GetDebugSpriteCount() * sizeof(GLUtils::Vertex_t)) / 1024);
			}

			// Output camera position
			if (ImGui::CollapsingHeader("Camera:", iFlags))
			{
				ImGui::Text("Camera Position: [%.1f,%.1f,%.1f] ", s_pOverlayParams->_pvCameraOffset->x, s_pOverlayParams->_pvCameraOffset->y, s_pOverlayParams->_pvCameraOffset->z);
				ImGui::Checkbox("Fly Cam", &s_pOverlayParams->_bFlyCameraMode);
				ImGui::SameLine(95);
				ImGui::SliderFloat("", &s_pOverlayParams->_fFlyCamSpeed, 0.0f, 3.0f, "Fly Cam Speed: %.1f");
				if (ImGui::Button("Reset Fly Cam", ImVec2(292, 18)))
				{
					DELETEX s_pOverlayParams->_pvCameraOffset;
					s_pOverlayParams->_pvCameraOffset = NEWX glm::vec3(0.0f, 0.0f, 0.0f);
					s_pOverlayParams->_bFlyCameraMode = false;
				}
			}

			ImGui::End();

			// Draw the log window
			s_pDebugLog->Draw("Output Log", &s_bOutputLogActive, s_pLogPosition);
		}
	}	
};
