/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Client.h"
#include "Service.h"
#include "Utils/Log.h"
#include "base64/Base64.h"

#include "grpc/impl/codegen/log.h"
#include "ai/inworld/common/status.pb.h"

#include <vector>
#include <unordered_map>
#include <functional>
#include <cstring>
#include <algorithm>

// prevent std::min/max errors on windows
#undef min
#undef max

constexpr int64_t gMaxTokenLifespan = 60 * 45; // 45 minutes

const std::string DefaultTargetUrl = "api-engine.inworld.ai:443";

static void GrpcLog(gpr_log_func_args* args)
{
	if (args->severity != GPR_LOG_SEVERITY_ERROR)
	{
		return;
	}

	// skip wrong errors(should be severity=info in grpc code)
	std::string Message(args->message);
	uint32_t Idx = Message.find("CompressMessage:");
	if (Idx == 0)
	{
		return;
	}
	Idx = Message.find("DecompressMessage:");
	if (Idx == 0)
	{
		return;
	}
	
	Inworld::LogError("GRPC %s %s::%d",
		args->message,
		args->file,
		args->line);
}

namespace Inworld
{

	static std::unique_ptr<Client> g_ClientPtr;

	void CreateClient()
	{
		g_ClientPtr = std::make_unique<Client>();
	}

	void DestroyClient()
	{
		g_ClientPtr.reset();
	}

	std::unique_ptr<Inworld::Client>& GetClient()
	{
		return g_ClientPtr;
	}

	class ClientService : public IClientService
	{
	public:
		virtual ~ClientService()
		{
			_ClientStream.reset();
			_SessionService.reset();
		}

		virtual std::unique_ptr<ServiceSession>& Session() override { return _SessionService; }
		virtual std::unique_ptr<ClientStream>& Stream() override { return _ClientStream; }

		virtual void OpenSession() override
		{
			if (!_SessionService)
			{
				Inworld::LogError("ClientService::OpenSession error, service invalid");
				return;
			}

			_ClientStream = _SessionService->OpenSession();
		}

	private:
		std::unique_ptr<Inworld::ClientStream> _ClientStream;
		std::unique_ptr<Inworld::ServiceSession> _SessionService;
	};

	ErrorDetails::ErrorDetails(const std::string& data)
	{
		google::rpc::Status status;
		ai::inworld::common::InworldStatus InworldStatus;
		if(status.ParseFromString(data) && status.details_size() > 0 && status.details(0).UnpackTo(&InworldStatus))
		{
			Error = static_cast<ErrorDetails::ErrorType>(InworldStatus.error_type());
			Reconnect = static_cast<ErrorDetails::ReconnectionType>(InworldStatus.reconnect_type());
			if (Reconnect == ErrorDetails::ReconnectionType::Timeout)
			{
				ReconnectTime = std::time(0) + std::max(InworldStatus.reconnect_time().seconds() - std::time(0), int64_t(0));
			}
			MaxRetries = InworldStatus.max_retries();
			if (Error == ErrorDetails::ErrorType::ResourceNotFound)
			{
				ResourceNotFoundPayload.Id = InworldStatus.resource_not_found().resource_id();
				ResourceNotFoundPayload.Type = static_cast<ErrorDetails::ResourceNotFoundDetails::ResourceType>(InworldStatus.resource_not_found().resource_type());
			}
		}
	}

}

const Inworld::SessionInfo& Inworld::Client::GetSessionInfo() const
{
	return _SessionInfo;
}

void Inworld::Client::SetOptions(const ClientOptions& options)
{
	_ClientOptions = options;

}

void Inworld::Client::Visit(const SessionControlResponse_LoadScene& Event)
{
	Inworld::Log("Load scene SUCCESS");
	for (const auto& Info : Event.GetAgentInfos())
	{
		Inworld::Log("Character registered: %s, Id: %s, GivenName: %s", Info.BrainName.c_str(), Info.AgentId.c_str(), Info.GivenName.c_str());
	}

	SetConnectionState(ConnectionState::Connected);
	StartClientStream();
}

