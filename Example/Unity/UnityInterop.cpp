#include "UnityInterop.h"


NDKUnity::CUnityWrapper* Unity_InitWrapper()
{
	if (g_pWrapper == nullptr)
		g_pWrapper = new NDKUnity::CUnityWrapper();
	return g_pWrapper;	
}

void Unity_SetLogger(Inworld::UnityLogCallback callback)
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->SetLoggerCallBack(callback);
	Inworld::LogWarning("Callback Initialized");
}

void Unity_Hello()
{
	const auto callBack = g_pWrapper->GetLoggerCallBack();
	if (callBack == nullptr || Inworld::g_UnityLoggerCallback == nullptr)
		return;
	Inworld::Log("Hello");
}

void Unity_GetAccessToken(const char* strServerURL, const char* strAPIKey, const char* strAPISecret)
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->SetServerURL(strServerURL);
	g_pWrapper->SetAPIKey(strAPIKey);
	g_pWrapper->SetAPISecret(strAPISecret);
	Inworld::SessionInfo info = g_pWrapper->GetAccessToken();
	Inworld::Log("Get Token completed: %s", info.Token);
}

void Unity_DestroyWrapper()
{
	delete g_pWrapper;
	g_pWrapper = nullptr;
}