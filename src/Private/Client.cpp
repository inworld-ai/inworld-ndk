/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Client.h"
#include "Service.h"
#include "Utils/Utils.h"
#include "Utils/Log.h"
#include "base64/Base64.h"

#include "grpc/impl/codegen/log.h"

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
		ARG_CHAR(args->message),
		ARG_CHAR(args->file),
		args->line);
}

namespace Inworld
{
	class ClientServiceImpl : public ClientService
	{
	public:
		virtual ~ClientServiceImpl()
		{
			_ClientStream.reset();
			_SessionService.reset();
		}

		virtual std::unique_ptr<ServiceSession>& Session() { return _SessionService; }
		virtual std::unique_ptr<ClientStream>& Stream() { return _ClientStream; }

		virtual void OpenSession() override
		{
			if (!_SessionService)
			{
				Inworld::LogError("ClientServiceImpl::OpenSession error, service invalid");
				return;
			}

			_ClientStream = _SessionService->OpenSession();
		}

	private:
		std::unique_ptr<Inworld::ClientStream> _ClientStream;
		std::unique_ptr<Inworld::ServiceSession> _SessionService;
	};
}
 

const Inworld::SessionInfo& Inworld::ClientBase::GetSessionInfo() const
{
	return _SessionInfo;
}

void Inworld::ClientBase::SetOptions(const ClientOptions& options)
{
	_ClientOptions = options;

}

void Inworld::ClientBase::Visit(const SessionControlResponse_LoadScene& Event)
{
	Inworld::Log("Load scene SUCCESS");
	for (const auto& Info : Event.GetAgentInfos())
	{
		Inworld::Log("Character registered: %s, Id: %s, GivenName: %s", ARG_STR(Info.BrainName), ARG_STR(Info.AgentId), ARG_STR(Info.GivenName));
	}

	if (_OnLoadSceneCallback)
	{
		_OnLoadSceneCallback(Event.GetAgentInfos());
		_OnLoadSceneCallback = nullptr;
	}

	SetConnectionState(ConnectionState::Connected);
	StartClientStream();
}

void Inworld::ClientBase::Visit(const SessionControlResponse_LoadCharacters& Event)
{
	Inworld::Log("Load characters SUCCESS");
	for (const auto& Info : Event.GetAgentInfos())
	{
		Inworld::Log("Character registered: %s, Id: %s, GivenName: %s", ARG_STR(Info.BrainName), ARG_STR(Info.AgentId), ARG_STR(Info.GivenName));
	}

	if (_OnLoadCharactersCallback)
	{
		_OnLoadCharactersCallback(Event.GetAgentInfos());
		_OnLoadCharactersCallback = nullptr;
	}
}

void Inworld::ClientBase::SendPacket(std::shared_ptr<Inworld::Packet> Packet)
{
	if (GetConnectionState() != ConnectionState::Connected)
	{
		Inworld::LogWarning("Packet skipped. Send packets only when connected. Connection state is '%d'", static_cast<int32_t>(GetConnectionState()));
		return;
	}

	PushPacket(Packet);
}

void Inworld::ClientBase::PushPacket(std::shared_ptr<Inworld::Packet> Packet)
{
	_OutgoingPackets.PushBack(Packet);

	TryToStartWriteTask();
}

std::shared_ptr<Inworld::TextEvent> Inworld::ClientBase::SendTextMessage(const Inworld::Routing& Routing, const std::string& Text)
{
	auto Packet = std::make_shared<TextEvent>(Text, Routing);
	SendPacket(Packet);
	_LatencyTracker.HandlePacket(Packet);
	return Packet;
}

std::shared_ptr<Inworld::DataEvent> Inworld::ClientBase::SendSoundMessage(const Inworld::Routing& Routing, const std::string& Data)
{
	auto Packet = std::make_shared<AudioDataEvent>(Data, Routing);
	SendPacket(Packet);
#ifdef INWORLD_AUDIO_DUMP
	if (bDumpAudio)
		_AudioChunksToDump.PushBack(Data);
#endif
	return Packet;
}

