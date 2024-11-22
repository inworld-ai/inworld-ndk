/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <string>
#include <functional>
#include <type_traits>
#include <memory>

#include "Define.h"
#include "Types.h"
#include "Packets.h"
#include "Utils/SharedQueue.h"
#include "AsyncRoutine.h"
#include "Utils/PerceivedLatencyTracker.h"
#include "AECFilter.h"
#include "ClientSpeechProcessor.h"

using PacketQueue = Inworld::SharedQueue<std::shared_ptr<Inworld::Packet>>;

// don't include grpc related stuff to public headers
namespace ai { namespace inworld { namespace packets {
	class InworldPacket;
}}}
namespace InworldPackets = ai::inworld::packets;

namespace grpc
{
	template <class W, class R>
	class ClientReaderWriter;
}

namespace Inworld
{
	class ServiceSession;
	using ClientStream = ::grpc::ClientReaderWriter<InworldPackets::InworldPacket, InworldPackets::InworldPacket>;

	// use for grpc related Client members
	class IClientService
	{
	public:
		virtual ~IClientService() = default;
		virtual std::unique_ptr<ServiceSession>& Session() = 0;
		virtual std::unique_ptr<ClientStream>& Stream() = 0;
		virtual void OpenSession(const ClientHeaderData& Metadata) = 0;
	};

	struct INWORLD_EXPORT SdkInfo
	{
		std::string Type;
		std::string Subtype;
		std::string Version;
		std::string OS;
		std::string Description;
	};

	using Capabilities = ControlEventSessionConfiguration::Capabilities;
	using UserConfiguration = ControlEventSessionConfiguration::UserConfiguration;
	struct INWORLD_EXPORT ClientOptions
	{
		Capabilities Capabilities;
		UserConfiguration UserConfig;
		std::string ServerUrl;
		std::string Resource;
		std::string ApiKey;
		std::string ApiSecret;
		std::string Base64;
		std::string ProjectName;
		std::string GameSessionId;
		ClientHeaderData Metadata;
	};

	struct INWORLD_EXPORT ErrorDetails
	{
		ErrorDetails() = default;
		ErrorDetails(const std::string& data);

		enum class ErrorType : uint8_t
		{
			SessionTokenExpired,
			SessionTokenInvalid,
			SessionResourcesExhaused,
			BillingTokensExhausted,
			AccountDisabled,
			SessionInvalid,
			ResourceNotFound,
			SafetyViolation,
			SessionExpired
		};
		ErrorType Error = ErrorType::SessionTokenExpired;

		enum class ReconnectionType : uint8_t
		{
			Undefined,
			NoRetry,
			Immediate,
			Timeout
		};
		ReconnectionType Reconnect = ReconnectionType::Undefined;

		int64_t ReconnectTime;

		int32_t MaxRetries = 0;

		struct ResourceNotFoundDetails
		{
			std::string Id;
			enum class ResourceType : uint8_t
			{
				Undefined,
				Conversation,
			};
			ResourceType Type;
		};
		ResourceNotFoundDetails ResourceNotFoundPayload;
	};

	// use for client lifecycle
	class Client;
	INWORLD_EXPORT std::unique_ptr<Client> CreateClient();
	INWORLD_EXPORT void DestroyClient(std::unique_ptr<Client> client);

	class INWORLD_EXPORT Client final : public PacketVisitor
	{
	public:
		enum class ConnectionState : uint8_t 
		{
			Idle,
			Connecting,
			Connected,
			Failed,
			Paused,
			Disconnected,
			Reconnecting
		};

		Client();
		virtual ~Client();

#pragma region Lifetime
		// callbacks will not be called on calling thread
		void InitClientAsync(const SdkInfo& SdkInfo, std::function<void(ConnectionState)> ConnectionStateCallback, std::function<void(std::shared_ptr<Inworld::Packet>)> PacketCallback);
		void StartClientFromSceneId(const std::string& SceneId);
		void StartClientFromSave(const SessionSave& Save);
		void StartClientFromToken(const SessionToken& Info);
		void PauseClient();
		void ResumeClient();
		void StopClient();
		void DestroyClient();
#pragma endregion

#pragma region Conversations
		std::shared_ptr<ControlEventConversationUpdate> UpdateConversation(const std::vector<std::string>& AgentIds, const std::string& ConversationId = "", bool bIncludePlayer = true);

