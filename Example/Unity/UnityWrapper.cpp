#include "UnityWrapper.h"


void NDKUnity::Wrapper::SetLoggerCallBack(const Inworld::UnityLogCallback& callback)
{
	m_LogCallBack = callback;
	if (callback != nullptr)
		Inworld::LogSetUnityLogCallback(callback);
	Inworld::Log("Unity NDK Logger Initialized");
}