/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */


#include "InworldSTT.h"

#include <ctranslate2/translator.h>
#include <ctranslate2/models/whisper.h>

void Inworld::STT_Initialize(const char* model)
{
    ctranslate2::Translator translator("ende_ctranslate2/", ctranslate2::Device::CPU);

    const std::vector<std::vector<std::string>> batch = {{"▁H", "ello", "▁world", "!"}};
    const std::vector<ctranslate2::TranslationResult> results = translator.translate_batch(batch);

    std::string result;
    for (const auto& token : results[0].output())
        result += token + " ";

    auto res = result;
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
