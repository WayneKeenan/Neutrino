#pragma once

namespace Neutrino {

	class CAudioInterface {
		
		public:
			virtual ~CAudioInterface() {};
			virtual void SetGlobalVolume(const float iVolume) =0;
			virtual void SetMusicVolume(const float iVolume) = 0;
			virtual void SetSampleVolume(const float iVolume) = 0;
			
			virtual void PauseAll()=0;
			virtual void ResumeAll()=0;

			virtual bool LoadSample(const int iID, const char* pFilename) =0;
			virtual bool LoadMusic(const int iID, const char* pFilename) = 0;
			virtual bool PlaySample(const int iID) =0;
			virtual bool PlayMusic(const int iID) = 0;
			virtual void UnloadAllMusic() =0;
			virtual void UnloadAllSamples() =0;
	};
};