		std::shared_ptr<TextEvent> SendTextMessage(const Inworld::Routing& Routing, const std::string& Text);
		std::shared_ptr<TextEvent> SendTextMessage(const std::string& AgentId, const std::string& Text);
	    std::shared_ptr<TextEvent> SendTextMessageToConversation(const std::string& ConversationId, const std::string& Text);
		
	    void StartAudioSession(const Inworld::Routing& Routing, const AudioSessionStartPayload& Payload);
	    void StartAudioSession(const std::string& AgentId, const AudioSessionStartPayload& Payload);
	    void StartAudioSessionInConversation(const std::string& ConversationId, const AudioSessionStartPayload& Payload);
		
	    void StopAudioSession(const Inworld::Routing& Routing);
	    void StopAudioSession(const std::string& AgentId);
	    void StopAudioSessionInConversation(const std::string& ConversationId);

		void SendSoundMessage(const Inworld::Routing& Routing, const std::string& Data);
		void SendSoundMessage(const std::string& AgentId, const std::string& Data);
		void SendSoundMessageToConversation(const std::string& ConversationId, const std::string& Data);
		
		void SendSoundMessageWithAEC(const Inworld::Routing& Routing, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData);
		void SendSoundMessageWithAEC(const std::string& AgentId, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData);
		void SendSoundMessageWithAECToConversation(const std::string& ConversationId, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData);

		std::shared_ptr<CustomEvent> SendCustomEvent(const Inworld::Routing& Routing, const std::string& Name, const std::unordered_map<std::string, std::string>& Params);
		std::shared_ptr<CustomEvent> SendCustomEvent(const std::string& AgentId, const std::string& Name, const std::unordered_map<std::string, std::string>& Params);
		std::shared_ptr<CustomEvent> SendCustomEventToConversation(const std::string& ConversationId, const std::string& Name, const std::unordered_map<std::string, std::string>& Params);
		
		std::shared_ptr<ActionEvent> SendNarrationEvent(const Inworld::Routing& Routing, const std::string& Content);
		std::shared_ptr<ActionEvent> SendNarrationEvent(const std::string& AgentId, const std::string& Content);

		std::shared_ptr<CancelResponseEvent> CancelResponse(const Inworld::Routing& Routing, const std::string& InteractionId, const std::vector<std::string>& UtteranceIds);
		std::shared_ptr<CancelResponseEvent> CancelResponse(const std::string& AgentId, const std::string& InteractionId, const std::vector<std::string>& UtteranceIds);
#pragma endregion

#pragma region Entities
		void CreateOrUpdateItems(const std::vector<CreateOrUpdateItemsOperationEvent::EntityItem>& Items, const std::vector<std::string>& AddToEntities);
		void RemoveItems(const std::vector<std::string>& ItemIds);
		void AddItemsInEntities(const std::vector<std::string>& ItemIds, const std::vector<std::string>& EntityNames);
		void RemoveItemsInEntities(const std::vector<std::string>& ItemIds, const std::vector<std::string>& EntityNames);
		void ReplaceItemsInEntities(const std::vector<std::string>& ItemIds, const std::vector<std::string>& EntityNames);
#pragma endregion

#pragma region Unitary Session
		void LoadScene(const std::string& Scene);
		void LoadCharacters(const std::vector<std::string>& Names);
		void UnloadCharacters(const std::vector<std::string>& Names);

		void LoadCapabilities(const Capabilities& Capabilities);
		void LoadUserConfiguration(const UserConfiguration& UserConfig);
		
		// the callback is not called on calling thread for Async methods
		void SaveSessionStateAsync(std::function<void(const SessionSave&, bool)> Callback);
#pragma endregion

