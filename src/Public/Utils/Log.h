/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <string>
#include <memory>
#include <functional>

#include "Define.h"

namespace Inworld
{
#ifdef INWORLD_LOG_CALLBACK
	using LogCallback = void(*)(const char* message);
	INWORLD_EXPORT void SetLogCallbacks(LogCallback info, LogCallback warning, LogCallback error);
	INWORLD_EXPORT void ClearLogCallbacks();
	extern std::function<void(const char* message)> g_LogInfoCallback;
	extern std::function<void(const char* message)> g_LogWarnCallback;
	extern std::function<void(const char* message)> g_LogErrorCallback;
#endif

	template<typename... Args>
	std::string Format(std::string fmt, Args &&... args)
	{
#ifdef INWORLD_LOG
		size_t size = std::snprintf(nullptr, 0, fmt.c_str(), std::forward<Args>(args)...) + 1;
		std::unique_ptr<char[]> buf(new char[size]);
		std::snprintf(buf.get(), size, fmt.c_str(), std::forward<Args>(args)...);
		return std::string(buf.get(), buf.get() + size - 1);
#else
        return {};
#endif
	}
	
	void INWORLD_EXPORT Log(const std::string& message);

	template<typename... Args>
	void Log(std::string fmt, Args &&... args)
	{
#ifdef INWORLD_LOG
		Log(Format(fmt, args...));
#endif
	}

	void INWORLD_EXPORT LogWarning(const std::string& message);

	template<typename... Args>
	void LogWarning(std::string fmt, Args &&... args)
	{
#ifdef INWORLD_LOG
		LogWarning(Format(fmt, args...));
#endif
	}
	
	void INWORLD_EXPORT LogError(const std::string& message);

	template<typename... Args>
	void LogError(std::string fmt, Args &&... args)
	{
#ifdef INWORLD_LOG
		LogError(Format(fmt, args...));
#endif
	}

}