void Inworld::Client::Visit(const SessionControlResponse_LoadCharacters& Event)
{
	Inworld::Log("Load characters SUCCESS");
	for (const auto& Info : Event.GetAgentInfos())
	{
		Inworld::Log("Character registered: %s, Id: %s, GivenName: %s", Info.BrainName.c_str(), Info.AgentId.c_str(), Info.GivenName.c_str());
	}
}

void Inworld::Client::SendPacket(std::shared_ptr<Inworld::Packet> Packet)
{
	if (GetConnectionState() != ConnectionState::Connected && GetConnectionState() != ConnectionState::Reconnecting)
	{
		Inworld::LogWarning("Packet skipped. Send packets only when connected. Connection state is '%d'", static_cast<int32_t>(GetConnectionState()));
		return;
	}

	PushPacket(Packet);
}

std::shared_ptr<Inworld::ControlEventConversationUpdate> Inworld::Client::UpdateConversation(
	const std::vector<std::string>& AgentIds, const std::string& ConversationId, bool bIncludePlayer)
{
	auto Packet = std::make_shared<Inworld::ControlEventConversationUpdate>(ConversationId, AgentIds, bIncludePlayer);
	SendPacket(Packet);
	return Packet;
}

void Inworld::Client::PushPacket(std::shared_ptr<Inworld::Packet> Packet)
{
	_OutgoingPackets.PushBack(Packet);

	TryToStartWriteTask();
}

std::shared_ptr<Inworld::TextEvent> Inworld::Client::SendTextMessage(const Inworld::Routing& Routing, const std::string& Text)
{
	auto Packet = std::make_shared<TextEvent>(Text, Routing);
	SendPacket(Packet);
	_LatencyTracker.HandlePacket(Packet);
	return Packet;
}

std::shared_ptr<Inworld::DataEvent> Inworld::Client::SendSoundMessage(const Inworld::Routing& Routing, const std::string& Data)
{
	auto Packet = std::make_shared<AudioDataEvent>(Data, Routing);
	SendPacket(Packet);
#ifdef INWORLD_AUDIO_DUMP
	if (bDumpAudio)
		_AudioChunksToDump.PushBack(Data);
#endif
	return Packet;
}

std::shared_ptr<Inworld::DataEvent> Inworld::Client::SendSoundMessageWithAEC(const Inworld::Routing& Routing, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData)
{
	std::vector<int16_t> FilteredData = _EchoFilter.FilterAudio(InputData, OutputData);

	std::string Data;
	Data.resize(FilteredData.size() * sizeof(int16_t));
	std::memcpy((void*)Data.data(), (void*)FilteredData.data(), Data.size());

	return SendSoundMessage(Routing, Data);
}

std::shared_ptr<Inworld::CustomEvent> Inworld::Client::SendCustomEvent(const Inworld::Routing& Routing, const std::string& Name, const std::unordered_map<std::string, std::string>& Params)
{
	auto Packet = std::make_shared<CustomEvent>(Name, Params, Routing);
	SendPacket(Packet);
	return Packet;
}

std::shared_ptr<Inworld::ControlEvent> Inworld::Client::StartAudioSession(const Inworld::Routing& Routing, const AudioSessionStartPayload& Payload)
{
	auto Packet = std::make_shared<Inworld::ControlEventAudioSessionStart>(Routing, static_cast<ai::inworld::packets::AudioSessionStartPayload_MicrophoneMode>(Payload.MicMode));
	SendPacket(Packet);
	return Packet;
}

std::shared_ptr<Inworld::ControlEvent> Inworld::Client::StopAudioSession(const Inworld::Routing& Routing)
{
	auto Packet = std::make_shared<Inworld::ControlEvent>(ai::inworld::packets::ControlEvent_Action_AUDIO_SESSION_END, std::string{}, Routing);
	SendPacket(Packet);
	return Packet;
}

std::shared_ptr<Inworld::ControlEvent> Inworld::Client::StartAudioSession(const std::string& AgentId, const AudioSessionStartPayload& Payload)
{
	return StartAudioSession(Inworld::Routing::Player2Agent(AgentId), Payload);
}

