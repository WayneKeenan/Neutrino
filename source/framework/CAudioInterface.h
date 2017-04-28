
namespace Neutrino {

	class CAudioInterface {
		
		public:
			virtual void SetGlobalVolume(const int iVolume) =0;

			virtual void PauseAll()=0;
			virtual void ResumeAll()=0;

			virtual void PauseMusic()=0;
			virtual void ResumeMusic()0=;

			virtual int LoadSample(const char* pFilepathAndName)=0;
			virtual bool PlaySample(const int iSampleID)=0;
		
			virtual void FadeOutMusic()=0;
			virtual void FadeInMusic()=0;

			virtual void DeallocateAllSamples()=0;
			virtual void DeallocateAllMusic()=0;
	};
};