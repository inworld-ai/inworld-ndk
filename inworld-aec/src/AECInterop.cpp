/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "AECInterop.h"
#include "AECHandle.h"

#include <iostream>
	
webrtc::AECHandle* g_pAECHandle = nullptr;

void* WebRtcAec3_Create(int32_t sample_rate_hz)
{
	g_pAECHandle = new webrtc::AECHandle();
	return g_pAECHandle;
}

void WebRtcAec3_Free(void *handle)
{
	if (g_pAECHandle)
	{
		delete g_pAECHandle;
		g_pAECHandle = nullptr;
	}
}
int32_t WebRtcAec3_BufferFarend(void* handle, const int16_t* farend)
{
	// In case handle is not g_pAECHandle.
	
	if (g_pAECHandle)
	{
		g_pAECHandle->SampleOutput(farend);
		return 0;
	}
	return -1;
}
int32_t WebRtcAec3_Process(void* handle, const int16_t* nearend, int16_t* out)
{
	// In case handle is not g_pAECHandle.
	if (g_pAECHandle)
	{
		g_pAECHandle->SampleInput(nearend);
		memcpy(out, g_pAECHandle->GetFilteredData(), webrtc::AECHandle::kBytesPerFrame);
		return 0;
	}
	return -1;
}