std::shared_ptr<Inworld::ControlEvent> Inworld::Client::StopAudioSession(const std::string& AgentId)
{
	return StopAudioSession(Inworld::Routing::Player2Agent(AgentId));
}

std::shared_ptr<Inworld::TextEvent> Inworld::Client::SendTextMessage(const std::string& AgentId, const std::string& Text)
{
	return SendTextMessage(Routing::Player2Agent(AgentId), Text);
}

std::shared_ptr<Inworld::TextEvent> Inworld::Client::SendTextMessageToConversation(const std::string& ConversationId, const std::string& Text)
{
	return SendTextMessage(Routing::Player2Conversation(ConversationId), Text);
}

std::shared_ptr<Inworld::DataEvent> Inworld::Client::SendSoundMessage(const std::string& AgentId, const std::string& Data)
{
	return SendSoundMessage(Routing::Player2Agent(AgentId), Data);
}

std::shared_ptr<Inworld::DataEvent> Inworld::Client::SendSoundMessageToConversation(const std::string& ConversationId, const std::string& Data)
{
	return SendSoundMessage(Routing::Player2Conversation(ConversationId), Data);
}

std::shared_ptr<Inworld::DataEvent> Inworld::Client::SendSoundMessageWithAEC(const std::string& AgentId, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData)
{
	return SendSoundMessageWithAEC(Routing::Player2Agent(AgentId), InputData, OutputData);
}

std::shared_ptr<Inworld::DataEvent> Inworld::Client::SendSoundMessageWithAECToConversation(
	const std::string& ConversationId, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData)
{
	return SendSoundMessageWithAEC(Routing::Player2Conversation(ConversationId), InputData, OutputData);
}

std::shared_ptr<Inworld::CustomEvent> Inworld::Client::SendCustomEvent(const std::string& AgentId, const std::string& Name, const std::unordered_map<std::string, std::string>& Params)
{
	return SendCustomEvent(Routing::Player2Agent(AgentId), Name, Params);
}

std::shared_ptr<Inworld::CustomEvent> Inworld::Client::SendCustomEventToConversation(const std::string& ConversationId,
	const std::string& Name, const std::unordered_map<std::string, std::string>& Params)
{
	return SendCustomEvent(Routing::Player2Conversation(ConversationId), Name, Params);
}

std::shared_ptr<Inworld::ActionEvent> Inworld::Client::SendNarrationEvent(const Inworld::Routing& Routing,
	const std::string& Content)
{
	auto Packet = std::make_shared<ActionEvent>(Content, Routing);
	SendPacket(Packet);
	return Packet;
}

std::shared_ptr<Inworld::ActionEvent> Inworld::Client::SendNarrationEvent(const std::string& AgentId, const std::string& Content)
{
	return SendNarrationEvent(Routing::Player2Agent(AgentId), Content);
}

std::shared_ptr<Inworld::CancelResponseEvent> Inworld::Client::CancelResponse(const Inworld::Routing& Routing,
	const std::string& InteractionId, const std::vector<std::string>& UtteranceIds)
{
	auto Packet = std::make_shared<Inworld::CancelResponseEvent>(InteractionId, UtteranceIds, Routing);
	SendPacket(Packet);
	return Packet;
}

std::shared_ptr<Inworld::ControlEvent> Inworld::Client::StartAudioSessionInConversation(
	const std::string& ConversationId, const AudioSessionStartPayload& Payload)
{
	return StartAudioSession(Routing::Player2Conversation(ConversationId), Payload);
}

std::shared_ptr<Inworld::ControlEvent> Inworld::Client::StopAudioSessionInConversation(
	const std::string& ConversationId)
{
	return StopAudioSession(Routing::Player2Conversation(ConversationId));
}

void Inworld::Client::LoadScene(const std::string& Scene)
{
	ControlSession<SessionControlEvent_LoadScene>({ Scene });
}

void Inworld::Client::LoadCharacters(const std::vector<std::string>& Names)
{
	ControlSession<SessionControlEvent_LoadCharacters>({ Names });
}

void Inworld::Client::UnloadCharacters(const std::vector<std::string>& Names)
{
	ControlSession<SessionControlEvent_UnloadCharacters>({ Names });
}

