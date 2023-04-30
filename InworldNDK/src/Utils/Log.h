/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once


#ifdef INWORLD_NDK_LOG
#include "spdlog/spdlog.h"
#include <string>
#elif INWORLD_NDK_LOG_UE
#include "CoreMinimal.h"
#include "InworldAINdkModule.h"
#include "Runtime/Launch/Resources/Version.h"
#if ENGINE_MAJOR_VERSION > 4
#include <string_view>
namespace Inworld { using LogFormatType = std::string_view; }
#else
#include <string>
namespace Inworld { using LogFormatType = std::string; }
#endif
#else
#include <string>
#endif

namespace Inworld
{
	INWORLDAINDK_API extern std::string g_SessionId;

	INWORLDAINDK_API void LogSetSessionId(const std::string Id);
	INWORLDAINDK_API void LogClearSessionId();

#ifdef INWORLD_NDK_LOG

	void ConvertToSpdFmt(std::string& fmt);

	template<typename... Args>
	void Log(std::string fmt, Args &&... args)
	{
		ConvertToSpdFmt(fmt);
		spdlog::info(std::vformat(fmt, std::make_format_args(args...)));
	}
	
	template<typename... Args>
	void LogError(std::string fmt, Args &&... args)
	{
		ConvertToSpdFmt(fmt);
		const char* message = std::vformat(fmt, std::make_format_args(args...)).c_str();
		spdlog::error("{} (SessionId: {})", message, g_SessionId.c_str());
	}

#define ARG_STR(str) str.c_str()
#define ARG_CHAR(str) str

#elif INWORLD_NDK_LOG_UE

	// copy of FString::PrintfImpl to avoid static asserts
	INWORLDAINDK_API FString VARARGS PrintfImpl(const TCHAR* Fmt, ...);

	template<typename... Args>
	void Log(LogFormatType fmt, Args... args)
	{
		FString TcharFmt = PrintfImpl(UTF8_TO_TCHAR(fmt.data()), args...);
		UE_LOG(LogInworld, Log, TEXT("%s"), *TcharFmt);
	}

	template<typename... Args>
	void LogError(LogFormatType fmt, Args... args)
	{
		FString TcharFmt = PrintfImpl(UTF8_TO_TCHAR(fmt.data()), args...);
		UE_LOG(LogInworld, Error, TEXT("%s (SessionId: %s)"), *TcharFmt, UTF8_TO_TCHAR(g_SessionId.c_str()));
	}

#define ARG_STR(str) UTF8_TO_TCHAR(str.c_str())
#define ARG_CHAR(str) UTF8_TO_TCHAR(str)

#else
	template<typename... Args>
	void Log(std::string fmt, Args &&... args)
	{

	}

	template<typename... Args>
	void LogError(std::string fmt, Args &&... args)
	{

	}

#define ARG_STR(str) str
#define ARG_CHAR(str) str

#endif

}
