#pragma once
#include "Types.h"
#include <array>
#include "Assert.h"
#include <glm/glm.hpp> 


namespace Neutrino
{
	// DebugOverlayInit()
	// 		Creates the struct holding overlay params and sets defaults. 
	void DebugOverlayInit();

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


	// ------------------------------------------------------------------------------------------------------

	// 
	// Nicked from Barry C. / Garez
	// 
	// Frame rate monitor. This is used to keep a frame-rate current based on the last
	// 'N' samples of time deltas.
	template<typename Sample, size_t SAMPLES>
	class CFrameRateT
	{
	public:
		CFrameRateT(): m_samples(0), m_sum(0)
		{
			ASSERT(m_sample.size() > 0);
		}

		void Tick(Sample sample)
		{
			ASSERT(sample > 0);
			if (m_samples < m_sample.size())
			{
				// Still reading the first 'm_sample.size()' numbers
				m_sample[m_samples++] = sample;
				m_sum += sample;
				m_fps[m_samples] = (1 / GetLatestDeltaTime());
			}
			else
			{
				// Loop subsequent samples around [m_sample.size()..(m_sample.size()*2-1)]
				Sample& slot = m_sample[m_samples % m_sample.size()];
				m_sum += sample - slot;
				slot = sample;
				m_fps[m_samples % m_fps.size()] = (1 / GetLatestDeltaTime());
				if (++m_samples >= (m_sample.size() * 2))
				{
					m_samples = m_sample.size();
				}
			}
			ASSERT(m_sum > 0);
		}
		Sample GetLatestDeltaTime() const
		{
			// Determine the latest sample, if any
			if (m_samples > 0)
			{
				size_t index = (m_samples + m_sample.size() - 1) % m_sample.size();
				return m_sample[index];
			}
			return 0;
		}

		Sample GetLatestFrameRate() const
		{
			// Determine the frame rate based on the latest sample only
			Sample delta_time = GetLatestDeltaTime();
			return (delta_time > 0) ? (1 / delta_time) : 0;
		}

		Sample GetAverageDeltaTime() const
		{
			// If we're still collecting the first 'm_sample.size()', divide by that
			// Otherwise divide by the number of samples in the ring buffer
			if (m_samples >= m_sample.size())
			{
				ASSERT(m_sum > 0);
				return (float)m_sum / (float)m_sample.size();
			}
			else if (m_samples > 0)
			{
				ASSERT(m_sum > 0);
				return (float)m_sum / (float)m_samples;
			}
			return 0;
		}

		Sample GetAverageFrameRate() const
		{
			// Determine the average frame rate based on all samples
			Sample delta_time = GetAverageDeltaTime();
			return (delta_time > 0) ? (1 / delta_time) : 0;
		}

		Sample* GetSamples()
		{
			return &m_sample[0];
		}

		Sample* GetFPS()
		{
			return &m_fps[0];
		}

		int GetSampleSize()
		{
			return (int)m_sample.size();
		}

		int GetSampleOffset()
		{
			return (int)m_samples;
		}

	private:
		std::array<Sample, SAMPLES> m_sample;
		std::array<Sample, SAMPLES> m_fps;
		size_t m_samples;
		Sample m_sum;
	};



	typedef CFrameRateT<float, 60> CFrameRate;
}