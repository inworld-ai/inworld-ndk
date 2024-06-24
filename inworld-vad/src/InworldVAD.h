/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include "Define.h"
#include <vector>
#include <string>

#if __cplusplus
extern "C"
{
#endif

namespace Inworld
{
    INWORLD_EXPORT void VAD_Initialize(const char* model);
    INWORLD_EXPORT void VAD_Terminate();
    INWORLD_EXPORT float VAD_Process(const float* audioData, size_t size);
    INWORLD_EXPORT void VAD_ResetState();
}

#if __cplusplus
}
#endif