std::shared_ptr<Inworld::DataEvent> Inworld::ClientBase::SendSoundMessageWithAEC(const Inworld::Routing& Routing, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData)
{
	std::vector<int16_t> FilteredData = _EchoFilter.FilterAudio(InputData, OutputData);

	std::string Data;
	Data.resize(FilteredData.size() * sizeof(int16_t));
	std::memcpy((void*)Data.data(), (void*)FilteredData.data(), Data.size());

	return SendSoundMessage(Routing, Data);
}

std::shared_ptr<Inworld::CustomEvent> Inworld::ClientBase::SendCustomEvent(const Inworld::Routing& Routing, const std::string& Name, const std::unordered_map<std::string, std::string>& Params)
{
	auto Packet = std::make_shared<CustomEvent>(Name, Params, Routing);
	SendPacket(Packet);
	return Packet;
}

void Inworld::ClientBase::StartAudioSession(const Inworld::Routing& Routing)
{
	auto Packet = std::make_shared<Inworld::ControlEvent>(ai::inworld::packets::ControlEvent_Action_AUDIO_SESSION_START, Routing);
	SendPacket(Packet);
}

void Inworld::ClientBase::StopAudioSession(const Inworld::Routing& Routing)
{
	auto Packet = std::make_shared<Inworld::ControlEvent>(ai::inworld::packets::ControlEvent_Action_AUDIO_SESSION_END, Routing);
	SendPacket(Packet);
}

std::shared_ptr<Inworld::TextEvent> Inworld::ClientBase::SendTextMessage(const std::string& AgentId, const std::string& Text)
{
	return SendTextMessage(Routing::Player2Agent(AgentId), Text);
}

std::shared_ptr<Inworld::TextEvent> Inworld::ClientBase::SendTextMessage(const std::vector<std::string>& AgentIds, const std::string& Text)
{
	if (AgentIds.size() == 1)
	{
		return SendTextMessage(AgentIds[0], Text);
	}
	return SendTextMessage(Routing::Player2Agents(AgentIds), Text);
}

std::shared_ptr<Inworld::DataEvent> Inworld::ClientBase::SendSoundMessage(const std::string& AgentId, const std::string& Data)
{
	return SendSoundMessage(Routing::Player2Agent(AgentId), Data);
}

std::shared_ptr<Inworld::DataEvent> Inworld::ClientBase::SendSoundMessage(const std::vector<std::string>& AgentIds, const std::string& Data)
{
	if (AgentIds.size() == 1)
	{
		return SendSoundMessage(AgentIds[0], Data);
	}
	return SendSoundMessage(Routing::Player2Agents(AgentIds), Data);
}

std::shared_ptr<Inworld::DataEvent> Inworld::ClientBase::SendSoundMessageWithAEC(const std::string& AgentId, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData)
{
	return SendSoundMessageWithAEC(Routing::Player2Agent(AgentId), InputData, OutputData);
}

std::shared_ptr<Inworld::DataEvent> Inworld::ClientBase::SendSoundMessageWithAEC(const std::vector<std::string>& AgentIds, const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData)
{
	if (AgentIds.size() == 1)
	{
		return SendSoundMessageWithAEC(AgentIds[0], InputData, OutputData);
	}
	return SendSoundMessageWithAEC(Routing::Player2Agents(AgentIds), InputData, OutputData);
}

std::shared_ptr<Inworld::CustomEvent> Inworld::ClientBase::SendCustomEvent(std::string AgentId, const std::string& Name, const std::unordered_map<std::string, std::string>& Params)
{
	return SendCustomEvent(Routing::Player2Agent(AgentId), Name, Params);
}

std::shared_ptr<Inworld::CustomEvent> Inworld::ClientBase::SendCustomEvent(const std::vector<std::string>& AgentIds, const std::string& Name, const std::unordered_map<std::string, std::string>& Params)
{
	if (AgentIds.size() == 1)
	{
		return SendCustomEvent(AgentIds[0], Name, Params);
	}
	return SendCustomEvent(Routing::Player2Agents(AgentIds), Name, Params);
}

