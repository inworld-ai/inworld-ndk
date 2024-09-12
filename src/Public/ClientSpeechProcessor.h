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
#include "Utils/SharedQueue.h"

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

        enum class UnderstandingMode : uint8_t
        {
            Unspecified = 0,
            Full = 1,
            SpeechRecognitionOnly = 2,
        };

        MicrophoneMode MicMode = MicrophoneMode::Unspecified;
        UnderstandingMode UndMode = UnderstandingMode::Unspecified;
    };

    using ClientSpeechPacketCallback = std::function<void(const std::shared_ptr<Packet>& Packet)>;

    class ClientSpeechProcessor;
    class Client;

    struct ClientSpeechOptions{};
    
    class INWORLD_EXPORT ClientSpeechProcessor
    {
    public:
	    ClientSpeechProcessor() = default;
        ClientSpeechProcessor(const ClientSpeechOptions& Options);
        ClientSpeechProcessor(const ClientSpeechProcessor&) = delete;
        ClientSpeechProcessor& operator=(const ClientSpeechProcessor&) = delete;
        ClientSpeechProcessor(ClientSpeechProcessor&&) = delete;
        ClientSpeechProcessor& operator=(ClientSpeechProcessor&&) = delete;
	    virtual ~ClientSpeechProcessor();

        void SetOutgoingPacketCallback(ClientSpeechPacketCallback OutgoingPacketCb) { _OutgoingPacketCb = OutgoingPacketCb; }
        void SetIncomingPacketCallback(ClientSpeechPacketCallback IncomingPacketCb) { _IncomingPacketCb = IncomingPacketCb; }

        virtual void StartAudioSession(const Inworld::Routing& Routing, const AudioSessionStartPayload& Payload);
        virtual void StopAudioSession(const Inworld::Routing& Routing);
        void SendSoundMessage(const Inworld::Routing& Routing, const std::string& Data);
        void SendSoundMessageWithAEC(const Inworld::Routing& Routing, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData);

        void EnableAudioDump(const std::string& FilePath);
        void DisableAudioDump();
	    
    protected:
	    bool StartActualAudioSession();
	    bool StopActualAudioSession();
	    virtual void ProcessAudio(const std::string& Data) = 0;
	    void SendAudio(const std::string& Data);
	    virtual void ClearState();

        AECFilter _EchoFilter;
	    AudioSessionStartPayload _AudioSessionPayload;
	    Routing _Routing;
	    bool _bSessionActive = false;

#ifdef INWORLD_AUDIO_DUMP
        AsyncRoutine _AsyncAudioDumper;
        SharedQueue<std::string> _AudioChunksToDump;
        bool _bDumpAudio = false;
        std::string _AudioDumpFileName = "C:/Tmp/AudioDump.wav";
#endif

        ClientSpeechPacketCallback _OutgoingPacketCb;
        ClientSpeechPacketCallback _IncomingPacketCb;
    };

    struct ClientSpeechOptions_Default : public ClientSpeechOptions {};

    class INWORLD_EXPORT ClientSpeechProcessor_Default : public ClientSpeechProcessor
    {
    public:
        ClientSpeechProcessor_Default(const ClientSpeechOptions_Default& Options)
            : ClientSpeechProcessor(Options)
        {}
	    virtual ~ClientSpeechProcessor_Default() override = default;
        virtual void StartAudioSession(const Inworld::Routing& Routing, const AudioSessionStartPayload& Payload) override;
        virtual void StopAudioSession(const Inworld::Routing& Routing) override;

    protected:
        virtual void ProcessAudio(const std::string& Data) override;
    };

    struct ClientSpeechOptions_VAD : public ClientSpeechOptions
    {
        std::string VADModelPath;

        float VADProbThreshhold = 0.3f;
        uint8_t VADBufferChunksNum = 5;
        uint8_t VADSilenceChunksNum = 5;
    };

    class INWORLD_EXPORT ClientSpeechProcessor_VAD : public ClientSpeechProcessor
    {
    public:
        ClientSpeechProcessor_VAD(const ClientSpeechOptions_VAD& Options);
	    virtual ~ClientSpeechProcessor_VAD() override;
        
    protected:
        virtual void ProcessAudio(const std::string& Data) override;
        virtual void ClearState() override;
        virtual void HandleVoiceDetected(const std::string& Data) = 0;
        virtual void HandleSilenceDetected(const std::string& Data) = 0;
        virtual void BufferAudio(const std::string& Data) {}
        bool DetectVoice(const std::string& Data);
        
        uint8_t _VADSilenceCounter = 0;

        float VADProbThreshhold = 0.3f;
        uint8_t VADBufferChunksNum = 5;
        uint8_t VADSilenceChunksNum = 5;
    };

    struct ClientSpeechOptions_VAD_DetectOnly : public ClientSpeechOptions_VAD {};

    class INWORLD_EXPORT ClientSpeechProcessor_VAD_DetectOnly : public ClientSpeechProcessor_VAD
    {
    public:
        ClientSpeechProcessor_VAD_DetectOnly(const ClientSpeechOptions_VAD_DetectOnly& Options)
            : ClientSpeechProcessor_VAD(Options)
        {}
        virtual ~ClientSpeechProcessor_VAD_DetectOnly() override  = default;
        virtual void StartAudioSession(const Inworld::Routing& Routing, const AudioSessionStartPayload& Payload) override;
        virtual void StopAudioSession(const Inworld::Routing& Routing) override;

    protected:
        virtual void ProcessAudio(const std::string& Data) override;
        virtual void HandleVoiceDetected(const std::string& Data) override;
        virtual void HandleSilenceDetected(const std::string& Data) override;

        bool _bVoiceDetected = false;
    };

    struct ClientSpeechOptions_VAD_DetectAndFilterAudio : public ClientSpeechOptions_VAD {};

    class INWORLD_EXPORT ClientSpeechProcessor_VAD_DetectAndFilterAudio : public ClientSpeechProcessor_VAD
    {
    public:
        ClientSpeechProcessor_VAD_DetectAndFilterAudio(const ClientSpeechOptions_VAD_DetectAndFilterAudio& Options)
            : ClientSpeechProcessor_VAD(Options)
        {}
        virtual ~ClientSpeechProcessor_VAD_DetectAndFilterAudio() override = default;
    protected:
        virtual void HandleVoiceDetected(const std::string& Data) override;
        virtual void HandleSilenceDetected(const std::string& Data) override;
        virtual void BufferAudio(const std::string& Data) override;
        virtual void ClearState() override;
        
        std::queue<std::string> _AudioQueue;
    };
}
