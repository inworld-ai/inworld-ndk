#include "ClientWrapper.h"


#include "spdlog/spdlog.h"

void (*NDKUnity::Wrapper::Log)(char* strMessage,int nSize);

NDKUnity::Wrapper* Unity_InitClient()
{
	return nullptr;
}

// void UnityLoggerCallback(void (*LogCallBack)(char* strMessage, int nSize))
// {
// 	NDKUnity::ClientWrapper::Log = LogCallBack;
// 	char acLogStr[512] = { 0 };
// 	sprintf_s(acLogStr, "%s","Hello CALLBACK!!");
// 	NDKUnity::ClientWrapper::Log(acLogStr, strlen(acLogStr));
// }