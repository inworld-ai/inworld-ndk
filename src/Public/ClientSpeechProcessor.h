/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <functional>
#include <queue>
#include <string>
#include <vector>

#include "AECFilter.h"
#include "Packets.h"

namespace Inworld
{
    struct INWORLD_EXPORT AudioSessionStartPayload
    {
        enum class MicrophoneMode : uint8_t
        {
            Unspecified = 0,
            OpenMic = 1,
            ExpectAudioEnd = 2,
        };

        MicrophoneMode MicMode = MicrophoneMode::Unspecified;
    };

    using ClientSpeechPacketCallback = std::function<void(const std::shared_ptr<Packet>& Packet)>;
    using ClientSpeechVADCallback = std::function<void(const std::string& OwnerId, bool bVoiceDetected)>;
    
    struct ClientSpeechOptions
    {
        enum class Mode : uint8_t
        {
            Default = 0,
            VAD = 1,
            STT = 2,
        };  
        Mode Mode = Mode::Default;

        std::string VADModelPath;
        std::string STTModelPath;

        float VADProbThreshhold = 0.3f;
        int8_t VADPreviousChunks = 5;
        int8_t VADSubsequentChunks = 5;

        ClientSpeechPacketCallback PacketCb;
        ClientSpeechVADCallback VADCb;
    };
    
    class INWORLD_EXPORT ClientSpeechProcessor final
    {
    public:
        
	    ClientSpeechProcessor() = delete;
        ClientSpeechProcessor(const ClientSpeechOptions& Options);
        ClientSpeechProcessor(const ClientSpeechProcessor&) = delete;
        ClientSpeechProcessor& operator=(const ClientSpeechProcessor&) = delete;
        ClientSpeechProcessor(ClientSpeechProcessor&&) = delete;
        ClientSpeechProcessor& operator=(ClientSpeechProcessor&&) = delete;
	    ~ClientSpeechProcessor();

	    void StartAudioSession(const std::string& AgentId, const AudioSessionStartPayload& Payload, const std::string& OwnerId);
	    void StartAudioSessionInConversation(const std::string& ConversationId, const AudioSessionStartPayload& Payload, const std::string& OwnerId);
		    
	    void StopAudioSession(const std::string& AgentId);
	    void StopAudioSessionInConversation(const std::string& ConversationId);

	    void SendSoundMessage(const std::string& AgentId, const std::vector<int16_t>& InputData);
	    void SendSoundMessageToConversation(const std::string& ConversationId, const std::vector<int16_t>& InputData);
		    
	    void SendSoundMessageWithAEC(const std::string& AgentId, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData);
	    void SendSoundMessageWithAECToConversation(const std::string& ConversationId, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData);
	    
    private:
	    bool StartActualAudioSession();
	    bool StopActualAudioSession();
	    void ProcessAudio(const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData);
	    void SendAudio(const std::string& Data);
	    void SendBufferedAudio();
	    void ClearState();

        AECFilter _EchoFilter;
        ClientSpeechOptions _Options;
	    std::queue<std::string> _AudioQueue;
	    AudioSessionStartPayload _AudioSessionPayload;
	    Routing _Routing;
        std::string _SessionOwnerId;
	    bool _bSessionActive = false;
	    int8_t _VADSilenceCounter = 0;
    };

}
