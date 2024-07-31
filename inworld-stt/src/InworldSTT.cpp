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
    const std::string model_path("opus-mt-en-de");
    const ctranslate2::models::ModelLoader model_loader(model_path);

    ctranslate2::Translator translator(model_loader);

    const std::vector<std::vector<std::string>> batch = {{"▁How", "▁are", "▁you", "▁doing", "?", "</s>"}};
    const auto translation = translator.translate_batch(batch);

    std::string result;
    for (const auto& token : translation[0].output())
        result += token + ' ';

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