std::shared_ptr<Inworld::ActionEvent> Inworld::ClientBase::SendNarrationEvent(std::string AgentId, const std::string& Content)
{
	auto Packet = std::make_shared<ActionEvent>(Content, Routing::Player2Agent(AgentId));
	SendPacket(Packet);
	return Packet;
}

void Inworld::ClientBase::LoadScene(const std::string& Scene, CharactersLoadedCb OnLoadSceneCallback)
{
	if (_OnLoadSceneCallback)
	{
		Inworld::LogError("Skip LoadScene. Another request is in progress.");
		return;
	}
	_OnLoadSceneCallback = OnLoadSceneCallback;
	ControlSession<SessionControlEvent_LoadScene>({ Scene });
}

void Inworld::ClientBase::LoadCharacters(const std::vector<std::string>& Names, CharactersLoadedCb OnLoadCharactersCallback)
{
	if (_OnLoadCharactersCallback)
	{
		Inworld::LogError("Skip LoadCharacters. Another request is in progress.");
		return;
	}
	_OnLoadCharactersCallback = OnLoadCharactersCallback;
	ControlSession<SessionControlEvent_LoadCharacters>({ Names });
}

void Inworld::ClientBase::UnloadCharacters(const std::vector<std::string>& Names)
{
	ControlSession<SessionControlEvent_UnloadCharacters>({ Names });
}

void Inworld::ClientBase::LoadSavedState(const std::string& SavedState)
{
	if (!SavedState.empty())
	{
		ControlSession<SessionControlEvent_SessionSave>({ SavedState });
	}
}

void Inworld::ClientBase::CancelResponse(const std::string& AgentId, const std::string& InteractionId, const std::vector<std::string>& UtteranceIds)
{
	auto Packet = std::make_shared<Inworld::CancelResponseEvent>(
		InteractionId, 
		UtteranceIds, 
		Inworld::Routing(Inworld::Actor(), 
		Inworld::Actor(ai::inworld::packets::Actor_Type_AGENT, AgentId)));
	SendPacket(Packet);
}

void Inworld::ClientBase::SetAudioDumpEnabled(bool bEnabled, const std::string& FileName)
{
#ifdef INWORLD_AUDIO_DUMP
	bDumpAudio = bEnabled;
	_AudioDumpFileName = FileName;
	_AsyncAudioDumper->Stop();
	if (bDumpAudio)
	{
		_AsyncAudioDumper->Start("InworldAudioDumper", std::make_unique<RunnableAudioDumper>(_AudioChunksToDump, _AudioDumpFileName));
		Inworld::Log("ASYNC audio dump STARTING");
	}
#endif
}

void Inworld::ClientBase::StartAudioSession(const std::string& AgentId)
{
	StartAudioSession(Inworld::Routing::Player2Agent(AgentId));
}

void Inworld::ClientBase::StartAudioSession(const std::vector<std::string>& AgentIds)
{
	StartAudioSession(Inworld::Routing::Player2Agents(AgentIds));
}

void Inworld::ClientBase::StopAudioSession(const std::string& AgentId)
{
	StopAudioSession(Inworld::Routing::Player2Agent(AgentId));
}

void Inworld::ClientBase::StopAudioSession(const std::vector<std::string>& AgentIds)
{
	StopAudioSession(Inworld::Routing::Player2Agents(AgentIds));
}

void Inworld::ClientBase::InitClient(const SdkInfo& SdkInfo, std::function<void(ConnectionState)> ConnectionStateCallback, std::function<void(std::shared_ptr<Inworld::Packet>)> PacketCallback)
{
	gpr_set_log_function(GrpcLog);

	_Service = std::make_unique<ClientServiceImpl>();

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

void Inworld::ClientBase::GenerateToken(std::function<void()> GenerateTokenCallback)
{
	_OnGenerateTokenCallback = GenerateTokenCallback;

	_AsyncGenerateTokenTask->Start(
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

				AddTaskToMainThread([this, Status]()
				{
					if (!Status.ok())
					{
						_ErrorMessage = std::string(Status.error_message().c_str());
						_ErrorCode = Status.error_code();
						Inworld::LogError("Generate session token FALURE! %s, Code: %d", ARG_STR(_ErrorMessage), _ErrorCode);
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
				});
			}
		)
	);
}

