/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Log.h"

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

}

#ifdef INWORLD_NDK_LOG

void Inworld::ConvertToSpdFmt(std::string& fmt)
{
	for (int32_t i = 0; i < fmt.size(); i++)
	{
		if (fmt[i] == '%')
		{
			fmt[i] = '{';
			fmt[i + 1] = '}';
		}
	}
}

#elif INWORLD_NDK_LOG_UE

#define STARTING_BUFFER_SIZE		512


FString Inworld::PrintfImpl(const TCHAR* Fmt, ...)

{
	int32		BufferSize = STARTING_BUFFER_SIZE;
	TCHAR	StartingBuffer[STARTING_BUFFER_SIZE];
	TCHAR* Buffer = StartingBuffer;
	int32		Result = -1;

	// First try to print to a stack allocated location 
	GET_VARARGS_RESULT(Buffer, BufferSize, BufferSize - 1, Fmt, Fmt, Result);

	// If that fails, start allocating regular memory
	if (Result == -1)
	{
		Buffer = nullptr;
		while (Result == -1)
		{
			BufferSize *= 2;
			Buffer = (TCHAR*)FMemory::Realloc(Buffer, BufferSize * sizeof(TCHAR));
			GET_VARARGS_RESULT(Buffer, BufferSize, BufferSize - 1, Fmt, Fmt, Result);
		};
	}

	Buffer[Result] = TEXT('\0');

	FString ResultString(Buffer);

	if (BufferSize != STARTING_BUFFER_SIZE)
	{
		FMemory::Free(Buffer);
	}

	return ResultString;
}
#else

#endif


