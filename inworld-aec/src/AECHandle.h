/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once
#include "api/echo_canceller3_factory.h"
#include "api/echo_canceller3_config.h"
#include "audio_processing/audio_buffer.h"
#include "audio_processing/high_pass_filter.h"

# pragma comment(lib, "secur32.lib")
# pragma comment(lib, "winmm.lib")
# pragma comment(lib, "dmoguids.lib")
# pragma comment(lib, "wmcodecdspuuid.lib")
# pragma comment(lib, "msdmo.lib")
# pragma comment(lib, "Strmiids.lib")

namespace webrtc
{
	class AECHandle
	{
	public:
		constexpr static int kSampleRate = 16000;
		constexpr static int kChannels = 1;
		constexpr static int kBitPerSample = 16;
		constexpr static int kSamplesPerFrame = 160;
		constexpr static int kBytesPerFrame = 320;

		AECHandle();
		virtual ~AECHandle();

		void SampleOutput(const int16_t* farendData);
		void SampleInput(const int16_t* nearendData);

		int16_t* GetFilteredData() const;
		
	private:	
		EchoCanceller3Config m_AecConfig;
		EchoCanceller3Factory m_AecFactory;

		AudioFrame m_RefFrame;
		AudioFrame m_AecFrame;
		
		std::unique_ptr<EchoControl> m_EchoController;
		std::unique_ptr<AudioBuffer> m_RefAudio;
		std::unique_ptr<AudioBuffer> m_AecAudio;
		std::unique_ptr<AudioBuffer> m_AecLinearAudio;
		std::unique_ptr<HighPassFilter> m_HPFilter;

		// std::unique_ptr<AudioBuffer> aecLinearAudio;
		int16_t* m_pFarendTmp;
		int16_t* m_pNearendTmp;
		int16_t* m_pFilteredTmp;		
	};
}


