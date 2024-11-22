/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once
#include "UnityWrapper.h"
#include "Utils/Log.h"

#if __cplusplus
extern "C"
{
#endif


	
#pragma region LifeCycle	
	inline NDKApp::NDKWrapper* g_pNDKWrapper = nullptr;
	INWORLD_EXPORT void* NDK_InitWrapper();
	INWORLD_EXPORT void NDK_DestroyWrapper();
#pragma endregion  LifeCycle

#pragma region Setter
	INWORLD_EXPORT void NDK_SetLogger(Inworld::LogCallback info, Inworld::LogCallback warning, Inworld::LogCallback error);
	INWORLD_EXPORT void NDK_Init();
	INWORLD_EXPORT void NDK_Hello();
	//INWORLD_EXPORT void Unity_SetPublicWorkspace(const char* strPublicWorkspace);
	//INWORLD_EXPORT void Unity_SetPacketCallback(NDKUnity::UnityPacketCallback pktCallBack, NDKUnity::PhonemeCallBack phonemeCallBack, NDKUnity::TriggerParamCallBack triggerParamCallBack);
	//INWORLD_EXPORT void Unity_SetClientRequest(const char* strClientID, const char* strClientVersion, const char* strClientDescription);
	//INWORLD_EXPORT void Unity_SetUserRequest(const char* strPlayerName, const char* strPlayerID);
	//INWORLD_EXPORT void Unity_AddUserProfile(const char* strProfileID, const char* strProfileValue);
	//INWORLD_EXPORT void Unity_SetCapabilities(const NDKUnity::Capabilities& capabilities);
#pragma endregion Setter
	
#pragma region Inworld Status
	//INWORLD_EXPORT void Unity_GetAccessToken(const char* serverURL, const char* strAPIKey, const char* strAPISecret, NDKUnity::UnityCallback callback);
	//INWORLD_EXPORT void Unity_SaveSessionState(NDKUnity::UnityCallback callback);
	//INWORLD_EXPORT void Unity_LoadScene(const char* strSceneName, const char* sessionState, NDKUnity::UnityCallback callback);
	//INWORLD_EXPORT void Unity_StartSession();
	//INWORLD_EXPORT void Unity_EndSession();
#pragma endregion Inworld Status

#pragma region Send Data
	//INWORLD_EXPORT void Unity_SendText(const char* agentID, const char* message);
	//INWORLD_EXPORT void Unity_SendAudio(const char* agentID, const char* data);
	//INWORLD_EXPORT void Unity_SendTrigger(const char* agentID, const char* triggerName);
	//INWORLD_EXPORT void Unity_SendTriggerParam(const char* agentID, const char* triggerName, const char* param, const char* paramValue);
	//INWORLD_EXPORT void Unity_CancelResponse(const char* agentID, const char* interactionIDToCancel);
	//INWORLD_EXPORT void Unity_StartAudio(const char* agentID);
	//INWORLD_EXPORT void Unity_StopAudio(const char* agentID);
#pragma endregion Send Data

	// INWORLD_EXPORT void Unity_Hello();
	
#pragma region Getter
	//INWORLD_EXPORT const NDKUnity::SessionInfo* Unity_GetSessionInfo();
	//INWORLD_EXPORT int Unity_GetAgentCount();
	//INWORLD_EXPORT const NDKUnity::AgentInfo* Unity_GetAgentInfo(int nIndex);
#pragma endregion Getter	

#if __cplusplus
}
#endif