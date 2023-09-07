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

void Unity_SetTextCallback(NDKUnity::TextCallBack callBack)
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->SetTextCallBack(callBack);
}


void Unity_Hello()
{
	if (g_pWrapper == nullptr)
		return;
	if (g_pWrapper->GetAgentInfo().size() == 0)
		return;
	g_pWrapper->SendTextMessage(g_pWrapper->GetAgentInfo()[0].AgentId, "Hello");
}

void Unity_GetAccessToken(const char* serverURL, const char* apiKey, const char* apiSecret, NDKUnity::UnityCallback callback)
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->SetServerURL(serverURL);
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

NDKUnity::SessionInfo Unity_GetSessionInfo()
{	
	if (g_pWrapper == nullptr)
		return {};
	return NDKUnity::SessionInfo(g_pWrapper->GetSessionInfo());
}

void Unity_LoadScene(const char* strSceneName, NDKUnity::UnityCallback callback)
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->LoadScene(strSceneName, callback);
}


int Unity_GetAgentCount()
{
	if (g_pWrapper == nullptr)
		return -1;
	return static_cast<int>(g_pWrapper->GetAgentInfo().size());
}

NDKUnity::AgentInfo Unity_GetAgentInfo(int nIndex)
{
	if (g_pWrapper == nullptr || nIndex < 0 || nIndex > g_pWrapper->GetAgentInfo().size())
		return {};
	return NDKUnity::AgentInfo(g_pWrapper->GetAgentInfo()[nIndex]);
}

void Unity_StartSession()
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->StartSession();
	Inworld::Log("Session Started");
}

