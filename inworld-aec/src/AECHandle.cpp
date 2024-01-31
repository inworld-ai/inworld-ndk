/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "AECHandle.h"

#include <iostream>
#include <valarray>

webrtc::AECHandle::AECHandle()
{
	m_AecConfig.filter.export_linear_aec_output = true;
	EchoCanceller3Factory aec_factory = EchoCanceller3Factory(m_AecConfig);
	m_EchoController = aec_factory.Create(kSampleRate, kChannels, kChannels);
	m_HPFilter = std::make_unique<HighPassFilter>(kSampleRate, kChannels);
	m_RefAudio = std::make_unique<AudioBuffer>(kSampleRate, kChannels, kSampleRate, kChannels, kSampleRate, kChannels);
	m_AecAudio = std::make_unique<AudioBuffer>(kSampleRate, kChannels, kSampleRate, kChannels, kSampleRate, kChannels);
	m_AecLinearAudio = std::make_unique<AudioBuffer>(kSampleRate, kChannels, kSampleRate, kChannels, kSampleRate, kChannels);
	
	m_pFarendTmp = new int16_t[kSamplesPerFrame];
	m_pNearendTmp = new int16_t[kSamplesPerFrame];
	m_pFilteredTmp = new int16_t[kSamplesPerFrame];
}

webrtc::AECHandle::~AECHandle()
{
	delete[] m_pFarendTmp;
	delete[] m_pNearendTmp;
	delete[] m_pFilteredTmp;
}

void webrtc::AECHandle::SampleOutput(const int16_t* farendData)
{
	memcpy(m_pFarendTmp, farendData, kBytesPerFrame);
	m_RefFrame.UpdateFrame(0, m_pFarendTmp, kSamplesPerFrame, kSampleRate, AudioFrame::kNormalSpeech, AudioFrame::kVadActive, 1);
	m_RefAudio->CopyFrom(&m_RefFrame);
	m_RefAudio->SplitIntoFrequencyBands();
	m_EchoController->AnalyzeRender(m_RefAudio.get());
	m_RefAudio->MergeFrequencyBands();
}

void webrtc::AECHandle::SampleInput(const int16_t* nearendData)
{
	memcpy(m_pNearendTmp, nearendData, kBytesPerFrame);
	m_AecFrame.UpdateFrame(0, m_pNearendTmp, kSamplesPerFrame, kSampleRate, AudioFrame::kNormalSpeech, AudioFrame::kVadActive, 1);
	m_AecAudio->CopyFrom(&m_AecFrame);
	m_EchoController->AnalyzeCapture(m_AecAudio.get());
	m_AecAudio->SplitIntoFrequencyBands();
	m_HPFilter->Process(m_AecAudio.get(), true);
	m_EchoController->SetAudioBufferDelay(0);
	m_EchoController->ProcessCapture(m_AecAudio.get(), m_AecLinearAudio.get(), false); //YAN: Is Linear Audio related to AEC??
	m_AecAudio->MergeFrequencyBands();
	m_AecAudio->CopyTo(&m_AecFrame);
	memcpy(m_pFilteredTmp, m_AecFrame.data(), kBytesPerFrame);
	m_AecFrame.UpdateFrame(0, nullptr, kSamplesPerFrame, kSampleRate, AudioFrame::kNormalSpeech, AudioFrame::kVadActive, 1);
	m_AecLinearAudio->CopyTo(&m_AecFrame);
	memcpy(m_pNearendTmp, m_AecFrame.data(), kBytesPerFrame);
}

int16_t* webrtc::AECHandle::GetFilteredData() const
{
	return m_pFilteredTmp;
}
