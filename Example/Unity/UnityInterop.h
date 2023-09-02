#pragma once
#include "UnityWrapper.h"

#if __cplusplus
extern "C"
{
#endif
	
#define DLL_EXPORT _declspec(dllexport)

	NDKUnity::Wrapper* g_pWrapper = nullptr;

	DLL_EXPORT NDKUnity::Wrapper* Unity_InitWrapper();

	DLL_EXPORT void Unity_SetLogger(Inworld::UnityLogCallback callback);

	DLL_EXPORT void Unity_Hello();

	DLL_EXPORT void Unity_DestroyWrapper();

#if __cplusplus
}
#endif