#pragma once
#include "UnityWrapper.h"
#include "Data/UnityNDKInteropData.h"

#if __cplusplus
extern "C"
{
#endif
	
#define DLL_EXPORT _declspec(dllexport)


	
	inline NDKUnity::CUnityWrapper* g_pWrapper = nullptr;

	DLL_EXPORT NDKUnity::CUnityWrapper* Unity_InitWrapper();

	DLL_EXPORT void Unity_SetClientRequest(const char* strClientID, const char* strClientVersion);
	
	DLL_EXPORT void Unity_SetUserRequest(const char* strPlayerName, const char* strPlayerID);
	
	DLL_EXPORT void Unity_AddUserProfile(const char* strProfileID, const char* strProfileValue);

	DLL_EXPORT void Unity_SetCapabilities(NDKUnity::Capabilities capabilities);

	DLL_EXPORT void Unity_GetUserSettings();

	DLL_EXPORT void Unity_GetCapabilities();

	DLL_EXPORT void Unity_GetAccessToken(const char* strServerURL, const char* strAPIKey, const char* strAPISecret);	

	DLL_EXPORT void Unity_SetLogger(Inworld::UnityLogCallback callback);

	DLL_EXPORT void Unity_Hello();

	DLL_EXPORT void Unity_DestroyWrapper();

#if __cplusplus
}
#endif