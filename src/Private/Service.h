/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include "ProtoDisableWarning.h"

#include <atomic>
#include <memory>
#include <functional>

#include "grpcpp/impl/codegen/status.h"
#include "grpcpp/impl/codegen/sync_stream.h"
#include "grpcpp/create_channel.h"
#include "ai/inworld/engine/world-engine.grpc.pb.h"
#include "ai/inworld/engine/v1/feedback.grpc.pb.h"
#include "ai/inworld/engine/v1/state_serialization.grpc.pb.h"

#include "Utils/Utils.h"
#include "Utils/SharedQueue.h"
#include "Client.h"
#include "Define.h"
#include "Packets.h"
#include "Runnable.h"
#include "Types.h"

#ifdef INWORLD_AUDIO_DUMP
#include "Utils/AudioSessionDumper.h"
#endif

namespace InworldEngine = ai::inworld::engine;
namespace InworldEngineV1 = ai::inworld::engine::v1;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

namespace Inworld
{
	using ClientStream = ::grpc::ClientAsyncReaderWriter<InworldPackets::InworldPacket, InworldPackets::InworldPacket>;

	enum class TagType : uint8_t
	{
		OpenSession,
		CloseSession,
		Read,
		Write
	};
	
	class INWORLD_EXPORT RunnableMessaging : public Runnable
	{
	public:
		RunnableMessaging(std::unique_ptr<ClientStream>& ClientStream, std::mutex& Mutex, std::atomic<bool>& HasReaderWriterFinished, bool& WriteWaiting, bool& ReadWaiting, SharedQueue<std::shared_ptr<Inworld::Packet>>& Packets)
			: _ClientStream(ClientStream)
			, _HasReaderWriterFinished(HasReaderWriterFinished)
			, _Packets(Packets)
			, _WriteWaiting(WriteWaiting)
			, _ReadWaiting(ReadWaiting)
			, _Mutex(Mutex)
		{}
		virtual ~RunnableMessaging() = default;

	protected:
		std::unique_ptr<ClientStream>& _ClientStream;
		std::atomic<bool>& _HasReaderWriterFinished;

		SharedQueue<std::shared_ptr<Inworld::Packet>>& _Packets;

		bool& _WriteWaiting;
		bool& _ReadWaiting;

		std::mutex& _Mutex;

		TagType _OpenSessionTag = TagType::OpenSession;
		TagType _CloseSessionTag = TagType::CloseSession;
		TagType _ReadTag = TagType::Read;
		TagType _WriteTag = TagType::Write;
	};

	class INWORLD_EXPORT RunnableRead : public RunnableMessaging
	{
	public:
		RunnableRead(std::unique_ptr<ClientStream>& ClientStream, std::mutex& Mutex, std::atomic<bool>& HasReaderWriterFinished, bool& WriteWaiting, bool& ReadWaiting, std::condition_variable& ReadCV, SharedQueue<std::shared_ptr<Inworld::Packet>>& IncomingPackets, std::function<void()> ProcessedCallback = nullptr)
			: RunnableMessaging(ClientStream, Mutex, HasReaderWriterFinished, WriteWaiting, ReadWaiting, IncomingPackets)
			,	_ReadCV(ReadCV)
			,	_ProcessedCallback(ProcessedCallback)
		{}
		virtual ~RunnableRead() override = default;
		virtual void Run() override;

	protected:
		std::condition_variable& _ReadCV;
		std::function<void()> _ProcessedCallback;
	};

	class INWORLD_EXPORT RunnableWrite : public RunnableMessaging
	{
	public:
		RunnableWrite(std::unique_ptr<ClientStream>& ClientStream, std::mutex& Mutex, std::atomic<bool>& HasReaderWriterFinished, bool& WriteWaiting, bool& ReadWaiting, std::condition_variable& WriteCV, SharedQueue<std::shared_ptr<Inworld::Packet>>& OutgoingPackets)
			: RunnableMessaging(ClientStream, Mutex, HasReaderWriterFinished, WriteWaiting, ReadWaiting, OutgoingPackets)
			,	_WriteCV(WriteCV)
		{} 
		virtual ~RunnableWrite() override = default;

		virtual void Run() override;

	protected:
		std::condition_variable& _WriteCV;
	};

