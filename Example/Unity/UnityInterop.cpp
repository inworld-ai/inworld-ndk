#include "UnityInterop.h"

NDKUnity::Wrapper* Unity_InitWrapper()
{
	if (g_pWrapper == nullptr)
		g_pWrapper = new NDKUnity::Wrapper();
	return g_pWrapper;	
}

void Unity_SetLogger(Inworld::UnityLogCallback callback)
{
	if (g_pWrapper == nullptr)
		return;
	g_pWrapper->SetLoggerCallBack(callback);
	// Inworld::Log("Callback Initialized");
}
void Unity_Hello()
{
	Inworld::Log("Callback Initialized");
}
void Unity_DestroyWrapper()
{
	delete g_pWrapper;
	g_pWrapper = nullptr;
}