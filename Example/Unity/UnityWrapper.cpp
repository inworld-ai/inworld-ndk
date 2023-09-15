/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "UnityWrapper.h"

#include "Data/UnityAgentInfo.h"
#include "Data/UnityNDKInteropData.h"


#pragma region Logger

void NDKUnity::CUnityWrapper::SetLoggerCallBack(const Inworld::LoggerCallBack& callback)
{
	m_LogCallBack = callback;
	if (callback != nullptr)
		Inworld::LogSetLoggerCallback(callback);
	Inworld::Log("Unity NDK Logger Initialized");
}

void NDKUnity::CUnityWrapper::SetPacketCallBack(
	const TextCallBack& textCallBack,
	const AudioCallBack& audioCallBack,
	const ControlCallBack& controlCallBack,
	const EmotionCallBack& emotionCallBack,
	const CancelResponseCallBack& cancelResponseCallBack,
	const CustomCallBack& customCallBack,
	const PhonemeCallBack& phonemeCallBack,
	const TriggerParamCallBack& triggerParamCallBack)
{
	m_PacketHandler.SetPacketCallback(
		textCallBack,
		audioCallBack,
		controlCallBack,
		emotionCallBack,
		cancelResponseCallBack,
		customCallBack,
		phonemeCallBack,
		triggerParamCallBack);
}


void NDKUnity::CUnityWrapper::SetClientRequest(std::string strClientID, std::string strClientVersion)
{
	_ClientId = strClientID;
	_ClientVer = strClientVersion;	
}

void NDKUnity::CUnityWrapper::SetUserRequest(const std::string& strPlayerName, const std::string& strUserID)
{
	_ClientOptions.PlayerName = strPlayerName;
	_ClientOptions.UserId = strUserID;
}

void NDKUnity::CUnityWrapper::AddUserProfile(const std::string& strProfileID, const std::string& strProfileVal)
{
	_ClientOptions.UserSettings.Profile.Fields.push_back({strProfileID, strProfileVal});
}

void NDKUnity::CUnityWrapper::LoadScene(const std::string& strSceneName, UnityCallback callback)
{
	_ClientOptions.SceneName = strSceneName;
	if (!_SessionInfo.IsValid())
	{
		return;
	}
	Inworld::LogSetSessionId(_SessionInfo.SessionId);

	_AsyncLoadSceneTask->Start(
		"InworldLoadScene",
		std::make_unique<Inworld::RunnableLoadScene>(
			_SessionInfo.Token,
			_SessionInfo.SessionId,
			_ClientOptions.ServerUrl,
			_ClientOptions.SceneName,
			_ClientOptions.PlayerName,
			_ClientOptions.UserId,
			_ClientOptions.UserSettings,
			_ClientId,
			_ClientVer,
			_SessionInfo.SessionSavedState,
			_ClientOptions.Capabilities,
			[this, callback](const grpc::Status& Status, const InworldEngine::LoadSceneResponse& Response)
			{
				AddTaskToMainThread([this, Status, Response, callback]
				{
					if (!Status.ok())
					{
						Inworld::LogError("Load scene FAILURE! %s, Code: %d", ARG_STR(std::string(Status.error_message())), Status.error_code());
						return;
					}
					Inworld::Log("Load scene SUCCESS. Session Id: %s", ARG_STR(_SessionInfo.SessionId));
					m_AgentInfos.clear();
					
					for (int32_t i = 0; i < Response.agents_size(); i++)
					{
						UnityAgentInfo Info;
						Info.BrainName = Response.agents(i).brain_name();
						Info.AgentId = Response.agents(i).agent_id();
						Info.GivenName = Response.agents(i).given_name();
						Info.AvatarImg = Response.agents(i).character_assets().avatar_img();
						Info.AvatarImgOriginal = Response.agents(i).character_assets().avatar_img_original();
						Info.RpmModelUri = Response.agents(i).character_assets().rpm_model_uri();
						Info.RpmImageUriPortrait = Response.agents(i).character_assets().rpm_image_uri_portrait();
						Info.RpmImageUriPosture = Response.agents(i).character_assets().rpm_image_uri_posture();
						m_AgentInfos.push_back(Info);
					}
					if (callback)
						callback();
				});				
			}
		)
	);
}
	


void NDKUnity::CUnityWrapper::StartSession()
{
	StartReaderWriter();
}

void NDKUnity::CUnityWrapper::EndSession()
{
	StopReaderWriter();
}

void NDKUnity::CUnityWrapper::SetCapabilities(Inworld::CapabilitySet capabilities)
{
	_ClientOptions.Capabilities = capabilities;
}

void NDKUnity::CUnityWrapper::StopClient()
{
	// YAN: Anything other than Idle to get the full clean process.
	SetConnectionState(ConnectionState::Disconnected);
	Client::StopClient();
}

void NDKUnity::CUnityWrapper::AddTaskToMainThread(std::function<void()> Task)
{
	Task();
}
#pragma endregion Logger


#pragma region Client Options

#pragma endregion Client Options