	template<typename TService>
	class INWORLD_EXPORT Service
	{
	public:
		explicit Service(const std::string& ServerUrl)
			: _ServerUrl(ServerUrl)
		{}

		void Cancel()
		{
			if (_Context)
			{
				_Context->TryCancel();
			}
		}

	protected:
		std::unique_ptr<typename TService::Stub>& CreateStub()
		{
			grpc::SslCredentialsOptions SslCredentialsOptions;
			SslCredentialsOptions.pem_root_certs = Utils::GetSslRootCerts();
			_Stub = TService::NewStub(grpc::CreateChannel(_ServerUrl, grpc::SslCredentials(SslCredentialsOptions)));
			return _Stub;
		}

		std::unique_ptr<ClientContext>& UpdateContext(const ClientHeaderData& Metadata)
		{
			_Context = std::make_unique<ClientContext>();
			for (auto& Data : Metadata)
			{
				_Context->AddMetadata(Data.first, Data.second);
			}
			return _Context;
		}

		std::string _ServerUrl;
		std::unique_ptr<typename TService::Stub> _Stub;
		std::unique_ptr<ClientContext> _Context;
	};

	template<typename TService, class TResponse>
	class INWORLD_EXPORT RunnableRequest : public Runnable, public Service<TService>
	{
	public:
		RunnableRequest(const std::string& ServerUrl, std::function<void(const grpc::Status& Status, const TResponse& Response)> Callback = nullptr)
			: Service<TService>(ServerUrl)
			, _Callback(Callback)
		{}
		virtual ~RunnableRequest() = default;

		virtual grpc::Status RunProcess() = 0;

		virtual void Run() override
		{
			_Status = RunProcess();
			_IsDone = true;

			if (_Callback)
			{
				_Callback(_Status, _Response);
			}
		}

		virtual void Deinitialize() override
		{
			Service<TService>::Cancel();
		}

		grpc::Status& GetStatus() { return _Status; }
		TResponse& GetResponse() { return _Response; }

	protected:

		grpc::Status _Status;
		TResponse _Response;

		std::function<void(const grpc::Status& Status, const TResponse& Response)> _Callback;
	};

	class INWORLD_EXPORT RunnableGenerateSessionToken : public RunnableRequest<InworldEngine::WorldEngine, InworldEngine::AccessToken>
	{
	public:
		RunnableGenerateSessionToken(const std::string& ServerUrl, const std::string& Resource, const std::string& ApiKey, const std::string& ApiSecret, std::function<void(const grpc::Status&, const InworldEngine::AccessToken&)> Callback = nullptr)
			: RunnableRequest(ServerUrl, Callback)
			, _Resource(Resource)
			, _ApiKey(ApiKey)
			, _ApiSecret(ApiSecret)
		{}

		virtual ~RunnableGenerateSessionToken() override = default;

		virtual grpc::Status RunProcess() override;

	private:
		std::string GenerateHeader() const;

		std::string _Resource;
		std::string _ApiKey;
		std::string _ApiSecret;
	};

	class INWORLD_EXPORT RunnableGetSessionState : public RunnableRequest<InworldEngineV1::StateSerialization, InworldEngineV1::SessionState>
	{
	public:
		RunnableGetSessionState(const std::string& ServerUrl, const std::string& Token, const std::string& SessionName, std::function<void(const grpc::Status&, const InworldEngineV1::SessionState&)> Callback = nullptr)
			: RunnableRequest(ServerUrl, Callback)
			, _Token(Token)
			, _SessionName(SessionName)
		{}

		virtual ~RunnableGetSessionState() override = default;

		virtual grpc::Status RunProcess() override;

	private:
		std::string _Token;
		std::string _SessionName;
	};

class INWORLD_EXPORT RunnableCreateInteractionFeedback : public RunnableRequest<InworldEngineV1::Feedback, InworldEngineV1::InteractionFeedback>
	{
	public:
		RunnableCreateInteractionFeedback(const std::string& ServerUrl, const std::string& SessionId, const std::string& Token, const std::string& ResourceId, const InteractionFeedback& Feedback, std::function<void(const grpc::Status&, const InworldEngineV1::InteractionFeedback&)> Callback = nullptr)
			: RunnableRequest(ServerUrl, Callback)
			, _SessionId(SessionId)
			, _Token(Token)
			, _ResourceId(ResourceId)
			, _Feedback(Feedback)
		{
			if (_Feedback.DislikeReasons.size() == 0)
			{
				_Feedback.DislikeReasons.insert(InteractionFeedback::DislikeType::UNSPECIFIED);
			}
		}

