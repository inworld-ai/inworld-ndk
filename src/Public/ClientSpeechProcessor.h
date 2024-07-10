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
#include "AsyncRoutine.h"
#include "Packets.h"
#include "SharedQueue.h"

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
    using ClientSpeechVADCallback = std::function<void(bool bVoiceDetected)>;
    
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
        uint8_t VADPreviousChunks = 5;
        uint8_t VADSubsequentChunks = 5;

        ClientSpeechPacketCallback PacketCb;
        ClientSpeechVADCallback VADCb;
    };
    
    class INWORLD_EXPORT ClientSpeechProcessor final
    {
    public:
        
	    ClientSpeechProcessor() = default;
        ClientSpeechProcessor(const ClientSpeechOptions& Options);
        ClientSpeechProcessor(const ClientSpeechProcessor&) = delete;
        ClientSpeechProcessor& operator=(const ClientSpeechProcessor&) = delete;
        ClientSpeechProcessor(ClientSpeechProcessor&&) = default;
        ClientSpeechProcessor& operator=(ClientSpeechProcessor&&) = default;
	    ~ClientSpeechProcessor();

        void StartAudioSession(const Inworld::Routing& Routing, const AudioSessionStartPayload& Payload);
        void StopAudioSession(const Inworld::Routing& Routing);
        void SendSoundMessage(const Inworld::Routing& Routing, const std::string& Data);
        void SendSoundMessageWithAEC(const Inworld::Routing& Routing, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData);

        void EnableAudioDump(const std::string& FileName);
        void DisableAudioDump();
	    
    private:
	    bool StartActualAudioSession();
	    bool StopActualAudioSession();
	    void ProcessAudio(const std::string& InputData);
	    void SendAudio(const std::string& Data);
	    void SendBufferedAudio();
	    void ClearState();

        AECFilter _EchoFilter;
        ClientSpeechOptions _Options;
	    std::queue<std::string> _AudioQueue;
	    AudioSessionStartPayload _AudioSessionPayload;
	    Routing _Routing;
	    bool _bSessionActive = false;
        uint8_t _VADSilenceCounter = 0;

#ifdef INWORLD_AUDIO_DUMP
        AsyncRoutine _AsyncAudioDumper;
        SharedQueue<std::string> _AudioChunksToDump;
        bool _bDumpAudio = false;
        std::string _AudioDumpFileName = "C:/Tmp/AudioDump.wav";
#endif
    };
}