void Inworld::ClientBase::StartClient(const ClientOptions& Options, const SessionInfo& Info, CharactersLoadedCb LoadSceneCallback)
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
			Inworld::LogError("Invalid base64 signature, ignored.");
		}
	}
	if (_ClientOptions.ProjectName.empty())
	{
		Inworld::LogWarning("Please provide ClientOptions.ProjectName");
	}

	_SessionInfo = Info;

	_LatencyTracker.TrackAudioReplies(Options.Capabilities.Audio);

	SetConnectionState(ConnectionState::Connecting);

	if (!_SessionInfo.IsValid())
	{
		GenerateToken([this, LoadSceneCallback]()
		{
			StartSession(LoadSceneCallback);
		});
	}
	else
	{
		StartSession(LoadSceneCallback);
	}
}

void Inworld::ClientBase::PauseClient()
{
	if (_ConnectionState != ConnectionState::Connected)
	{
		return;
	}

	StopClientStream();

	SetConnectionState(ConnectionState::Paused);
}

void Inworld::ClientBase::ResumeClient()
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

void Inworld::ClientBase::StopClient()
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
	_AsyncGenerateTokenTask->Stop();
	_AsyncGetSessionState->Stop();
#ifdef INWORLD_AUDIO_DUMP
	_AsyncAudioDumper->Stop();
#endif
	_ClientOptions = ClientOptions();
	_SessionInfo = SessionInfo();
	SetConnectionState(ConnectionState::Idle);
	Inworld::LogClearSessionId();
}

void Inworld::ClientBase::DestroyClient()
{
	StopClient();
	_OnPacketCallback = nullptr;
	_OnLoadSceneCallback = nullptr;
	_OnGenerateTokenCallback = nullptr;
	_OnConnectionStateChangedCallback = nullptr;
	_LatencyTracker.ClearCallback();
	_Service.reset();
}

void Inworld::ClientBase::SaveSessionState(std::function<void(std::string, bool)> Callback)
{
	const size_t CharactersPos = _ClientOptions.SceneName.find("characters");
	const size_t ScenesPos = _ClientOptions.SceneName.find("scenes");
	const size_t Pos = CharactersPos != std::string::npos ? CharactersPos : ScenesPos;
	if (Pos == std::string::npos)
	{
		Inworld::LogError("Inworld::ClientBase::SaveSessionState: Couldn't form SessionName");
		Callback({}, false);
		return;
	}

	const std::string SessionName = _ClientOptions.SceneName.substr(0, Pos) + "sessions/" + _SessionInfo.SessionId;
	_AsyncGetSessionState->Start(
		"InworldSaveSession",
		std::make_unique<RunnableGetSessionState>(
			_ClientOptions.ServerUrl,
			_SessionInfo.Token,
			SessionName,
			[this, Callback](const grpc::Status& Status, const InworldEngineV1::SessionState& State)
			{
				AddTaskToMainThread([this, Status, State, Callback]() {
					if (!Status.ok())
					{
						Inworld::LogError("Save session state FALURE! %s, Code: %d", ARG_STR(Status.error_message()), (int32_t)Status.error_code());
						Callback({}, false);
						return;
					}
					Callback(State.state(), true);
				});
			}
		));
}

bool Inworld::ClientBase::GetConnectionError(std::string& OutErrorMessage, int32_t& OutErrorCode) const
{
	OutErrorMessage = _ErrorMessage;
	OutErrorCode = _ErrorCode;
	return _ErrorCode != grpc::StatusCode::OK;
}

void Inworld::ClientBase::SetConnectionState(ConnectionState State)
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
	}

	if (_OnConnectionStateChangedCallback)
	{
		_OnConnectionStateChangedCallback(_ConnectionState);
	}
}

