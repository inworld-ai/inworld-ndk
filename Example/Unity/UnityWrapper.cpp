#include "UnityWrapper.h"

#pragma region Logger

void NDKUnity::CUnityWrapper::SetLoggerCallBack(const Inworld::UnityLogCallback& callback)
{
	m_LogCallBack = callback;
	if (callback != nullptr)
		Inworld::LogSetUnityLogCallback(callback);
	Inworld::Log("Unity NDK Logger Initialized");
}

void NDKUnity::CUnityWrapper::AddTaskToMainThread(std::function<void()> Task)
{
	Task();
}
#pragma endregion Logger


#pragma region Client Options

#pragma endregion Client Options