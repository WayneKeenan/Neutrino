#pragma once
#include "CAudioInterface.h"
#include "SDL_mixer.h"
#include "../Types.h"

namespace Neutrino
{
	class CSDLMixer : public CAudioInterface {
	public:
		CSDLMixer();
		~CSDLMixer();

		void SetGlobalVolume(const float iVolume);
		void SetMusicVolume(const float iVolume);
		void SetSampleVolume(const float iVolume);
		void PauseAll();
		void ResumeAll();
		bool LoadSample(const int iID, const char* pFilename);
		bool LoadMusic(const int iID, const char* pFilename);
		bool PlaySample(const int iID);
		bool PlayMusic(const int iID);
		void UnloadAllMusic();
		void UnloadAllSamples();

	private:
		Mix_Chunk* m_aLoadedSamples[_iMAX_SAMPLES];
		Mix_Music* m_aLoadedMusic[_iMAX_MUSIC];

		float m_fMasterVolume = 1.0f;
		float m_fMusicVolume = 1.0f;
		float m_fSampleVolume = 1.0f;
	};
}
