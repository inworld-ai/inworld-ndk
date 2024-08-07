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
    using ClientSpeechVADCallback = std::function<void(bool bVoiceDetected)>;

    class ClientSpeechProcessor;
    class Client;
    struct ClientSpeechOptions
    {
        enum class SpeechMode : uint8_t
        {
            Default = 0,                // no VAD
            VAD_DetectOnly = 1,         // detect voice to send callbacks, but send all player audio
            VAD_DetectAndSendAudio = 2, // detect voice to send callbacks and send only voice audio
        };  
        SpeechMode Mode = SpeechMode::Default;

        std::string VADModelPath;

        float VADProbThreshhold = 0.3f;
        uint8_t VADBufferChunksNum = 5;
        uint8_t VADSilenceChunksNum = 5;

        ClientSpeechPacketCallback PacketCb;
        ClientSpeechVADCallback VADCb;
        ClientSpeechVADCallback VADImmediateCb;

    private:
        std::unique_ptr<ClientSpeechProcessor> CreateSpeechProcessor();
        friend class Client;
    };
    
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
        ClientSpeechOptions _Options;
	    AudioSessionStartPayload _AudioSessionPayload;
	    Routing _Routing;
	    bool _bSessionActive = false;

#ifdef INWORLD_AUDIO_DUMP
        AsyncRoutine _AsyncAudioDumper;
        SharedQueue<std::string> _AudioChunksToDump;
        bool _bDumpAudio = false;
        std::string _AudioDumpFileName = "C:/Tmp/AudioDump.wav";
#endif
    };

    class INWORLD_EXPORT ClientSpeechProcessor_Default : public ClientSpeechProcessor
    {
    public:
        ClientSpeechProcessor_Default(const ClientSpeechOptions& Options) : ClientSpeechProcessor(Options) {}
	    virtual ~ClientSpeechProcessor_Default() override = default;
        virtual void StartAudioSession(const Inworld::Routing& Routing, const AudioSessionStartPayload& Payload) override;
        virtual void StopAudioSession(const Inworld::Routing& Routing) override;

    protected:
        virtual void ProcessAudio(const std::string& Data) override;
    };

    class INWORLD_EXPORT ClientSpeechProcessor_VAD : public ClientSpeechProcessor
    {
    public:
        ClientSpeechProcessor_VAD(const ClientSpeechOptions& Options);
	    virtual ~ClientSpeechProcessor_VAD() override;
        
    protected:
        virtual void ProcessAudio(const std::string& Data) override;
        virtual void ClearState() override;
        virtual void HandleVoiceDetected(const std::string& Data) = 0;
        virtual void HandleSilenceDetected(const std::string& Data) = 0;
        virtual void BufferAudio(const std::string& Data) {}
        bool DetectVoice(const std::string& Data);
        
        uint8_t _VADSilenceCounter = 0;
    };

    class INWORLD_EXPORT ClientSpeechProcessor_VAD_DetectOnly : public ClientSpeechProcessor_VAD
    {
    public:
        ClientSpeechProcessor_VAD_DetectOnly(const ClientSpeechOptions& Options) : ClientSpeechProcessor_VAD(Options) {}
        virtual ~ClientSpeechProcessor_VAD_DetectOnly() override  = default;
        virtual void StartAudioSession(const Inworld::Routing& Routing, const AudioSessionStartPayload& Payload) override;
        virtual void StopAudioSession(const Inworld::Routing& Routing) override;

    protected:
        virtual void ProcessAudio(const std::string& Data) override;
        virtual void HandleVoiceDetected(const std::string& Data) override;
        virtual void HandleSilenceDetected(const std::string& Data) override;

        bool _bVoiceDetected = false;
    };

    class INWORLD_EXPORT ClientSpeechProcessor_VAD_DetectAndSendAudio : public ClientSpeechProcessor_VAD
    {
    public:
        ClientSpeechProcessor_VAD_DetectAndSendAudio(const ClientSpeechOptions& Options) : ClientSpeechProcessor_VAD(Options) {}
        virtual ~ClientSpeechProcessor_VAD_DetectAndSendAudio() override = default;
    protected:
        virtual void HandleVoiceDetected(const std::string& Data) override;
        virtual void HandleSilenceDetected(const std::string& Data) override;
        virtual void BufferAudio(const std::string& Data) override;
        virtual void ClearState() override;
        
        std::queue<std::string> _AudioQueue;
    };
}
