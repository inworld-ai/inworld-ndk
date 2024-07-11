/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */


#include "InworldSTT.h"

#include <ctranslate2/translator.h>

namespace ct = ctranslate2;

void Inworld::STT_Initialize(const char* model)
{
    ct::Translator translator("", ct::Device::CPU);
    std::vector<std::string> source;
    auto results = translator.translate_batch({source});
}

void Inworld::STT_Terminate()
{
}

float Inworld::STT_Process(const float* audioData, size_t size)
{
    return 0.f;
}

void Inworld::STT_ResetState()
{
}
