#include "UnityWrapper.h"

#include "Data/UnityNDKInteropData.h"


#pragma region Logger

void NDKUnity::CUnityWrapper::SetLoggerCallBack(const Inworld::UnityLogCallback& callback)
{
	m_LogCallBack = callback;
	if (callback != nullptr)
		Inworld::LogSetUnityLogCallback(callback);
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
	ClientBase::LoadScene(strSceneName, [this, callback](const std::vector<Inworld::AgentInfo>& agentInfos)
	{
		m_AgentInfos = agentInfos;
		if (callback)
			callback();
	});	
}

void NDKUnity::CUnityWrapper::StartSession()
{
	StartReaderWriter();
}

void NDKUnity::CUnityWrapper::SetCapabilities(Inworld::CapabilitySet capabilities)
{
	_ClientOptions.Capabilities = capabilities;
}


void NDKUnity::CUnityWrapper::AddTaskToMainThread(std::function<void()> Task)
{
	Task();
}
#pragma endregion Logger


#pragma region Client Options

#pragma endregion Client Options