void Inworld::Client::LoadSavedState(const std::string& SavedState)
{
	if (!SavedState.empty())
	{
		ControlSession<SessionControlEvent_SessionSave>({ SavedState });
	}
}

void Inworld::Client::LoadCapabilities(const Capabilities& Capabilities)
{
	ControlSession<SessionControlEvent_Capabilities>(Capabilities);
}

void Inworld::Client::LoadUserConfiguration(const UserConfiguration& UserConfig)
{
	ControlSession<SessionControlEvent_UserConfiguration>(UserConfig);
}

std::shared_ptr<Inworld::CancelResponseEvent> Inworld::Client::CancelResponse(const std::string& AgentId, const std::string& InteractionId, const std::vector<std::string>& UtteranceIds)
{
	auto Packet = std::make_shared<Inworld::CancelResponseEvent>(
		InteractionId, 
		UtteranceIds, 
		Inworld::Routing::Player2Agent(AgentId));
	SendPacket(Packet);
	return Packet;
}

void Inworld::Client::SetAudioDumpEnabled(bool bEnabled, const std::string& FileName)
{
#ifdef INWORLD_AUDIO_DUMP
	bDumpAudio = bEnabled;
	_AudioDumpFileName = FileName;
	_AsyncAudioDumper.Stop();
	if (bDumpAudio)
	{
		_AsyncAudioDumper.Start("InworldAudioDumper", std::make_unique<RunnableAudioDumper>(_AudioChunksToDump, _AudioDumpFileName));
		Inworld::Log("ASYNC audio dump STARTING");
	}
#endif
}

void Inworld::Client::InitClientAsync(const SdkInfo& SdkInfo, std::function<void(ConnectionState)> ConnectionStateCallback, std::function<void(std::shared_ptr<Inworld::Packet>)> PacketCallback)
{
	gpr_set_log_function(GrpcLog);

	_Service = std::make_unique<ClientService>();

	_SdkInfo = SdkInfo;
	if (_SdkInfo.Type.empty())
	{
		_SdkInfo.Type = "ndk";
		if (!_SdkInfo.Subtype.empty())
		{
			Inworld::LogWarning("SdkInfo.Type is empty, but SdkInfo.Subtype is %s. Using empty subtype.", _SdkInfo.Subtype.c_str());
			_SdkInfo.Subtype = "";
		}
	}
	if (_SdkInfo.Version.empty())
	{
		_SdkInfo.Version = NDK_VERSION_STRING;
	}
	if (_SdkInfo.OS.empty())
	{
		Inworld::LogWarning("Please provide SdkInfo.OS, operating system or browser");
	}

	_OnConnectionStateChangedCallback = ConnectionStateCallback;
	_OnPacketCallback = PacketCallback;

	SetConnectionState(ConnectionState::Idle);
}

void Inworld::Client::GenerateToken(std::function<void()> GenerateTokenCallback)
{
	_OnGenerateTokenCallback = GenerateTokenCallback;

	_AsyncGenerateTokenTask.Start(
		"InworldGenerateTokenTask",
		std::make_unique<RunnableGenerateSessionToken>(
			_ClientOptions.ServerUrl,
			_ClientOptions.Resource,
			_ClientOptions.ApiKey,
			_ClientOptions.ApiSecret,
			[this](const grpc::Status& Status, const InworldEngine::AccessToken& Token) mutable
			{
				if (_SessionInfo.SessionId.empty())
				{
					_SessionInfo.SessionId = Token.session_id();
				}
				_SessionInfo.Token = Token.token();
				_SessionInfo.ExpirationTime = std::time(0) + std::max(std::min(Token.expiration_time().seconds() - std::time(0), gMaxTokenLifespan), int64_t(0));

				if (!Status.ok())
				{
					_ErrorMessage = Status.error_message().c_str();
					_ErrorCode = Status.error_code();
					_ErrorDetails = Status.error_details();
					StopClientStream();
					Inworld::LogError("Generate session token FALURE! %s, Code: %d", _ErrorMessage.c_str(), _ErrorCode);
					SetConnectionState(ConnectionState::Failed);
				}
				else
				{
					if (_OnGenerateTokenCallback)
					{
						_OnGenerateTokenCallback();
					}
				}
				_OnGenerateTokenCallback = nullptr;
			}
		)
	);
}

