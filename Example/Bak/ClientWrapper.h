#pragma once
#include <string>
#include "Client.h"

#if __cplusplus
extern "C"
{
#endif

#ifdef INWORLD_UNITY
#define DLL_EXPORT _declspec(dllexport)
#endif

namespace NDKUnity
{
	class Wrapper
	{	
		public:
			static void (*Log)(char* strMessage,int nSize);
	};
}

	DLL_EXPORT NDKUnity::Wrapper* Unity_InitClient();

	// DLL_EXPORT void UnityLoggerCallback(void (*LogCallBack)(char* strMessage, int nSize));

	DLL_EXPORT inline void (*LogCallBack)(char* strMessage,int nSize);
	
#if __cplusplus
}
#endif
