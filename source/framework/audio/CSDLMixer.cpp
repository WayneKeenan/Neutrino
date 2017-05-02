#include "CSDLMixer.h"
#include "../../framework/File.h"
#include "../../framework/Log.h"

namespace Neutrino
{

	CSDLMixer::CSDLMixer()
	{
		for (int i = 0; i < _iMAX_SAMPLES; ++i)
		{
			m_aLoadedSamples[i] = NULL;
		}

		for (int i = 0; i < _iMAX_MUSIC; ++i)
		{
			m_aLoadedMusic[i] = NULL;
		}
	}

	CSDLMixer::~CSDLMixer()
	{
		for (int i = 0; i < _iMAX_SAMPLES; ++i)
		{
			if (NULL != m_aLoadedSamples[i])
				Mix_FreeChunk(m_aLoadedSamples[i]);
		}

		for (int i = 0; i < _iMAX_MUSIC; ++i)
		{
			if (NULL != m_aLoadedMusic[i])
				Mix_FreeMusic(m_aLoadedMusic[i]);
		}
		LOG_INFO("Music and Samples Deallocated");
	}

	void CSDLMixer::SetGlobalVolume(const float iVolume)
	{
		m_fMasterVolume = iVolume;
		Mix_VolumeMusic((int)((128.0f * m_fMusicVolume)*m_fMasterVolume));
		Mix_Volume(-1, (int)((128.0f * m_fSampleVolume)*m_fMasterVolume));
	}

	void CSDLMixer::SetMusicVolume(const float iVolume)
	{
		m_fMusicVolume = iVolume;
		Mix_VolumeMusic((int)((128.0f * m_fMusicVolume)*m_fMasterVolume));
	}

	void CSDLMixer::SetSampleVolume(const float iVolume)
	{
		m_fSampleVolume = iVolume;
		Mix_Volume(-1, (int)((128.0f * m_fSampleVolume)*m_fMasterVolume));
	}

	void CSDLMixer::PauseAll()
	{

	}

	void CSDLMixer::ResumeAll() 
	{

	}

	bool CSDLMixer::LoadSample(const int iID, const char* pFilename)
	{
		uint32 iFileSize = GetFileSizeBytes(pFilename);
		ASSERT(iFileSize > 0, "LoadSample: %s zero bytes in size?", pFilename);

		const char* pFileBytes = LoadResourceBytes(pFilename);
		ASSERT(NULL != pFileBytes, "LoadSample: loading file failed: %s", pFilename);

		SDL_RWops* pOps = SDL_RWFromConstMem(pFileBytes, iFileSize);
		ASSERT(pOps, "LoadSample: SDL_RWFromMem failed");

		m_aLoadedSamples[iID] = Mix_LoadWAV_RW(pOps, 1);
		ASSERT(NULL != m_aLoadedSamples[iID], "LoadSample: loading file failed: %s", pFilename);
		// TODO: should this assert here? We could just ignore it, and continue...

		DELETEX[] pFileBytes;
		return true;
	}

	bool CSDLMixer::LoadMusic(const int iID, const char* pFilename)
	{
		uint32 iFileSize = GetFileSizeBytes(pFilename);
		ASSERT(iFileSize > 0, "LoadMusic: %s zero bytes in size?", pFilename);

		const char* pFileBytes = LoadResourceBytes(pFilename);
		ASSERT(NULL != pFileBytes, "LoadMusic: loading file failed: %s", pFilename);

		SDL_RWops* pOps = SDL_RWFromConstMem(pFileBytes, iFileSize);
		ASSERT(pOps, "LoadMusic: SDL_RWFromMem failed");

		m_aLoadedMusic[iID] = Mix_LoadMUS_RW(pOps, 1);
		ASSERT(NULL != m_aLoadedMusic[iID], "LoadMusic: loading file failed: %s", pFilename);
		// TODO: should this assert here? We could just ignore it, and continue...

		LOG_INFO("-- OK. Size %dk", iFileSize / 1024);
		DELETEX[] pFileBytes;
		return true;
		return true;
	}

	bool CSDLMixer::PlaySample(const int iID)
	{
		if (Mix_PlayChannel(-1, m_aLoadedSamples[iID], 0) == -1)
		{
			LOG_ERROR("Play channel: %s", Mix_GetError());
		}
		return true;
	}

	bool CSDLMixer::PlayMusic(const int iID)
	{
		if (Mix_PlayMusic(m_aLoadedMusic[iID], -1) == -1)
		{
			LOG_ERROR("Play music: %s", Mix_GetError());
		}
		return true;
	}






}