void Inworld::Client::StartClient(const ClientOptions& Options, const SessionInfo& Info)
{
	if (_ConnectionState != ConnectionState::Idle && _ConnectionState != ConnectionState::Failed)
	{
		return;
	}
	_ClientOptions = Options;
	if (!_ClientOptions.Base64.empty())
	{
		std::string Decoded;
		macaron::Base64::Decode(_ClientOptions.Base64, Decoded);
		const size_t Idx = Decoded.find(':');
		if (Idx != std::string::npos)
		{
			_ClientOptions.ApiKey = Decoded.substr(0, Idx);
			_ClientOptions.ApiSecret = Decoded.substr(Idx + 1, Decoded.size() - Idx + 1);
		}
		else
		{
			Inworld::LogError("StartClient: invalid base64 signature, ignored.");
		}
	}
	if (_ClientOptions.ProjectName.empty())
	{
		Inworld::LogWarning("StartClient: provide ClientOptions.ProjectName");
	}

	_SessionInfo = Info;

	_LatencyTracker.TrackAudioReplies(Options.Capabilities.Audio);

	SetConnectionState(ConnectionState::Connecting);

	if (!_SessionInfo.IsValid())
	{
		GenerateToken([this]()
		{
			StartSession();
		});
	}
	else
	{
		StartSession();
	}
}

void Inworld::Client::PauseClient()
{
	if (_ConnectionState != ConnectionState::Connected)
	{
		return;
	}

	StopClientStream();

	SetConnectionState(ConnectionState::Paused);
}

void Inworld::Client::ResumeClient()
{
	if (_ConnectionState != ConnectionState::Disconnected && _ConnectionState != ConnectionState::Paused)
	{
		return;
	}

	SetConnectionState(ConnectionState::Reconnecting);

	if (!_SessionInfo.IsValid())
	{
		GenerateToken([this]()
		{
			if (_Service->Session())
			{
				_Service->Session()->SetToken(_SessionInfo.Token);
				StartClientStream();
			}
		});
	}
	else
	{
		StartClientStream();
	}
}

void Inworld::Client::StopClient()
{
	if (_ConnectionState == ConnectionState::Idle)
	{
		return;
	}

	StopClientStream();
	if (_Service->Session())
	{
		_Service->Session()->Cancel();
	}
	_AsyncGenerateTokenTask.Stop();
	_AsyncGetSessionState.Stop();
#ifdef INWORLD_AUDIO_DUMP
	_AsyncAudioDumper.Stop();
#endif
	_ClientOptions = ClientOptions();
	_SessionInfo = SessionInfo();
	SetConnectionState(ConnectionState::Idle);
	Inworld::LogClearSessionId();
}

void Inworld::Client::DestroyClient()
{
	StopClient();
	_OnPacketCallback = nullptr;
	_OnGenerateTokenCallback = nullptr;
	_OnConnectionStateChangedCallback = nullptr;
	_LatencyTracker.ClearCallback();
	_Service.reset();
}

std::string GetSessionName(const std::string& SceneName, const std::string& SessionId)
{
	const size_t CharactersPos = SceneName.find("characters");
	const size_t ScenesPos = SceneName.find("scenes");
	const size_t Pos = CharactersPos != std::string::npos ? CharactersPos : ScenesPos;
	if (Pos == std::string::npos)
	{
		Inworld::LogError("Inworld::Client::GetSessionName: Couldn't form SessionName");
		return {};
	}
	return SceneName.substr(0, Pos) + "sessions/" + SessionId;
}

