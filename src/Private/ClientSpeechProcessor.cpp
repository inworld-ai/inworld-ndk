﻿/**
 * Copyright 2022-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "ClientSpeechProcessor.h"

#include <filesystem>

#ifdef INWORLD_VAD
#include "InworldVAD.h"
#endif
#include "Log.h"
#include "Service.h"
#include "ai/inworld/packets/packets.pb.h"  

Inworld::ClientSpeechProcessor::ClientSpeechProcessor(const ClientSpeechOptions& Options)
    : _Options(Options)
{
#ifndef INWORLD_VAD
    _Options.Mode = ClientSpeechOptions::SpeechMode::Default;
#endif
    
    if (_Options.Mode == ClientSpeechOptions::SpeechMode::Default)
    {
        return;
    }

    if (_Options.VADModelPath.empty())
    {
        LogError("Inworld::ClientSpeechProcessor invalid VAD model path. Falling back to default mode.");
        _Options.Mode = ClientSpeechOptions::SpeechMode::Default;
        return;
    }

    if (!_Options.VADCb)
    {
        LogError("Inworld::ClientSpeechProcessor invalid VAD callback. Falling back to default mode.");
        _Options.Mode = ClientSpeechOptions::SpeechMode::Default;
        return;
    }

#ifdef INWORLD_VAD
    VAD_Initialize(_Options.VADModelPath.c_str());
#endif
}

Inworld::ClientSpeechProcessor::~ClientSpeechProcessor()
{
	ClearState();
    DisableAudioDump();
#ifdef INWORLD_VAD
    VAD_Terminate();
#endif
}

void Inworld::ClientSpeechProcessor::SendSoundMessageWithAEC(const Inworld::Routing& Routing,
    const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData)
{
    std::vector<int16_t> FilteredData = _EchoFilter.FilterAudio(InputData, OutputData);
    const std::string Data((char*)FilteredData.data(), FilteredData.size() * 2);
    SendSoundMessage(Routing, Data);
}

void Inworld::ClientSpeechProcessor::ClearState()
{
	StopActualAudioSession();
	_Routing = {};
	_AudioQueue = {};
	_AudioSessionPayload = {};
	_VADSilenceCounter = 0;
    if (_Options.Mode >= ClientSpeechOptions::SpeechMode::VAD)
    {
#ifdef INWORLD_VAD
        VAD_ResetState();
#endif
    }
}

void Inworld::ClientSpeechProcessor::StartAudioSession(const Inworld::Routing& Routing,
    const AudioSessionStartPayload& Payload)
{
    ClearState();
    _Routing = Routing;
    _AudioSessionPayload = Payload;
    if (_Options.Mode == ClientSpeechOptions::SpeechMode::Default)
    {
        StartActualAudioSession();
    }
}

void Inworld::ClientSpeechProcessor::StopAudioSession(const Inworld::Routing& Routing)
{
    if (_Options.Mode == ClientSpeechOptions::SpeechMode::Default)
    {
        StopActualAudioSession();
    }
    ClearState();
}

void Inworld::ClientSpeechProcessor::SendSoundMessage(const Inworld::Routing& Routing, const std::string& Data)
{
    if (Routing != _Routing)
    {
        LogWarning("Inworld::ClientSpeechProcessor::SendSoundMessage invalid routing.");
        return;
    }

    ProcessAudio(Data);
}

#ifdef INWORLD_AUDIO_DUMP
bool IsValidPath(const std::string& path) {
    namespace fs = std::filesystem;
    const fs::path filePath(path);

    if (!filePath.has_parent_path()) {
        return false;
    }

    if (!fs::exists(filePath.parent_path())) {
        return false;
    }

    const std::string filename = filePath.filename().string();
    const size_t dotPosition = filename.find_last_of('.');
    if (dotPosition == std::string::npos || dotPosition == 0 || dotPosition == filename.length() - 1) {
        return false;
    }

    if (filename.find('.', dotPosition + 1) != std::string::npos) {
        return false;
    }

    return true;
}
#endif

void Inworld::ClientSpeechProcessor::EnableAudioDump(const std::string& FilePath)
{
#ifdef INWORLD_AUDIO_DUMP
    _bDumpAudio = true;
    if (IsValidPath(FilePath))
    {
        _AudioDumpFileName = FilePath;
    }
    else if (!IsValidPath(_AudioDumpFileName))
    {
        Inworld::LogError("Inworld::ClientSpeechProcessor::EnableAudioDump invalid file path.");
        return;
    }
    _AsyncAudioDumper.Stop();
    _AsyncAudioDumper.Start("InworldAudioDumper", std::make_unique<RunnableAudioDumper>(_AudioChunksToDump, _AudioDumpFileName));
#endif
}

void Inworld::ClientSpeechProcessor::DisableAudioDump()
{
#ifdef INWORLD_AUDIO_DUMP
    _AsyncAudioDumper.Stop();
#endif
}

bool Inworld::ClientSpeechProcessor::StartActualAudioSession()
{
	if (_bSessionActive)
	{
		return false;
	}

    if (_Options.VADCb)
    {
        _Options.VADCb(true);
    }

    if (_Options.PacketCb)
    {
        const auto Packet = std::make_shared<Inworld::ControlEventAudioSessionStart>(_Routing,
            static_cast<ai::inworld::packets::AudioSessionStartPayload_MicrophoneMode>(_AudioSessionPayload.MicMode),
            static_cast<ai::inworld::packets::AudioSessionStartPayload_UnderstandingMode>(_AudioSessionPayload.UndMode));
        _Options.PacketCb(Packet);

        Log("ClientSpeechProcessor: start actual audio session.");
    }
    
	_bSessionActive = true;
    
	return true;
}

bool Inworld::ClientSpeechProcessor::StopActualAudioSession()
{
	if (!_bSessionActive)
	{
		return false;
	}
	
    if (_Options.VADCb)
    {
        _Options.VADCb(false);
    }

    if (_Options.PacketCb)
    {
        const auto Packet
            = std::make_shared<Inworld::ControlEvent>(ai::inworld::packets::ControlEvent_Action_AUDIO_SESSION_END, std::string{}, _Routing);
        _Options.PacketCb(Packet);

        Log("ClientSpeechProcessor: stop actual audio session.");
    }
    
	_bSessionActive = false;
    
	return true;
}

void Inworld::ClientSpeechProcessor::ProcessAudio(const std::string& Data)
{
	if (_Options.Mode == ClientSpeechOptions::SpeechMode::Default)
	{
		SendAudio(Data);
		return;
	}
	
	std::vector<float> FloatData;
    FloatData.reserve(Data.size() / 2);
	for (size_t i = 0; i < Data.size(); i += 2)
	{
	    const int16_t Sample = *reinterpret_cast<const int16_t*>(Data.data() + i);
		FloatData.emplace_back(static_cast<float>(Sample) / 32767.0f);
	}

#ifdef INWORLD_VAD
	const float SpeechProb = Inworld::VAD_Process(FloatData.data(), FloatData.size());
#else
    constexpr float SpeechProb = 1.f;
#endif
	if (SpeechProb > _Options.VADProbThreshhold)
	{
		_VADSilenceCounter = 0;
		const bool bJustStarted = StartActualAudioSession();
		if (bJustStarted)
		{
			_AudioQueue.push(Data);
			SendBufferedAudio();
		}
		else
		{
			SendAudio(Data);
		}
		return;
	}

	if (!_bSessionActive)
	{
		_AudioQueue.push(Data);
		if (_AudioQueue.size() > _Options.VADPreviousChunks)
		{
			_AudioQueue.pop();
		}
		return;
	}

	SendAudio(Data);
	if (++_VADSilenceCounter > _Options.VADSubsequentChunks)
	{
		StopActualAudioSession();
		_VADSilenceCounter = 0;
	}
}

void Inworld::ClientSpeechProcessor::SendAudio(const std::string& Data)
{
	if (_Routing._ConversationId.empty() && _Routing._Target._Name.empty())
	{
		LogWarning("Inworld::ClientSpeechProcessor::SendAudio invalid routing.");
		return;
	}

    const auto Packet = std::make_shared<AudioDataEvent>(Data, _Routing);
	if (_Options.PacketCb)
    {
        _Options.PacketCb(Packet);
	    
#ifdef INWORLD_AUDIO_DUMP
	    if (_bDumpAudio)
	        _AudioChunksToDump.PushBack(Data);
#endif
    }
}

void Inworld::ClientSpeechProcessor::SendBufferedAudio()
{
	if (_AudioQueue.empty())
	{
		return;
	}
	
	std::string Data;
	while (!_AudioQueue.empty())
	{
		Data.append(_AudioQueue.front());
		_AudioQueue.pop();
	}

    SendAudio(Data);
}