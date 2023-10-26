/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once
#include "UnityWrapper.h"

#if __cplusplus
extern "C"
{
#endif

#if _WIN32	
#define DLL_EXPORT _declspec(dllexport)
#else
#define DLL_EXPORT
#endif
	
#pragma region LifeCycle	
	inline NDKUnity::CUnityWrapper* g_pWrapper = nullptr;
	DLL_EXPORT NDKUnity::CUnityWrapper* Unity_InitWrapper();
	DLL_EXPORT void Unity_DestroyWrapper();
#pragma endregion  LifeCycle

#pragma region Setter
	DLL_EXPORT void Unity_SetLogger(Inworld::LoggerCallBack callback);
	DLL_EXPORT void Unity_SetPacketCallback(NDKUnity::UnityPacketCallback pktCallBack, NDKUnity::PhonemeCallBack phonemeCallBack, NDKUnity::TriggerParamCallBack triggerParamCallBack);
	DLL_EXPORT void Unity_SetClientRequest(const char* strClientID, const char* strClientVersion);	
	DLL_EXPORT void Unity_SetUserRequest(const char* strPlayerName, const char* strPlayerID);	
	DLL_EXPORT void Unity_AddUserProfile(const char* strProfileID, const char* strProfileValue);
	DLL_EXPORT void Unity_SetCapabilities(const NDKUnity::Capabilities& capabilities);
#pragma endregion Setter
	
#pragma region Inworld Status
	DLL_EXPORT void Unity_GetAccessToken(const char* strServerURL, const char* strAPIKey, const char* strAPISecret, NDKUnity::UnityCallback callback);
	DLL_EXPORT void Unity_LoadScene(const char* strSceneName, NDKUnity::UnityCallback callback);	
	DLL_EXPORT void Unity_StartSession();
	DLL_EXPORT void Unity_EndSession();
#pragma endregion Inworld Status

#pragma region Send Data
	DLL_EXPORT void Unity_SendText(const char* agentID, const char* message);
	DLL_EXPORT void Unity_SendAudio(const char* agentID, const char* data);
	DLL_EXPORT void Unity_SendTrigger(const char* agentID, const char* triggerName);
	DLL_EXPORT void Unity_SendTriggerParam(const char* agentID, const char* triggerName, const char* param, const char* paramValue);
	DLL_EXPORT void Unity_CancelResponse(const char* agentID, const char* interactionIDToCancel);
	DLL_EXPORT void Unity_StartAudio(const char* agentID);
	DLL_EXPORT void Unity_StopAudio(const char* agentID);
#pragma endregion Send Data

	DLL_EXPORT void Unity_Hello();
	
#pragma region Getter
	DLL_EXPORT const NDKUnity::SessionInfo* Unity_GetSessionInfo();
	DLL_EXPORT int Unity_GetAgentCount();
	DLL_EXPORT const NDKUnity::AgentInfo* Unity_GetAgentInfo(int nIndex);  
#pragma endregion Getter	

#if __cplusplus
}
#endif