void Inworld::Client::SaveSessionStateAsync(std::function<void(std::string, bool)> Callback)
{
	const std::string SessionName = GetSessionName(_ClientOptions.SceneName, _SessionInfo.SessionId);
	if (SessionName.empty())
	{
		Inworld::LogError("Inworld::Client::SaveSessionState: SessionName is empty!");
		Callback({}, false);
		return;
	}

	_AsyncGetSessionState.Start(
		"InworldSaveSession",
		std::make_unique<RunnableGetSessionState>(
			_ClientOptions.ServerUrl,
			_SessionInfo.Token,
			SessionName,
			[this, Callback](const grpc::Status& Status, const InworldEngineV1::SessionState& State)
			{
				if (!Status.ok())
				{
					Inworld::LogError("Save session state FALURE! %s, Code: %d", Status.error_message().c_str(), (int32_t)Status.error_code());
					Callback({}, false);
					return;
				}
				Callback(State.state(), true);
			}
		));
}

void Inworld::Client::SendFeedbackAsync(std::string& InteractionId, const InteractionFeedback& Feedback, std::function<void(std::string, bool)> Callback)
{
	const std::string SessionName = GetSessionName(_ClientOptions.SceneName, _SessionInfo.SessionId);
	if (SessionName.empty())
	{
		Inworld::LogError("Inworld::Client::SendFeedback: SessionName is empty!");
		return;
	}

	_AsyncSendFeedback.Start(
		"InworldSendFeedback",
		std::make_unique<RunnableCreateInteractionFeedback>(
			_ClientOptions.ServerUrl,
			_SessionInfo.SessionId,
			_SessionInfo.Token,
			SessionName + "/interactions/" + InteractionId + "/groups/default",
			Feedback,
			[this, Callback](const grpc::Status& Status, const InworldEngineV1::InteractionFeedback& Feedback)
			{
				if (!Status.ok())
				{
					Inworld::LogError("Send Feedback FALURE! %s, Code: %d", Status.error_message().c_str(), (int32_t)Status.error_code());
					if(Callback) Callback({}, false);
					return;
				}
				Inworld::Log("Feedback %s sent!", Feedback.name().c_str());
				if(Callback) Callback(Feedback.name(), true);
			}
		));
}

void Inworld::Client::SetConnectionState(ConnectionState State)
{
	if (_ConnectionState == State)
	{
		return;
	}

	_ConnectionState = State;

	if (_ConnectionState == ConnectionState::Connected || _ConnectionState == ConnectionState::Idle)
	{
		_ErrorMessage = std::string();
		_ErrorCode = grpc::StatusCode::OK;
		_ErrorDetails = {};
	}

	if (_OnConnectionStateChangedCallback)
	{
		_OnConnectionStateChangedCallback(_ConnectionState);
	}
}

void Inworld::Client::StartSession()
{
	if (!_SessionInfo.IsValid())
	{
		return;
	}

	if (_ClientOptions.SceneName.empty())
	{
		Inworld::LogError("StartSession error, Provide ClientOptions.SceneName.");
		return;
	}

	Inworld::LogSetSessionId(_SessionInfo.SessionId);
	Inworld::Log("Session Id: %s", _SessionInfo.SessionId.c_str());

	_Service->Session() = std::make_unique<ServiceSession>(
		_SessionInfo.Token,
		_SessionInfo.SessionId,
		_ClientOptions.ServerUrl
		);
	StartClientStream();
	if (!_Service->Stream())
	{
		Inworld::LogError("StartSession error, _Service->Stream() is invalid.");
		return;
	}

	std::string SdkDesc = _SdkInfo.Type;
	SdkDesc += !_SdkInfo.Subtype.empty() ? ("/" + _SdkInfo.Subtype + ";") : ";";
	if (!_SdkInfo.Version.empty())
	{
		SdkDesc += _SdkInfo.Version + ";";
	}
	if (!_SdkInfo.OS.empty())
	{
		SdkDesc += _SdkInfo.OS + ";";
	}
	if (!_ClientOptions.ProjectName.empty())
	{
		SdkDesc += _ClientOptions.ProjectName;
	}

	// order matters
	LoadCapabilities(_ClientOptions.Capabilities);
	if (!_ClientOptions.GameSessionId.empty())
	{
		ControlSession<SessionControlEvent_SessionConfiguration>({ _ClientOptions.GameSessionId });
	}
	ControlSession<SessionControlEvent_ClientConfiguration>(
		{ 
			_SdkInfo.Type,
			_SdkInfo.Version,
			SdkDesc,
		});
	LoadUserConfiguration(_ClientOptions.UserConfig);
	LoadSavedState(_SessionInfo.SessionSavedState);
	LoadScene(_ClientOptions.SceneName);
}

