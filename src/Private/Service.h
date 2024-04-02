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
#include "ai/inworld/studio/v1alpha/tokens.grpc.pb.h"
#include "ai/inworld/studio/v1alpha/users.grpc.pb.h"
#include "ai/inworld/studio/v1alpha/workspaces.grpc.pb.h"
#include "ai/inworld/studio/v1alpha/scenes.grpc.pb.h"
#include "ai/inworld/studio/v1alpha/characters.grpc.pb.h"
#include "ai/inworld/studio/v1alpha/apikeys.grpc.pb.h"
#include "ai/inworld/engine/v1/state_serialization.grpc.pb.h"

#include "Utils/Utils.h"
#include "Utils/SharedQueue.h"
#include "Define.h"
#include "Packets.h"
#include "Runnable.h"

#ifdef INWORLD_AUDIO_DUMP
#include "Utils/AudioSessionDumper.h"
#endif

namespace InworldEngine = ai::inworld::engine;
namespace InworldV1alpha = ai::inworld::studio::v1alpha;
namespace InworldEngineV1 = ai::inworld::engine::v1;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

namespace Inworld
{
	using ClientStream = ::grpc::ClientReaderWriter<InworldPackets::InworldPacket, InworldPackets::InworldPacket>;

	class INWORLD_EXPORT RunnableMessaging : public Runnable
	{
	public:
		RunnableMessaging(ClientStream& ClientStream, std::atomic<bool>& bInHasReaderWriterFinished, SharedQueue<std::shared_ptr<Inworld::Packet>>& Packets, std::function<void(const std::shared_ptr<Inworld::Packet>)> ProcessedCallback = nullptr, std::function<void(const grpc::Status&)> InErrorCallback = nullptr)
			: _ClientStream(ClientStream)
			, _HasReaderWriterFinished(bInHasReaderWriterFinished)
			, _Packets(Packets)
			, _ProcessedCallback(ProcessedCallback)
			, _ErrorCallback(InErrorCallback)
		{}
		virtual ~RunnableMessaging() = default;

	protected:
		ClientStream& _ClientStream;
		std::atomic<bool>& _HasReaderWriterFinished;

		SharedQueue<std::shared_ptr<Inworld::Packet>>& _Packets;
		std::function<void(const std::shared_ptr<Inworld::Packet>)> _ProcessedCallback;
		std::function<void(const grpc::Status&)> _ErrorCallback;
	};

	class INWORLD_EXPORT RunnableRead : public RunnableMessaging
	{
	public:
		RunnableRead(ClientStream& ClientStream, std::atomic<bool>& bHasReaderWriterFinished, SharedQueue<std::shared_ptr<Inworld::Packet>>& Packets, std::function<void(const std::shared_ptr<Inworld::Packet>)> ProcessedCallback = nullptr, std::function<void(const grpc::Status&)> ErrorCallback = nullptr)
			: RunnableMessaging(ClientStream, bHasReaderWriterFinished, Packets, ProcessedCallback, ErrorCallback)
		{}
		virtual ~RunnableRead() = default;

		virtual void Run() override;
	};

	class INWORLD_EXPORT RunnableWrite : public RunnableMessaging
	{
	public:
		RunnableWrite(ClientStream& ClientStream, std::atomic<bool>& bHasReaderWriterFinished, SharedQueue<std::shared_ptr<Inworld::Packet>>& Packets, std::function<void(const std::shared_ptr<Inworld::Packet>)> ProcessedCallback = nullptr, std::function<void(const grpc::Status&)> ErrorCallback = nullptr)
			: RunnableMessaging(ClientStream, bHasReaderWriterFinished, Packets, ProcessedCallback, ErrorCallback)
		{}
		virtual ~RunnableWrite() = default;

		virtual void Run() override;
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
		struct FHeader
		{
			std::string Key;
			std::string Value;
		};

		std::unique_ptr<typename TService::Stub>& CreateStub()
		{
			grpc::SslCredentialsOptions SslCredentialsOptions;
			SslCredentialsOptions.pem_root_certs = Utils::GetSslRootCerts();
			_Stub = TService::NewStub(grpc::CreateChannel(_ServerUrl, grpc::SslCredentials(SslCredentialsOptions)));
			return _Stub;
		}

