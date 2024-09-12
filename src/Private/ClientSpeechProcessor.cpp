/**
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
{
}

Inworld::ClientSpeechProcessor::~ClientSpeechProcessor()
{
	ClearState();
    DisableAudioDump();
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
	_AudioSessionPayload = {};
}


void Inworld::ClientSpeechProcessor::StartAudioSession(const Inworld::Routing& Routing,
                                                       const AudioSessionStartPayload& Payload)
{
    ClearState();
    _Routing = Routing;
    _AudioSessionPayload = Payload;
}

void Inworld::ClientSpeechProcessor::StopAudioSession(const Inworld::Routing& Routing)
{
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

    if (_OutgoingPacketCb)
    {
        const auto Packet = std::make_shared<Inworld::ControlEventAudioSessionStart>(_Routing,
            static_cast<ai::inworld::packets::AudioSessionStartPayload_MicrophoneMode>(_AudioSessionPayload.MicMode),
            static_cast<ai::inworld::packets::AudioSessionStartPayload_UnderstandingMode>(_AudioSessionPayload.UndMode));
        _OutgoingPacketCb(Packet);

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

    if (_OutgoingPacketCb)
    {
        const auto Packet
            = std::make_shared<Inworld::ControlEvent>(ai::inworld::packets::ControlEvent_Action_AUDIO_SESSION_END, std::string{}, _Routing);
        _OutgoingPacketCb(Packet);

        Log("ClientSpeechProcessor: stop actual audio session.");
    }
    
	_bSessionActive = false;
    
	return true;
}

void Inworld::ClientSpeechProcessor::SendAudio(const std::string& Data)
{
	if (_Routing._ConversationId.empty() && _Routing._Target._Name.empty())
	{
		LogWarning("Inworld::ClientSpeechProcessor::SendAudio invalid routing.");
		return;
	}

    const auto Packet = std::make_shared<AudioDataEvent>(Data, _Routing);
	if (_OutgoingPacketCb)
    {
        _OutgoingPacketCb(Packet);
	    
#ifdef INWORLD_AUDIO_DUMP
	    if (_bDumpAudio)
	        _AudioChunksToDump.PushBack(Data);
#endif
    }
}

void Inworld::ClientSpeechProcessor_Default::StartAudioSession(const Inworld::Routing& Routing,
    const AudioSessionStartPayload& Payload)
{
    ClientSpeechProcessor::StartAudioSession(Routing, Payload);
    StartActualAudioSession();
}

void Inworld::ClientSpeechProcessor_Default::StopAudioSession(const Inworld::Routing& Routing)
{
    StopActualAudioSession();
    ClientSpeechProcessor::StopAudioSession(Routing);
}

void Inworld::ClientSpeechProcessor_Default::ProcessAudio(const std::string& Data)
{
    SendAudio(Data);
}

Inworld::ClientSpeechProcessor_VAD::ClientSpeechProcessor_VAD(const ClientSpeechOptions_VAD& Options)
    : Inworld::ClientSpeechProcessor(Options)
    , VADProbThreshhold(Options.VADProbThreshhold)
    , VADBufferChunksNum(Options.VADBufferChunksNum)
    , VADSilenceChunksNum(Options.VADSilenceChunksNum)
{
#ifdef INWORLD_VAD
    VAD_Initialize(Options.VADModelPath.c_str());
#endif
}

Inworld::ClientSpeechProcessor_VAD::~ClientSpeechProcessor_VAD()
{
#ifdef INWORLD_VAD
    VAD_Terminate();
#endif
}

bool Inworld::ClientSpeechProcessor_VAD::DetectVoice(const std::string& Data)
{
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
    const bool bVoiceDetected = SpeechProb > VADProbThreshhold;
    
    return bVoiceDetected;
}

void Inworld::ClientSpeechProcessor_VAD::ProcessAudio(const std::string& Data)
{
    if (DetectVoice(Data))
    {
        _VADSilenceCounter = 0;
        HandleVoiceDetected(Data);
        return;
    }

    if (!_bSessionActive)
    {
        BufferAudio(Data);
        return;
    }

    if (++_VADSilenceCounter > VADSilenceChunksNum)
    {
        HandleSilenceDetected(Data);
        _VADSilenceCounter = 0;
    }
}

void Inworld::ClientSpeechProcessor_VAD::ClearState()
{
    ClientSpeechProcessor::ClearState();
#ifdef INWORLD_VAD
    VAD_ResetState();
#endif
}

void Inworld::ClientSpeechProcessor_VAD_DetectOnly::StartAudioSession(const Inworld::Routing& Routing,
                                                                      const AudioSessionStartPayload& Payload)
{
    ClientSpeechProcessor::StartAudioSession(Routing, Payload);
    StartActualAudioSession();
}

void Inworld::ClientSpeechProcessor_VAD_DetectOnly::StopAudioSession(const Inworld::Routing& Routing)
{
    StopActualAudioSession();
    ClientSpeechProcessor::StopAudioSession(Routing);
}

void Inworld::ClientSpeechProcessor_VAD_DetectOnly::ProcessAudio(const std::string& Data)
{
    SendAudio(Data);
    ClientSpeechProcessor_VAD::ProcessAudio(Data);
}

void Inworld::ClientSpeechProcessor_VAD_DetectOnly::HandleVoiceDetected(const std::string& Data)
{
    if (!_bVoiceDetected)
    {
        _bVoiceDetected = true;
        if (_IncomingPacketCb)
        {
            auto Packet = std::make_shared<Inworld::VADEvent>(_bVoiceDetected, _Routing);
            _IncomingPacketCb(Packet);
        }
    }
}

void Inworld::ClientSpeechProcessor_VAD_DetectOnly::HandleSilenceDetected(const std::string& Data)
{
    if (_bVoiceDetected)
    {
        _bVoiceDetected = false;
        if (_IncomingPacketCb)
        {
            auto Packet = std::make_shared<Inworld::VADEvent>(_bVoiceDetected, _Routing);
            _IncomingPacketCb(Packet);
        }
    }
}

void Inworld::ClientSpeechProcessor_VAD_DetectAndFilterAudio::HandleVoiceDetected(const std::string& Data)
{
    const bool bJustStarted = StartActualAudioSession();
    if (!bJustStarted)
    {
        // audio session is already active
        // send audio immediately
        SendAudio(Data);
    }
    else
    {
        // audio session just started
        // send buffered audio first
        _AudioQueue.push(Data);
        std::string DataToSend;
        while (!_AudioQueue.empty())
        {
            DataToSend.append(_AudioQueue.front());
            _AudioQueue.pop();
        }

        SendAudio(DataToSend);

        if (_IncomingPacketCb)
        {
            const bool bVoiceDetected = true;
            auto Packet = std::make_shared<Inworld::VADEvent>(bVoiceDetected, _Routing);
            _IncomingPacketCb(Packet);
        }
    }
}

void Inworld::ClientSpeechProcessor_VAD_DetectAndFilterAudio::HandleSilenceDetected(const std::string& Data)
{
    StopActualAudioSession();

    if (_IncomingPacketCb)
    {
        const bool bVoiceDetected = false;
        auto Packet = std::make_shared<Inworld::VADEvent>(bVoiceDetected, _Routing);
        _IncomingPacketCb(Packet);
    }
}

void Inworld::ClientSpeechProcessor_VAD_DetectAndFilterAudio::BufferAudio(const std::string& Data)
{
    ClientSpeechProcessor_VAD::BufferAudio(Data);

    _AudioQueue.push(Data);
    if (_AudioQueue.size() > VADBufferChunksNum)
    {
        _AudioQueue.pop();
    }
}

void Inworld::ClientSpeechProcessor_VAD_DetectAndFilterAudio::ClearState()
{
    ClientSpeechProcessor_VAD::ClearState();

    _AudioQueue = {};
    _VADSilenceCounter = 0;
}