		virtual ~RunnableCreateInteractionFeedback() override = default;

		virtual grpc::Status RunProcess() override;

	private:
		std::string _SessionId;
		std::string _Token;
		std::string _ResourceId;
		InteractionFeedback _Feedback;
	};

	class INWORLD_EXPORT ServiceSession : public Service<InworldEngine::WorldEngine>
	{
	public:
		ServiceSession(const std::string& Token, const std::string& SessionId, const std::string& ServerUrl)
			: Service(ServerUrl)
			, _Token(Token)
			, _SessionId(SessionId)
		{}
		
		std::unique_ptr<ClientStream> OpenSessionAsync(const ClientHeaderData& Metadata);

		std::unique_ptr<grpc::CompletionQueue>& CompletionQueue() { return _CompletionQueue; }
		
		void SetToken(const std::string& Token) { _Token = Token; }

		TagType _OpenSessionTag = TagType::OpenSession;

	private:
		std::unique_ptr<ClientContext>& Context();
		std::unique_ptr<grpc::CompletionQueue> _CompletionQueue;
		std::string _Token;
		std::string _SessionId;
	};

	class INWORLD_EXPORT RunnableRpcHandler : public RunnableMessaging
	{
	public:
		RunnableRpcHandler(std::unique_ptr<ServiceSession>& Session, std::unique_ptr<ClientStream>& ClientStream, std::mutex& Mutex, std::unique_ptr<grpc::CompletionQueue>& CompletionQueue, std::atomic<bool>& HasReaderWriterFinished, std::atomic<bool>& IsRunning, std::atomic<bool>& OpeningClientStream, bool& WriteWaiting, bool& ReadWaiting, std::condition_variable& ReadCV, std::condition_variable& WriteCV, SharedQueue<std::shared_ptr<Inworld::Packet>>& OutgoingPackets, std::function<void()> SessionOpenCallback, std::function<void(const grpc::Status&)> ErrorCallback)
			: RunnableMessaging(ClientStream, Mutex, HasReaderWriterFinished, WriteWaiting, ReadWaiting, OutgoingPackets)
			,	_Session(Session)
			,	_CompletionQueue(CompletionQueue)
			,	_OpeningClientStream(OpeningClientStream)
			,	_IsRunning(IsRunning)
			,	_ReadCV(ReadCV)
			,	_WriteCV(WriteCV)
			,	_EndCallback(ErrorCallback)
			,	_SessionOpenCallback(SessionOpenCallback)
		
		{} 
		virtual ~RunnableRpcHandler() override = default;

		virtual void Run() override;

		void Deinitialize() override
		{
			if(!_HasReaderWriterFinished)
			{
				_Session->Cancel();
			}
		}

	protected:
		std::unique_ptr<ServiceSession>& _Session;
		std::unique_ptr<grpc::CompletionQueue>& _CompletionQueue;
		std::condition_variable& _ReadCV;
		std::condition_variable& _WriteCV;
		std::atomic<bool>& _OpeningClientStream;
		std::atomic<bool>& _IsRunning;

		std::function<void()> _SessionOpenCallback;
		std::function<void(const grpc::Status&)> _EndCallback;

		Status _Status;
		bool _ShuttingDown = false;
	};

#ifdef INWORLD_AUDIO_DUMP
	class INWORLD_EXPORT RunnableAudioDumper : public Inworld::Runnable
	{
	public:
		RunnableAudioDumper(SharedQueue<std::string>& InAudioChuncks, const std::string& InFileName)
			: FileName(InFileName)
			  , AudioChuncks(InAudioChuncks)
		{}

		std::string FileName;
		virtual void Run() override;
	    virtual void Deinitialize() override;

	private:
		AudioSessionDumper AudioDumper;
		SharedQueue<std::string>& AudioChuncks;
	};
#endif
}
