/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <stdint.h>

#if defined(WEBRTC_WIN)
#define DLL_EXPORT _declspec(dllexport)
#elif defined(WEBRTC_MAC)
#define DLL_EXPORT __attribute__((visibility("default")))
#elif defined(WEBRTC_ANDROID)
#define DLL_EXPORT __attribute__((visibility("default")))
#else
#define DLL_EXPORT
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	DLL_EXPORT void* WebRtcAec3_Create(int32_t sample_rate_hz);
	DLL_EXPORT void WebRtcAec3_Free(void *handle);
	DLL_EXPORT int32_t WebRtcAec3_BufferFarend(void* handle, const int16_t* farend);
	DLL_EXPORT int32_t WebRtcAec3_Process(void* handle, const int16_t* nearend, int16_t* out);

#ifdef __cplusplus
}
#endif

