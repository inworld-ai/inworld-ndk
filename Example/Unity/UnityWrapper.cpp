#include "UnityWrapper.h"

#pragma region Logger

void NDKUnity::CUnityWrapper::SetLoggerCallBack(const Inworld::UnityLogCallback& callback)
{
	m_LogCallBack = callback;
	if (callback != nullptr)
		Inworld::LogSetUnityLogCallback(callback);
	Inworld::Log("Unity NDK Logger Initialized");
}

Inworld::SessionInfo NDKUnity::CUnityWrapper::GetAccessToken()
{
	m_Client.SetOptions(m_Options);
	m_Client.GenerateToken(nullptr);	
	return m_Client.GetSessionInfo();
}



#pragma endregion Logger


#pragma region Client Options

#pragma endregion Client Options