/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include "Define.h"
#include <cstddef>

#if __cplusplus
extern "C"
{
#endif

namespace Inworld
{
    INWORLD_EXPORT void STT_Initialize(const char* model);
    INWORLD_EXPORT void STT_Terminate();
    INWORLD_EXPORT float STT_Process(const float* audioData, size_t size);
    INWORLD_EXPORT void STT_ResetState();
}

#if __cplusplus
}
#endif
