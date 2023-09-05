#pragma once
#include "UnityWrapper.h"

#if __cplusplus
extern "C"
{
#endif
	
#define DLL_EXPORT _declspec(dllexport)

	inline NDKUnity::CUnityWrapper* g_pWrapper = nullptr;

	DLL_EXPORT NDKUnity::CUnityWrapper* Unity_InitWrapper();

	DLL_EXPORT void Unity_GetAccessToken(const char* strServerURL, const char* strAPIKey, const char* strAPISecret);

	DLL_EXPORT void Unity_SetLogger(Inworld::UnityLogCallback callback);

	DLL_EXPORT void Unity_Hello();

	DLL_EXPORT void Unity_DestroyWrapper();

#if __cplusplus
}
#endif