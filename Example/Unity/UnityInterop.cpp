/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "UnityInterop.h"
#include "Data/UnityNDKInteropData.h"

void* NDK_InitWrapper()
{
	if (g_pNDKWrapper == nullptr)
		g_pNDKWrapper = new NDKApp::NDKWrapper();
	return new NDKApp::NDKWrapper();
}
void NDK_DestroyWrapper()
{
	if (g_pNDKWrapper)
		delete g_pNDKWrapper;
	g_pNDKWrapper = nullptr;
}
void NDK_SetLogger(Inworld::LogCallback info, Inworld::LogCallback warning, Inworld::LogCallback error)
{
	if (g_pNDKWrapper == nullptr)
		return;
	g_pNDKWrapper->SetLoggerCallBack(info, warning, error);
}
INWORLD_EXPORT void NDK_Init()
{
	if (g_pNDKWrapper)
		g_pNDKWrapper->Init();
}
INWORLD_EXPORT void NDK_Hello()
{
	 g_pNDKWrapper->SendText("Hello");
}
//void Unity_Hello()
//{
//	if (g_pWrapper == nullptr)
//		return;
//	if (g_pWrapper->GetAgentInfo().empty())
//		return;
//	g_pWrapper->SendTextMessage(g_pWrapper->GetAgentInfo()[0].agentId, "Hello");
//}
//
//void Unity_SetClientRequest(const char* strClientID, const char* strClientVersion, const char* strClientDescription)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->SetClientRequest(strClientID, strClientVersion, strClientDescription);
//}
//
//void Unity_SetPublicWorkspace(const char* strPublicWorkspace)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->SetPublicWorkspace(strPublicWorkspace);
//}
//
//void Unity_SetUserRequest(const char* strPlayerName, const char* strPlayerID)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->SetUserRequest(strPlayerName, strPlayerID);
//}
//
//void Unity_AddUserProfile(const char* strProfileID, const char* strProfileValue)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->AddUserProfile(strProfileID, strProfileValue);
//}
//
//void Unity_SetCapabilities(const NDKUnity::Capabilities& capabilities)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->SetCapabilities(capabilities.ToNDK());
//}
//
//
//
//
//void Unity_SaveSessionState(NDKUnity::UnityCallback callback)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	std::function bridgeCallback = [callback](std::string result, bool success) 
//	{
//		if (success)
//		{
//			g_pWrapper->SetSessionState(result);
//			callback();
//		}		
//	};
//
//	g_pWrapper->SaveSessionState(bridgeCallback);
//}
//void Unity_GetAccessToken(const char* serverURL, const char* apiKey, const char* apiSecret, NDKUnity::UnityCallback callback)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->SetServerURL(serverURL);
//	g_pWrapper->SetAPIKey(apiKey);
//	g_pWrapper->SetAPISecret(apiSecret);
//	g_pWrapper->GenerateToken(callback);	
//}
//
//
//
//const NDKUnity::SessionInfo* Unity_GetSessionInfo()
//{	
//	if (g_pWrapper == nullptr)
//		return nullptr;
//	return new NDKUnity::SessionInfo(g_pWrapper->GetSessionInfo());
//}
//
//void Unity_LoadScene(const char* strSceneName, const char* sessionState, NDKUnity::UnityCallback callback)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->LoadScene(strSceneName, sessionState, callback);
//}
//
//
//int Unity_GetAgentCount()
//{
//	if (g_pWrapper == nullptr)
//		return -1;
//	return static_cast<int>(g_pWrapper->GetAgentInfo().size());
//}
//
//const NDKUnity::AgentInfo* Unity_GetAgentInfo(int nIndex)
//{
//	if (g_pWrapper == nullptr || nIndex < 0 || nIndex > g_pWrapper->GetAgentInfo().size())
//		return nullptr;
//	return new NDKUnity::AgentInfo(g_pWrapper->GetAgentInfo()[nIndex]);
//}
//
//void Unity_StartSession()
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->StartSession();
//	Inworld::Log("Session Started");
//}
//
//void Unity_SetPacketCallback(NDKUnity::UnityPacketCallback pktCallBack, NDKUnity::PhonemeCallBack phonemeCallBack, NDKUnity::TriggerParamCallBack triggerParamCallBack)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->SetPacketCallBack(pktCallBack, phonemeCallBack, triggerParamCallBack);
//}
//
//void Unity_EndSession()
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->EndSession();
//}
//
//void Unity_ResumeClient()
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->ResumeClient();
//}
//
//void Unity_SendText(const char* agentID, const char* message)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->SendTextMessage(agentID, message);
//}
//
//void Unity_SendAudio(const char* agentID, const char* data)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->SendSoundMessage(agentID, NDKUnity::Base64ToString(data));
//}
//
//void Unity_SendTrigger(const char* agentID, const char* triggerName)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	const auto pResult = g_pWrapper->SendCustomEvent(agentID, triggerName, {});
//	if (pResult == nullptr)
//		return;
//	Inworld::Log("Send Trigger: %s. InteractionID: %s", triggerName, pResult->_PacketId._InteractionId.c_str());
//}
//void Unity_SendTriggerParam(const char* agentID, const char* triggerName, const char* param, const char* paramValue)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	const auto pResult = g_pWrapper->SendCustomEvent(agentID, triggerName, {{param, paramValue}});
//	if (pResult == nullptr)
//		return;
//	Inworld::Log("Send Trigger: %s. InteractionID: %s", triggerName, pResult->_PacketId._InteractionId.c_str());
//}
//
//void Unity_CancelResponse(const char* agentID, const char* interactionIDToCancel)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->CancelResponse(agentID, interactionIDToCancel, {});
//}
//
//void Unity_StartAudio(const char* agentID)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->StartAudioSession(agentID);
//}
//
//void Unity_StopAudio(const char* agentID)
//{
//	if (g_pWrapper == nullptr)
//		return;
//	g_pWrapper->StopAudioSession(agentID);
//}
//
//
