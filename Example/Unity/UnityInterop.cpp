#include "UnityInterop.h"


NDKUnity::CUnityWrapper* Unity_InitWrapper()
{
	if (g_pWrapper == nullptr)
		g_pWrapper = new NDKUnity::CUnityWrapper();
	return g_pWrapper;	
}

void Unity_SetClientRequest(const char* strClientID, const char* strClientVersion)
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->SetClientRequest(strClientID, strClientVersion);
}

void Unity_SetUserRequest(const char* strPlayerName, const char* strPlayerID)
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->SetUserRequest(strPlayerName, strPlayerID);
}

void Unity_AddUserProfile(const char* strProfileID, const char* strProfileValue)
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->AddUserProfile(strProfileID, strProfileValue);
}

void Unity_SetCapabilities(NDKUnity::Capabilities capabilities)
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->SetCapabilities(capabilities.ToNDK());
}

void Unity_GetUserSettings()
{
	if (g_pWrapper == nullptr)
		return;
	auto userRequest = g_pWrapper->GetUserRequest();
}

void Unity_SetLogger(Inworld::UnityLogCallback callback)
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->SetLoggerCallBack(callback);
}

void Unity_Hello()
{
	const auto callBack = g_pWrapper->GetLoggerCallBack();
	if (callBack == nullptr || Inworld::g_UnityLoggerCallback == nullptr)
		return;
	Inworld::Log("Hello");
}

void Unity_GetAccessToken(const char* serverURl, const char* apiKey, const char* apiSecret, NDKUnity::OnTokenGenerated callback)
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->SetServerURL(serverURl);
	g_pWrapper->SetAPIKey(apiKey);
	g_pWrapper->SetAPISecret(apiSecret);
	g_pWrapper->GenerateToken(callback);	
}

void Unity_DestroyWrapper()
{
	if (g_pWrapper)
		g_pWrapper->DestroyClient();
	delete g_pWrapper;
	g_pWrapper = nullptr;
}


void Unity_GetCapabilities()
{
	auto cp = g_pWrapper->GetOptions().Capabilities;
}

char* Unity_GetSessionID()
{	
	std::string sessionId = g_pWrapper->GetSessionInfo().SessionId;
	char* sessionID = new char[sessionId.size() + 1];
	strcpy_s(sessionID, sessionId.size() + 1, sessionId.c_str());
	return sessionID;
}

void Unity_LoadScene(const char* strSceneName, NDKUnity::OnTokenGenerated callback)
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->LoadScene(strSceneName, callback);
}
