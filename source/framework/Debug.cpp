#include "Debug.h"
#include "Time.h"
#include "Memory.h"
#include "Sprite.h"
#include "GLUtils.h"
#include "ShaderUtils.h"
#include <string.h>
#include <glm/glm.hpp> 
#include "Input.h"
#include "File.h"
#include "Physics.h"

#include "../game/global/CGameGlobals.h"

#if defined DEBUG
namespace Neutrino
{
	static bool s_bDebugOverlayActive = true;
	static bool s_bOutputLogActive = true;
	static bool s_bPostProcessActive = false;
	static bool s_bGameGlobalsActive = true;


	static const ImVec2* s_pOverlayPosition = NEWX ImVec2(37.0f, 10.0f);
	static const ImVec2* s_pLogPosition = NEWX ImVec2(1100.0f, 850.0f);
	static const ImVec2* s_pOverlayDimensions = NEWX ImVec2(315.0f, 310.0f);
	static const ImVec2* s_pPostProcessPosition = NEWX ImVec2(1520.0f, 10.0f);
	static const ImVec2* s_pPostProcessDimensions = NEWX ImVec2(380.0f, 380.0f);

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
		float _fFlyCamSpeed = 3.5f;
	} DebugOverlay_t;

	static DebugOverlay_t* s_pOverlayParams;


	// Filepath for the post process settings. 
	static const int s_iFilepathLength = 4096;
#if defined _WIN32 
	static char s_pFilepathBuf[s_iFilepathLength] = "E:\\BitBucket\\Neutrino\\resources\\\0";
#else
	static char s_pFilepathBuf[s_iFilepathLength] = "/home/kor/Development/Neutrino/resources/\0";
