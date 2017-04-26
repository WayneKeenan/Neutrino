
namespace Neutrino {

	class CAudioInterface {
		
		public:
			virtual void SetGlobalVolume(const int iVolume) =0;
			
			virtual void PauseAll()=0;
			virtual void ResumeAll()=0;

			virtual int LoadSample(const char* pFilepathAndName) =0;
			virtual bool PlaySample(const int iSampleID) =0;
		
	};
};