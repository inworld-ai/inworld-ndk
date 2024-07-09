/**
 * Copyright 2022-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "ClientSpeechProcessor.h"

#include "InworldVAD.h"
#include "Log.h"
#include "ai/inworld/packets/packets.pb.h"

Inworld::ClientSpeechProcessor::ClientSpeechProcessor(const ClientSpeechOptions& Options)
    : _Options(Options)
{
    if (_Options.Mode >= ClientSpeechOptions::Mode::VAD)
    {
        if (_Options.VADModelPath.empty())
        {
            LogError("Inworld::ClientSpeechProcessor invalid VAD model path. Falling back to default mode.");
            _Options.Mode = ClientSpeechOptions::Mode::Default;
        }
        else if (!_Options.VADCb)
        {
            LogError("Inworld::ClientSpeechProcessor invalid VAD callback. Falling back to default mode.");
            _Options.Mode = ClientSpeechOptions::Mode::Default;
        }
        else
        {
            VAD_Initialize(_Options.VADModelPath.c_str());
        }
    }
    
    if (_Options.Mode == ClientSpeechOptions::Mode::STT)
    {
        /*if (_Options.STTModelPath.empty())
        {
            LogError("Inworld::ClientSpeechProcessor invalid STT model path. Falling back to default mode.");
            _Options.Mode = ClientSpeechOptions::Mode::Default;
        }
        else
        {
            STT_Initialize(_Options.STTModelPath.c_str());
        }*/
    }
}

Inworld::ClientSpeechProcessor::~ClientSpeechProcessor()
{
	ClearState();

}

void Inworld::ClientSpeechProcessor::ClearState()
{
	StopActualAudioSession();
	_Routing = {};
	_AudioQueue = {};
	_AudioSessionPayload = {};
	_VADSilenceCounter = 0;
    if (_Options.Mode >= ClientSpeechOptions::Mode::VAD)
    {
        VAD_ResetState();
    }
}

void Inworld::ClientSpeechProcessor::StartAudioSession(const std::string& AgentId, const AudioSessionStartPayload& Payload, const std::string& OwnerId)
{
	ClearState();
	_Routing = Routing::Player2Agent(AgentId);
	_AudioSessionPayload = Payload;
	_SessionOwnerId = OwnerId;
	if (_Options.Mode == ClientSpeechOptions::Mode::Default)
	{
		StartActualAudioSession();
	}
}

void Inworld::ClientSpeechProcessor::StartAudioSessionInConversation(const std::string& ConversationId, const AudioSessionStartPayload& Payload, const std::string& OwnerId)
{
	ClearState();
	_Routing = Routing::Player2Conversation(ConversationId);
	_AudioSessionPayload = Payload;
	_SessionOwnerId = OwnerId;
	if (_Options.Mode == ClientSpeechOptions::Mode::Default)
	{
		StartActualAudioSession();
	}
}

void Inworld::ClientSpeechProcessor::StopAudioSession(const std::string& AgentId)
{
	if (_Options.Mode == ClientSpeechOptions::Mode::Default)
	{
		StopActualAudioSession();
	}
	ClearState();
}

void Inworld::ClientSpeechProcessor::StopAudioSessionInConversation(const std::string& ConversationId)
{
	if (_Options.Mode == ClientSpeechOptions::Mode::Default)
	{
		StopActualAudioSession();
	}
	ClearState();
}

void Inworld::ClientSpeechProcessor::SendSoundMessage(const std::string& AgentId, const std::vector<int16_t>& InputData)
{
	if (_Routing._Target._Type != InworldPackets::Actor_Type::Actor_Type_AGENT || _Routing._Target._Name != AgentId)
	{
		LogWarning("Inworld::ClientSpeechProcessor::SendSoundMessage invalid routing: Agent %hs.", AgentId.c_str());
		return;
	}

	ProcessAudio(InputData, {});
}

void Inworld::ClientSpeechProcessor::SendSoundMessageToConversation(const std::string& ConversationId, const std::vector<int16_t>& InputData)
{
	if (_Routing._ConversationId != ConversationId)
	{
		LogWarning("Inworld::ClientSpeechProcessor::SendSoundMessage invalid routing: Conversation %hs.", ConversationId.c_str());
		return;
	}

	ProcessAudio(InputData, {});
}

void Inworld::ClientSpeechProcessor::SendSoundMessageWithAEC(const std::string& AgentId, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData)
{
	if (_Routing._Target._Type != InworldPackets::Actor_Type::Actor_Type_AGENT || _Routing._Target._Name != AgentId)
	{
		LogWarning("Inworld::ClientSpeechProcessor::SendSoundMessage invalid routing: Agent %hs.", AgentId.c_str());
		return;
	}

	ProcessAudio(InputData, OutputData);
}

void Inworld::ClientSpeechProcessor::SendSoundMessageWithAECToConversation(const std::string& ConversationId, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData)
{
	if (_Routing._ConversationId != ConversationId)
	{
		LogWarning("Inworld::ClientSpeechProcessor::SendSoundMessage invalid routing: Conversation %hs.", ConversationId.c_str());
		return;
	}

	ProcessAudio(InputData, OutputData);
}

bool Inworld::ClientSpeechProcessor::StartActualAudioSession()
{
	if (_bSessionActive)
	{
		return false;
	}

    if (_Options.VADCb)
    {
        _Options.VADCb(_SessionOwnerId, true);
    }

    if (_Options.PacketCb)
    {
        const auto Packet = std::make_shared<Inworld::ControlEventAudioSessionStart>
            (_Routing, static_cast<ai::inworld::packets::AudioSessionStartPayload_MicrophoneMode>(_AudioSessionPayload.MicMode));
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
        _Options.VADCb(_SessionOwnerId, false);
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

void Inworld::ClientSpeechProcessor::ProcessAudio(const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData)
{
    std::vector<int16_t> FilteredData = _EchoFilter.FilterAudio(InputData, OutputData);
	const std::string Data((char*)FilteredData.data(), FilteredData.size() * 2);

	if (_Options.Mode == ClientSpeechOptions::Mode::Default)
	{
		SendAudio(Data);
		return;
	}
	
	std::vector<float> FloatData(FilteredData.size());
	for (size_t i = 0; i < FilteredData.size(); ++i)
	{
		FloatData[i] = static_cast<float>(FilteredData[i]) / 32767.0f;
	}

	const float SpeechProb = Inworld::VAD_Process(FloatData.data(), FloatData.size());
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