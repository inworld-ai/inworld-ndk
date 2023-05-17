/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <string>
#include <functional>

#include <future>
#include "Packets.h"
#include "Utils/SharedQueue.h"
#include "AsyncRoutine.h"
#include "AECFilter.h"


using PacketQueue = Inworld::SharedQueue<std::shared_ptr<Inworld::Packet>>;

namespace Inworld
{
	struct INWORLDAINDK_API ClientOptions
	{
		std::string AuthUrl;
		std::string LoadSceneUrl;
		std::string SceneName;
		std::string ApiKey;
		std::string ApiSecret;
		std::string PlayerName;
	};

	struct INWORLDAINDK_API SessionInfo
	{
		std::string SessionId;
		std::string Token;
		int64_t ExpirationTime;

		bool IsValid() const;
	};

	struct INWORLDAINDK_API AgentInfo
	{
		std::string BrainName;
		std::string AgentId;
		std::string GivenName;
	};

	class INWORLDAINDK_API ClientBase
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

		ClientBase() = default;
		virtual ~ClientBase() = default;

		void SendPacket(std::shared_ptr<Inworld::Packet> Packet);

		virtual std::shared_ptr<TextEvent> SendTextMessage(const std::string& AgentId, const std::string& Text);
		virtual std::shared_ptr<DataEvent> SendSoundMessage(const std::string& AgentId, const std::string& Data);
		virtual std::shared_ptr<DataEvent> SendSoundMessageWithAEC(const std::string& AgentId, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData);
		virtual std::shared_ptr<CustomEvent> SendCustomEvent(std::string AgentId, const std::string& Name);

		virtual void CancelResponse(const std::string& AgentId, const std::string& InteractionId, const std::vector<std::string>& UtteranceIds);

		virtual void StartAudioSession(const std::string& AgentId);
		virtual void StopAudioSession(const std::string& AgentId);

		virtual void InitClient(std::string UserId,	std::string ClientId, std::string ClientVer, std::function<void(ConnectionState)> ConnectionStateCallback, std::function<void(std::shared_ptr<Inworld::Packet>)> PacketCallback);
		virtual void StartClient(const ClientOptions& Options, const SessionInfo& Info, std::function<void(const std::vector<AgentInfo>&)> LoadSceneCallback);
		virtual void PauseClient();
		virtual void ResumeClient();
		virtual void StopClient();
		virtual void DestroyClient();

		virtual void GenerateToken(std::function<void()> RefreshTokenCallback);

		ConnectionState GetConnectionState() const { return _ConnectionState; }
		bool GetConnectionError(std::string& OutErrorMessage, int32_t& OutErrorCode) const;

		virtual void Update() {}

	protected:
		virtual void AddTaskToMainThread(std::function<void()> Task) = 0;

		template<typename TAsyncRoutine>
		void CreateAsyncRoutines()
		{
			_AsyncReadTask = std::make_unique<TAsyncRoutine>();
			_AsyncWriteTask = std::make_unique<TAsyncRoutine>();
			_AsyncLoadSceneTask = std::make_unique<TAsyncRoutine>();
			_AsyncGenerateTokenTask = std::make_unique<TAsyncRoutine>();
		}

	private:
		void LoadScene();
		void OnSceneLoaded(const grpc::Status& Status, const InworldEngine::LoadSceneResponse& Response);

		void SetConnectionState(ConnectionState State);
		
		void StartReaderWriter();
		void StopReaderWriter();
		void TryToStartReadTask();
		void TryToStartWriteTask();

		std::function<void()> _OnGenerateTokenCallback;
		std::function<void(const std::vector<AgentInfo>&)> _OnLoadSceneCallback;
		std::function<void(ConnectionState)> _OnConnectionStateChangedCallback;
		std::function<void(std::shared_ptr<Inworld::Packet>)> _OnPacketCallback;

		std::unique_ptr<ReaderWriter> _ReaderWriter;
		std::atomic<bool> _bHasReaderWriterFinished = false;

		std::unique_ptr<IAsyncRoutine> _AsyncReadTask;
		std::unique_ptr<IAsyncRoutine> _AsyncWriteTask;
		std::unique_ptr<IAsyncRoutine> _AsyncGenerateTokenTask;
		std::unique_ptr<IAsyncRoutine> _AsyncLoadSceneTask;

		PacketQueue _IncomingPackets;
		PacketQueue _OutgoingPackets;

		std::atomic<bool> _bPendingIncomingPacketFlush = false;

		ConnectionState _ConnectionState = ConnectionState::Idle;
		std::string _ErrorMessage = std::string();
		int32_t _ErrorCode = grpc::StatusCode::OK;

		std::string _UserId;
		std::string _ClientId;
		std::string _ClientVer;

		ClientOptions _ClientOptions;
		SessionInfo _SessionInfo;

		AECFilter _EchoFilter;
	};

	class INWORLDAINDK_API Client : public ClientBase
	{
	public:
		Client()
		{
			CreateAsyncRoutines<Inworld::AsyncRoutine>();
		}

		virtual void Update() override;

	protected:
		virtual void AddTaskToMainThread(std::function<void()> Task) override;

	private:
		void ExecutePendingTasks();

		SharedQueue<std::function<void()>> _MainThreadTasks;
	};
}