		void SendFeedbackAsync(std::string& InteractionId, const InteractionFeedback& Feedback, std::function<void(const std::string&, bool)> Callback = nullptr);

		void GenerateToken(std::function<void()> RefreshTokenCallback);

	private:
		template<class T, class U>
		void InitSpeechProcessor(const T& Options);
	public:
		template<class T>
		void InitSpeechProcessor(const T& Options);
		template<>
		void InitSpeechProcessor(const ClientSpeechOptions_Default& Options);
		template<>
		void InitSpeechProcessor(const ClientSpeechOptions_VAD_DetectOnly& Options);
		template<>
		void InitSpeechProcessor(const ClientSpeechOptions_VAD_DetectAndFilterAudio& Options);

		void DestroySpeechProcessor();

	    void EnableAudioDump(const std::string& FileName = "");
	    void DisableAudioDump();
		
		ConnectionState GetConnectionState() const { return _ConnectionState; }
		inline bool GetConnectionError(std::string& OutErrorMessage, int32_t& OutErrorCode, ErrorDetails& OutErrorDetails) const
		{
			OutErrorMessage = _ErrorMessage;
			OutErrorCode = _ErrorCode;
			OutErrorDetails = _ErrorDetails;
			return _ErrorCode != 0;
		}

		void SetPerceivedLatencyTrackerCallback(std::function<void(const std::string&, uint32_t)> PerceivedLatencyCallback) { _OnPerceivedLatencyCallback = PerceivedLatencyCallback; }
		void ClearPerceivedLatencyTrackerCallback() { _OnPerceivedLatencyCallback = nullptr; }
		
		const SessionToken& GetSessionToken() const;
		void SetOptions(const ClientOptions& options);	
		const ClientOptions& GetOptions() const;

		virtual void Visit(const ControlEventCurrentSceneStatus& Event) override;
		virtual void Visit(const PingEvent& Event) override;
		virtual void Visit(const LogEvent& Event) override;

	protected:
		void SendPacket(std::shared_ptr<Inworld::Packet> Packet);
		void PushPacket(std::shared_ptr<Inworld::Packet> Packet);
		void RecvPacket(std::shared_ptr<Inworld::Packet> Packet);

		void StartClientStream();
		void PauseClientStream();
		void ResumeClientStream();
		void StopClientStream();
		void SetConnectionState(ConnectionState State);

		std::function<void(std::shared_ptr<Inworld::Packet>)> _OnPacketCallback;
		ClientOptions _ClientOptions;
		std::string _SceneId;
		SessionToken _SessionToken;
		SdkInfo _SdkInfo;

	private:
		void StartSession();
		void TryToStartReadTask();
		void TryToStartWriteTask();

		template <typename T>
		void ControlSession(typename T::Data D)
		{
			static_assert(std::is_base_of<SessionControlEvent, T>::value, "ControlSession can be used only with SessionControlEvents");
			PushPacket(std::make_shared<T>(D));
		}

		std::function<void()> _OnGenerateTokenCallback;
		std::function<void(ConnectionState)> _OnConnectionStateChangedCallback;
		std::function<void(const std::string&, uint32_t)> _OnPerceivedLatencyCallback;

		std::atomic<bool> _bHasClientStreamFinished = false;

		AsyncRoutine _AsyncReadTask;
		AsyncRoutine _AsyncWriteTask;
		AsyncRoutine _AsyncGenerateTokenTask;
		AsyncRoutine _AsyncGetSessionState;
		AsyncRoutine _AsyncSendFeedback;
		
		std::unique_ptr<IClientService> _Service;

		PacketQueue _IncomingPackets;
		PacketQueue _OutgoingPackets;

	    std::unique_ptr<ClientSpeechProcessor> _SpeechProcessor;

		std::atomic<bool> _bPendingIncomingPacketFlush = false;

		ConnectionState _ConnectionState = ConnectionState::Idle;
		std::string _ErrorMessage = std::string();
		int32_t _ErrorCode = 0;
		ErrorDetails _ErrorDetails;

		PerceivedLatencyTracker _LatencyTracker;
	};
}

