#pragma once
#include "UnityWrapper.h"

#if __cplusplus
extern "C"
{
#endif
	
#define DLL_EXPORT _declspec(dllexport)

#pragma region LifeCycle	
	inline NDKUnity::CUnityWrapper* g_pWrapper = nullptr;
	DLL_EXPORT NDKUnity::CUnityWrapper* Unity_InitWrapper();
	DLL_EXPORT void Unity_DestroyWrapper();
#pragma endregion  LifeCycle

#pragma region Setter
	DLL_EXPORT void Unity_SetLogger(Inworld::UnityLogCallback callback);
	DLL_EXPORT void Unity_SetClientRequest(const char* strClientID, const char* strClientVersion);	
	DLL_EXPORT void Unity_SetUserRequest(const char* strPlayerName, const char* strPlayerID);	
	DLL_EXPORT void Unity_AddUserProfile(const char* strProfileID, const char* strProfileValue);
	DLL_EXPORT void Unity_SetCapabilities(NDKUnity::Capabilities capabilities);
#pragma endregion Setter
	
#pragma region AsyncFunc
	DLL_EXPORT void Unity_GetAccessToken(const char* strServerURL, const char* strAPIKey, const char* strAPISecret, NDKUnity::UnityCallback callback);
	DLL_EXPORT void Unity_LoadScene(const char* strSceneName, NDKUnity::UnityCallback callback);
#pragma endregion AsyncFunc

	
	DLL_EXPORT void Unity_GetUserSettings();

	DLL_EXPORT void Unity_GetCapabilities();	

	DLL_EXPORT void Unity_Hello();

	
#pragma region Getter
	DLL_EXPORT NDKUnity::SessionInfo Unity_GetSessionInfo();
	DLL_EXPORT int Unity_GetAgentCount();
	DLL_EXPORT NDKUnity::AgentInfo Unity_GetAgentInfo(int nIndex);  // NOLINT(clang-diagnostic-return-type-c-linkage)
#pragma endregion Getter	

#if __cplusplus
}
#endif