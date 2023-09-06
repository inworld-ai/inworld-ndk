#include "UnityWrapper.h"



#pragma region Logger

void NDKUnity::CUnityWrapper::SetLoggerCallBack(const Inworld::UnityLogCallback& callback)
{
	m_LogCallBack = callback;
	if (callback != nullptr)
		Inworld::LogSetUnityLogCallback(callback);
	Inworld::Log("Unity NDK Logger Initialized");
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

void NDKUnity::CUnityWrapper::SetCapabilities(Inworld::CapabilitySet capabilities)
{
	_ClientOptions.Capabilities = capabilities;
	Inworld::Log("Set Audio = {0}", capabilities.Audio ? "true" : "false");
	Inworld::Log("Set Emotions = {0}", capabilities.Emotions ? "true" : "false");
}

void NDKUnity::CUnityWrapper::AddTaskToMainThread(std::function<void()> Task)
{
	Task();
}
#pragma endregion Logger


#pragma region Client Options

#pragma endregion Client Options