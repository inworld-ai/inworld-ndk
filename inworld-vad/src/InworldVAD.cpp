/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "InworldVAD.h"

#include "onnxruntime_cxx_api.h"

#include <locale>
#include <codecvt>

Inworld::VAD::VAD(const std::string& Model)
    : Env(std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "VAD"))
{
    // Initialize Session options if needed
    Ort::SessionOptions SessionOptions;
    SessionOptions.SetIntraOpNumThreads(1);

    std::wstring_convert<std::codecvt_utf8<wchar_t>> Converter;
    std::wstring WModel = Converter.from_bytes(Model);
    
    // Load the VAD model
    Session = std::make_unique<Ort::Session>(Ort::Session(*Env, WModel.c_str(), SessionOptions));
}

float Inworld::VAD::ProcessAudioChunk(const std::string& AudioData)
{
    // Assuming AudioData contains raw PCM16 data
    std::vector<int16_t> AudioVector((int16_t*)AudioData.data(), (int16_t*)(AudioData.data() + AudioData.size()));

    // Prepare input tensor
    std::vector<int64_t> InputTensorShape = {1, static_cast<int64_t>(AudioVector.size())}; // Single batch
    Ort::MemoryInfo MemoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    const auto InputTensor = Ort::Value::CreateTensor<int16_t>(MemoryInfo, AudioVector.data(), AudioVector.size(), InputTensorShape.data(), InputTensorShape.size());

    // Specify model input and output
    const char* InputNames[] = {"input"};
    const char* OutputNames[] = {"output"};
    const Ort::Value InputTensors[] = {InputTensor};
        
    // Run the model
    auto OutputTensors = Session->Run(Ort::RunOptions{nullptr}, InputNames, InputTensors, 1, OutputNames, 1);

    // Get and return the output probability
    auto* OutputData = OutputTensors.front().GetTensorMutableData<float>();
    return OutputData[0];  // Probability of voice activity
}