void Inworld::ClientBase::StartSession(CharactersLoadedCb LoadSceneCallback)
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
	Inworld::Log("Session Id: %s", ARG_STR(_SessionInfo.SessionId));

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
	ControlSession<SessionControlEvent_Capabilities>(_ClientOptions.Capabilities);
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
	ControlSession<SessionControlEvent_UserConfiguration>(_ClientOptions.UserSettings);
	LoadSavedState(_SessionInfo.SessionSavedState);
	LoadScene(_ClientOptions.SceneName, LoadSceneCallback);
}

void Inworld::ClientBase::StartClientStream()
{
	const bool bHasPendingWriteTask = _AsyncWriteTask->IsValid() && !_AsyncWriteTask->IsDone();
	const bool bHasPendingReadTask = _AsyncReadTask->IsValid() && !_AsyncReadTask->IsDone();
	if (!bHasPendingWriteTask && !bHasPendingReadTask && _Service->Session())
	{
		_ErrorMessage = std::string();
		_ErrorCode = grpc::StatusCode::OK;
		_Service->OpenSession();
		_bHasClientStreamFinished = false;
		TryToStartReadTask();
		TryToStartWriteTask();
	}
}

void Inworld::ClientBase::StopClientStream()
{
	_bHasClientStreamFinished = true;
	if (_Service->Session())
	{
		_Service->Session()->Cancel();
	}
	_AsyncReadTask->Stop();
	_AsyncWriteTask->Stop();
	_Service->Stream().reset();
}

void Inworld::ClientBase::TryToStartReadTask()
{
	if (!_Service->Stream())
	{
		return;
	}

	const bool bHasPendingReadTask = _AsyncReadTask->IsValid() && !_AsyncReadTask->IsDone();
	if (!bHasPendingReadTask)
	{
		_AsyncReadTask->Start(
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
						AddTaskToMainThread(
							[this]()
							{
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
							});
					}
					if (_ConnectionState != ConnectionState::Connected)
					{
						AddTaskToMainThread(
							[this]()
							{
								SetConnectionState(ConnectionState::Connected);
							});
					}
				},
				[this](const grpc::Status& Status)
				{
					_ErrorMessage = std::string(Status.error_message().c_str());
					_ErrorCode = Status.error_code();
					Inworld::LogError("Message READ failed: %s. Code: %d", ARG_STR(_ErrorMessage), _ErrorCode);
					AddTaskToMainThread(
						[this]()
						{
							SetConnectionState(ConnectionState::Disconnected);
						});
				}
			)
		);
	}
}

void Inworld::ClientBase::TryToStartWriteTask()
{
	if (!_Service->Stream())
	{
		return;
	}

	const bool bHasPendingWriteTask = _AsyncWriteTask->IsValid() && !_AsyncWriteTask->IsDone();
	if (!bHasPendingWriteTask)
	{
		const bool bHasOutgoingPackets = !_OutgoingPackets.IsEmpty();
		if (bHasOutgoingPackets)
		{
			_AsyncWriteTask->Start(
				"InworldWrite",
				std::make_unique<RunnableWrite>(
					*_Service->Stream().get(),
					_bHasClientStreamFinished,
					_OutgoingPackets,
					[this](const std::shared_ptr<Inworld::Packet> InPacket)
					{
						if (_ConnectionState != ConnectionState::Connected)
						{
							AddTaskToMainThread([this]() {
								SetConnectionState(ConnectionState::Connected);
							});
						}
					},
					[this](const grpc::Status& Status)
					{
						_ErrorMessage = std::string(Status.error_message().c_str());
						_ErrorCode = Status.error_code();
						Inworld::LogError("Message WRITE failed: %s. Code: %d", ARG_STR(_ErrorMessage), _ErrorCode);
						AddTaskToMainThread([this]() {
							SetConnectionState(ConnectionState::Disconnected);
						});
					}
				)
			);
		}
	}
}

void Inworld::Client::Update()
{
	ExecutePendingTasks();
}

void Inworld::Client::AddTaskToMainThread(std::function<void()> Task)
{
	_MainThreadTasks.PushBack(Task);
}

void Inworld::Client::ExecutePendingTasks()
{
	std::function<void()> Task;
	while (_MainThreadTasks.PopFront(Task))
	{
		Task();
	}
}