		std::unique_ptr<ClientContext>& UpdateContext(const std::vector<FHeader>& Headers)
		{
			_Context = std::make_unique<ClientContext>();
			for (auto& Header : Headers)
			{
				_Context->AddMetadata(Header.Key, Header.Value);
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

		virtual ~RunnableGenerateSessionToken() = default;

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

		virtual ~RunnableGetSessionState() = default;

		virtual grpc::Status RunProcess() override;

	private:
		std::string _Token;
		std::string _SessionName;
	};

	class INWORLD_EXPORT ServiceSession : public Service<InworldEngine::WorldEngine>
	{
	public:
		ServiceSession(const std::string& Token, const std::string& SessionId, const std::string& ServerUrl)
			: Service(ServerUrl)
			, _Token(Token)
			, _SessionId(SessionId)
		{}
		
		std::unique_ptr<ClientStream> OpenSession();

		void SetToken(const std::string& Token) { _Token = Token; }

	private:
		std::unique_ptr<ClientContext>& Context();
		
		std::string _Token;
		std::string _SessionId;
	};

	class INWORLD_EXPORT RunnableGenerateUserTokenRequest : public RunnableRequest<InworldV1alpha::Users, InworldV1alpha::GenerateTokenUserResponse>
	{
	public:
		RunnableGenerateUserTokenRequest(const std::string& InFirebaseToken, const std::string& InServerUrl, std::function<void(const grpc::Status& Status, const InworldV1alpha::GenerateTokenUserResponse& Response)> InCallback)
			: RunnableRequest(InServerUrl, InCallback)
			, _FirebaseToken(InFirebaseToken)
		{}
		virtual ~RunnableGenerateUserTokenRequest() = default;

		virtual grpc::Status RunProcess() override;

	private:
		std::string _FirebaseToken;
	};

	class INWORLD_EXPORT RunnableListWorkspacesRequest : public RunnableRequest<InworldV1alpha::Workspaces, InworldV1alpha::ListWorkspacesResponse>
	{
	public:
		RunnableListWorkspacesRequest(const std::string& InInworldToken, const std::string& InServerUrl, std::function<void(const grpc::Status& Status, const InworldV1alpha::ListWorkspacesResponse& Response)> InCallback)
			: RunnableRequest(InServerUrl, InCallback)
			, _InworldToken(InInworldToken)
		{}
		virtual ~RunnableListWorkspacesRequest() = default;

		virtual grpc::Status RunProcess() override;

	private:
		std::string _InworldToken;
	};

	class INWORLD_EXPORT RunnableListScenesRequest : public RunnableRequest<InworldV1alpha::Scenes, InworldV1alpha::ListScenesResponse>
	{
	public:
		RunnableListScenesRequest(const std::string& InInworldToken, const std::string& InServerUrl, const std::string& InWorkspace, std::function<void(const grpc::Status& Status, const InworldV1alpha::ListScenesResponse& Response)> InCallback)
			: RunnableRequest(InServerUrl, InCallback)
			, _InworldToken(InInworldToken)
			, _Workspace(InWorkspace)
		{}
		virtual ~RunnableListScenesRequest() = default;

		virtual grpc::Status RunProcess() override;

	private:
		std::string _InworldToken;
		std::string _Workspace;
	};

	class INWORLD_EXPORT RunnableListCharactersRequest : public RunnableRequest<InworldV1alpha::Characters, InworldV1alpha::ListCharactersResponse>
	{
	public:
		RunnableListCharactersRequest(const std::string& InInworldToken, const std::string& InServerUrl, const std::string& InWorkspace, std::function<void(const grpc::Status& Status, const InworldV1alpha::ListCharactersResponse& Response)> InCallback)
			: RunnableRequest(InServerUrl, InCallback)
			, _InworldToken(InInworldToken)
			, _Workspace(InWorkspace)
		{}
		virtual ~RunnableListCharactersRequest() = default;

		virtual grpc::Status RunProcess() override;

	private:
		std::string _InworldToken;
		std::string _Workspace;
	};

	class INWORLD_EXPORT RunnableListApiKeysRequest : public RunnableRequest<InworldV1alpha::ApiKeys, InworldV1alpha::ListApiKeysResponse>
	{
	public:
		RunnableListApiKeysRequest(const std::string& InInworldToken, const std::string& InServerUrl, const std::string& InWorkspace, std::function<void(const grpc::Status& Status, const InworldV1alpha::ListApiKeysResponse& Response)> InCallback)
			: RunnableRequest(InServerUrl, InCallback)
			, _InworldToken(InInworldToken)
			, _Workspace(InWorkspace)
		{}
		virtual ~RunnableListApiKeysRequest() = default;

		virtual grpc::Status RunProcess() override;

	private:
		std::string _InworldToken;
		std::string _Workspace;
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

	private:

		AudioSessionDumper AudioDumper;
		SharedQueue<std::string>& AudioChuncks;
	};
#endif
}
