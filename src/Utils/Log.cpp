/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Log.h"

#if INWORLD_LOG_CUSTOM
namespace Inworld
{
	std::unique_ptr<Logger> g_Logger = nullptr;
	void LogClearLogger() { g_Logger = nullptr; }
}
#elif INWORLD_LOG_SPD
	#include "spdlog/spdlog.h"
#elif ANDROID
	#include <android/log.h>
#endif

namespace Inworld
{
	std::string g_SessionId = "Unknown";

	void LogSetSessionId(const std::string Id)
	{
		g_SessionId = Id;
	}

	void LogClearSessionId()
	{
		g_SessionId = "Unknown";
	}

#ifdef INWORLD_UNITY

	void LogSetUnityLogCallback(void(*callback)(const char* message, int severity))
	{
		UnityLoggerCallback = callback;
	}
#endif

}

void Inworld::Log(const std::string& message)
{
#ifdef INWORLD_LOG
	#ifdef INWORLD_LOG_CUSTOM
		if (g_Logger) g_Logger->Log(message);
    #elif INWORLD_LOG_SPD
        spdlog::info(message);
	#elif ANDROID
		__android_log_print(ANDROID_LOG_INFO, "InworldNDK", "%s", message.c_str());
    #else
        std::cout << message << std::endl;
	#endif
#endif
}

void Inworld::LogWarning(const std::string& message)
{
#ifdef INWORLD_LOG
	#ifdef INWORLD_LOG_CUSTOM
		if (g_Logger) g_Logger->LogWarning(message);
    #elif INWORLD_LOG_SPD
        spdlog::warn(message);
	#elif ANDROID
		__android_log_print(ANDROID_LOG_WARN, "InworldNDK", "%s", message.c_str());
    #else
        std::cout << message << std::endl;
	#endif
#endif
}

void Inworld::LogError(const std::string& message)
{
#ifdef INWORLD_LOG
	const std::string error = VFormat("%s (SessionId: %s)", ARG_STR(message), ARG_STR(g_SessionId));
	#ifdef INWORLD_LOG_CUSTOM
	if (g_Logger) g_Logger->LogError(error);
    #elif INWORLD_LOG_SPD
		spdlog::error(error);
    #elif ANDROID
        __android_log_print(ANDROID_LOG_ERROR, "InworldNDK", "%s", error.c_str());
    #else
        std::cout << error << std::endl;
	#endif
#endif
}