#endif 

	
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

	void ResetFlyCamOffset()
	{
		s_pOverlayParams->_pvCameraOffset->x = 0.0f; 
		s_pOverlayParams->_pvCameraOffset->y = 0.0f;
		s_pOverlayParams->_pvCameraOffset->z = 0.0f;
	}

	void DebugOverlayUpdate()
	{
		const ImGuiTreeNodeFlags iFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed;
		//float fFPS = (float)floorf( 1.0f / (float)fmax(fEPSILON, (float)fAvgFPS )+0.5f );
		s_FPSSampler.Tick(GetMSDelta());
		PostProcessSettings_t* pScanlineSettings = GetCRTSettings();

		if (s_bDebugOverlayActive)
		{
			// If fly cam is enabled, grab player 2's input and move the camera.
			if (s_pOverlayParams->_bFlyCameraMode)
			{
				glm::vec3* vTraj = GetInputAxisGameDeltaScaled(1);
				vTraj->x = vTraj->x;
				vTraj->y = -vTraj->y;
				*s_pOverlayParams->_pvCameraOffset += ((*vTraj) * s_pOverlayParams->_fFlyCamSpeed)*GetGameMSDelta();
			}

			// Update performance counters
			snprintf(sFPS_Text, 32, "FPS: %.2f [%.2f]", s_FPSSampler.GetAverageFrameRate(), s_FPSSampler.GetLatestFrameRate());
			snprintf(sDELTA_Text, 32, "DELTA: [%.3f]", s_FPSSampler.GetAverageDeltaTime());


			//
			// DRAW MAIN OVERLAY (Performance and toggles)
			//
			{
				ImGui::SetNextWindowPos(*s_pOverlayPosition, ImGuiSetCond_FirstUseEver);
				ImGui::SetNextWindowSize(*s_pOverlayDimensions, ImGuiSetCond_FirstUseEver);
				ImGui::Begin("Neutrino Debug", &s_bDebugOverlayActive);
				if (ImGui::CollapsingHeader("Performance Counters:", iFlags))
				{
					ImGui::PlotLines(sDELTA_Text, s_FPSSampler.GetSamples(), (int)s_FPSSampler.GetSampleSize(), (int)s_FPSSampler.GetSampleOffset(), "", 1.0f / 120.0f, 1.0f / 24.0f, ImVec2(150, 32));
					ImGui::PlotLines(sFPS_Text, s_FPSSampler.GetFPS(), (int)s_FPSSampler.GetSampleSize(), (int)s_FPSSampler.GetSampleOffset(), "", 0.0f, 120.0f, ImVec2(150, 32));
					ImGui::Text("T Sprites: %d", GetSpriteCount()); ImGui::SameLine(150); ImGui::Text("VBO0: %dk", (int)(GetSpriteCount() * (sizeof(GLUtils::Vertex_t) * 6)) / 1024);
#if defined DEBUG 
					ImGui::Text("U Sprites: %d", GetDebugSpriteCount()); ImGui::SameLine(150); ImGui::Text("VBO1: %dk", (int)(GetDebugSpriteCount() * (sizeof(GLUtils::Vertex_t) * 6)) / 1024);
#endif
				}

				// Check for the flycam keyboard input
				static bool bDebounceKey = false;
				if (GetKeyState(eKeyboard_EditorInputs::_FLYCAM) && !bDebounceKey)
				{
					bDebounceKey = true;
					s_pOverlayParams->_bFlyCameraMode = !s_pOverlayParams->_bFlyCameraMode;
				}
				if (!GetKeyState(eKeyboard_EditorInputs::_FLYCAM) && bDebounceKey) bDebounceKey = false;

				// Output camera position
				if (ImGui::CollapsingHeader("Camera:", iFlags))
				{
					ImGui::Text("Camera Position: [%.1f,%.1f,%.1f] ", s_pOverlayParams->_pvCameraOffset->x, s_pOverlayParams->_pvCameraOffset->y, s_pOverlayParams->_pvCameraOffset->z);
					ImGui::Checkbox("Fly Cam", &s_pOverlayParams->_bFlyCameraMode);
					ImGui::SameLine(95);
					ImGui::SliderFloat("", &s_pOverlayParams->_fFlyCamSpeed, 0.0f, 24.0f, "Fly Cam Speed: %.1f");
					if (ImGui::Button("Reset Fly Cam", ImVec2(292, 18)))
					{
						DELETEX s_pOverlayParams->_pvCameraOffset;
						s_pOverlayParams->_pvCameraOffset = NEWX glm::vec3(0.0f, 0.0f, 0.0f);
						s_pOverlayParams->_bFlyCameraMode = false;
					}
				}

				// Output some toggle buttons
				if (ImGui::CollapsingHeader("Tweak Tools:", iFlags))
				{
					ImGui::Checkbox("Post Process Settings", &s_bPostProcessActive);
					ImGui::Checkbox("Game Globals Inspector", &s_bGameGlobalsActive);
				}

				// Toggles for debug draw 
				if (ImGui::CollapsingHeader("Debug Visualisers:", iFlags))
				{
					ImGui::Checkbox("Draw Physics World", GetPhysicsDebugDrawState());
				}

				ImGui::End();
			}

			//
			// Draw the post process settings window
			// 
			if (s_bPostProcessActive)
			{
				ImGui::SetNextWindowPos(*s_pPostProcessPosition, ImGuiSetCond_FirstUseEver);
				ImGui::SetNextWindowSize(*s_pPostProcessDimensions, ImGuiSetCond_FirstUseEver);
				ImGui::Begin("Post Process Settings", &s_bDebugOverlayActive);
				ImGui::InputText("Filepath", s_pFilepathBuf, s_iFilepathLength, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll);
				ImGui::Text("Click & move mouse to change values.\nHold ALT for smaller increments...");
				if (NULL != pScanlineSettings)
				{
					ImGui::DragFloat("Scanline", &pScanlineSettings->_fScanlineDark,0.05f, 0.0f, 1.0f, "Scanline Dark: %.3f");
					ImGui::DragFloat("Vert. Scanline", &pScanlineSettings->_fVScanlineDark, 0.05f, 0.0f, 1.0f, "Vert. Scanline Dark: %.3f");
					ImGui::DragFloat("Pixel Bias", &pScanlineSettings->_fPixelBias, 0.05f, 0.0f, 1.0f, "Pixel Bias: %.3f");
					ImGui::DragFloat("Contrast", &pScanlineSettings->_fContrast, 0.05f, 0.0f, 4.0f, "Contrast: %.3f");
					ImGui::DragFloat("Brightness", &pScanlineSettings->_fBrightness, 0.05f, 0.0f, 5.0f, "Brightness: %.3f");
					ImGui::DragFloat("Additive", &pScanlineSettings->_fAdditiveStrength, 0.05f, 0.0f, 1.0f, "Additive: %.3f");
					ImGui::DragFloat("Bloom Threshold", &pScanlineSettings->_fThreshold, 0.05f, 0.0f, 1.0f, "Threshold: %.3f");
					ImGui::DragFloat("Bloom Scaler", &pScanlineSettings->_fThresholdBrightScaler, 0.05f, 0.0f, 1.0f, "Scaler: %.3f");
					ImGui::DragFloat("Bloom Alpha", &pScanlineSettings->_fBloomAlpha, 0.05f, 0.0f, 2.0f, "Alpha: %.3f");
					ImGui::DragFloat("Bloom Contrast", &pScanlineSettings->_fBloomContrast, 0.05f, 0.0f, 2.0f, "Contrast: %.3f");
					ImGui::DragFloat("Bloom Brightness", &pScanlineSettings->_fBloomBright, 0.05f, 0.0f, 2.0f, "Brightness: %.3f");
					ImGui::Checkbox("Bloom", &pScanlineSettings->_bDoBloom); ImGui::SameLine();
					ImGui::Checkbox("Scanlines", &pScanlineSettings->_bDoScanlines);
					if (ImGui::Button("Save", ImVec2(380, 0)))
					{
						if (SavePostProcessSettings(pScanlineSettings, s_pFilepathBuf)) ImGui::OpenPopup("saveok"); else ImGui::OpenPopup("savefail");
					}

					// Modal popup for save success
					if (ImGui::BeginPopupModal("saveok", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("Post Process Settings have been saved!");
						if (ImGui::Button("OK", ImVec2(320, 0))) ImGui::CloseCurrentPopup();
						ImGui::EndPopup();
					}


					// Modal popup for save success
					if (ImGui::BeginPopupModal("savefail", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("Unable to save Post Process Settings. Er...");
						if (ImGui::Button("OK", ImVec2(320, 0))) ImGui::CloseCurrentPopup();
						ImGui::EndPopup();
					}
				}
				ImGui::End();
			}

			// Draw the log window
			s_pDebugLog->Draw("Output Log", &s_bOutputLogActive, s_pLogPosition);

			// Draw the GameGlobals inspector
			if (s_bGameGlobalsActive) CGameGlobals::Instance().DebugUpdate();
		}
	}	
};
#endif