void Inworld::Client::StartClientStream()
{
	const bool bHasPendingWriteTask = _AsyncWriteTask.IsValid() && !_AsyncWriteTask.IsDone();
	const bool bHasPendingReadTask = _AsyncReadTask.IsValid() && !_AsyncReadTask.IsDone();
	if (!bHasPendingWriteTask && !bHasPendingReadTask && _Service->Session())
	{
		_ErrorMessage = std::string();
		_ErrorCode = grpc::StatusCode::OK;
		_ErrorDetails = {};
		_Service->OpenSession();
		_bHasClientStreamFinished = false;
		TryToStartReadTask();
		TryToStartWriteTask();
	}
}

void Inworld::Client::StopClientStream()
{
	_bHasClientStreamFinished = true;
	if (_Service->Session())
	{
		_Service->Session()->Cancel();
	}
	_AsyncReadTask.Stop();
	_AsyncWriteTask.Stop();
	_Service->Stream().reset();
}

void Inworld::Client::TryToStartReadTask()
{
	if (!_Service->Stream())
	{
		return;
	}

	const bool bHasPendingReadTask = _AsyncReadTask.IsValid() && !_AsyncReadTask.IsDone();
	if (!bHasPendingReadTask)
	{
		_AsyncReadTask.Start(
			"InworldRead",
			std::make_unique<RunnableRead>(
				*_Service->Stream().get(), 
				_bHasClientStreamFinished, 
				_IncomingPackets,
				[this](const std::shared_ptr<Inworld::Packet> InPacket)
				{
					if (!_bPendingIncomingPacketFlush)
					{
						_bPendingIncomingPacketFlush = true;
						std::shared_ptr<Inworld::Packet> Packet;
						while (_IncomingPackets.PopFront(Packet))
						{
							if (Packet)
							{
								Packet->Accept(*this);
								_LatencyTracker.HandlePacket(Packet);
								if (_OnPacketCallback)
								{
									_OnPacketCallback(Packet);
								}
							}
						}
						_bPendingIncomingPacketFlush = false;
					}
					if (_ConnectionState != ConnectionState::Connected)
					{
						SetConnectionState(ConnectionState::Connected);
					}
				},
				[this](const grpc::Status& Status)
				{
					_ErrorMessage = Status.error_message();
					_ErrorCode = Status.error_code();
					_ErrorDetails = Status.error_details();
					StopClientStream();
					Inworld::LogError("Message READ failed: %s. Code: %d", _ErrorMessage.c_str(), _ErrorCode);
					SetConnectionState(ConnectionState::Disconnected);
				}
			)
		);
	}
}

void Inworld::Client::TryToStartWriteTask()
{
	if (!_Service->Stream())
	{
		return;
	}

	const bool bHasPendingWriteTask = _AsyncWriteTask.IsValid() && !_AsyncWriteTask.IsDone();
	if (!bHasPendingWriteTask)
	{
		const bool bHasOutgoingPackets = !_OutgoingPackets.IsEmpty();
		if (bHasOutgoingPackets)
		{
			_AsyncWriteTask.Start(
				"InworldWrite",
				std::make_unique<RunnableWrite>(
					*_Service->Stream().get(),
					_bHasClientStreamFinished,
					_OutgoingPackets,
					[this](const std::shared_ptr<Inworld::Packet> InPacket)
					{
						if (_ConnectionState != ConnectionState::Connected)
						{
							SetConnectionState(ConnectionState::Connected);
						}
					},
					[this](const grpc::Status& Status)
					{
						_ErrorMessage = Status.error_message();
						_ErrorCode = Status.error_code();
						_ErrorDetails = Status.error_details();
						StopClientStream();
						Inworld::LogError("Message WRITE failed: %s. Code: %d", _ErrorMessage.c_str(), _ErrorCode);
						SetConnectionState(ConnectionState::Disconnected);
					}
				)
			);
		}
	}
}

