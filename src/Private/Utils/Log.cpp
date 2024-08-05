/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Log.h"

#ifdef INWORLD_LOG

		#ifdef INWORLD_LOG_SPD
			#include "spdlog/spdlog.h"
		#endif

		#ifdef ANDROID
			#include <android/log.h>
		#endif

#endif

#ifdef INWORLD_LOG_CALLBACK
std::function<void(const char* message)> Inworld::g_LogInfoCallback = nullptr;
std::function<void(const char* message)> Inworld::g_LogWarnCallback = nullptr;
std::function<void(const char* message)> Inworld::g_LogErrorCallback = nullptr;

void Inworld::SetLogCallbacks(LogCallback info, LogCallback warn, LogCallback error)
{
    g_LogInfoCallback = info;
	g_LogWarnCallback = warn;
	g_LogErrorCallback = error;
}

void Inworld::ClearLogCallbacks()
{
	g_LogInfoCallback = nullptr;
	g_LogWarnCallback = nullptr;
	g_LogErrorCallback = nullptr;
}
#endif

void Inworld::Log(const std::string& message)
{
#ifdef INWORLD_LOG
	#if  defined(INWORLD_LOG_CALLBACK)
		if (g_LogInfoCallback) g_LogInfoCallback(message.c_str());
	#elif defined(ANDROID)
		__android_log_print(ANDROID_LOG_INFO, "inworld-ndk", "%s", message.c_str());
	#elif defined(INWORLD_LOG_SPD)
		spdlog::info(message);
	#else
		std::cout << message << std::endl;
	#endif
#endif
}

void Inworld::LogWarning(const std::string& message)
{
#ifdef INWORLD_LOG
	#if defined(INWORLD_LOG_CALLBACK)
		if (g_LogWarnCallback) g_LogWarnCallback(message.c_str());
	#elif defined(ANDROID)
		__android_log_print(ANDROID_LOG_WARN, "inworld-ndk", "%s", message.c_str());
  #elif defined(INWORLD_LOG_SPD)
    spdlog::warn(message);
  #else
    std::cout << message << std::endl;
	#endif
#endif
}

void Inworld::LogError(const std::string& message)
{
#ifdef INWORLD_LOG
	#if defined(INWORLD_LOG_CALLBACK)
		if (g_LogErrorCallback) g_LogErrorCallback(message.c_str());
	#elif defined(ANDROID)
		__android_log_print(ANDROID_LOG_ERROR, "inworld-ndk", "%s", message.c_str());
  #elif defined(INWORLD_LOG_SPD)
    spdlog::error(message);
  #else
    std::cout << message << std::endl;
	#endif
#endif
}
