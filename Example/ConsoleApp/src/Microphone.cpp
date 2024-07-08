/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Microphone.h"

#include "Log.h"

bool Inworld::Mic::StartCapture()
{
    if (bStarted)
    {
        Inworld::LogError("Capture already started!");
        return false;
    }
    
    std::vector<unsigned int> deviceIds = _Audio.getDeviceIds();
    if (deviceIds.size() < 1) {
        Inworld::LogError("No audio devices found!");
        return false;
    }
 
    RtAudio::StreamParameters parameters;
    parameters.deviceId = _Audio.getDefaultInputDevice();
    parameters.nChannels = 1;
    parameters.firstChannel = 0;
    constexpr uint32_t sampleRate = 16000;
    uint32_t bufferFrames = 256; // 256 sample frames
    
    if (_Audio.openStream(nullptr, &parameters, RTAUDIO_SINT16,
    sampleRate, &bufferFrames, RecordCallback)) 
    {
        Inworld::LogError(_Audio.getErrorText());
        return false;
    }
 
    if (_Audio.startStream()) {
        Inworld::LogError(_Audio.getErrorText());
        return false;
    }

    bStarted = true;

    return true;
}

bool Inworld::Mic::StopCapture()
{
    if (!bStarted)
    {
        Inworld::LogError("Capture not started!");
        return false;
    }
    
    if (_Audio.isStreamRunning())
        _Audio.stopStream();
 
     if (_Audio.isStreamOpen())
        _Audio.closeStream();

    bStarted = false;

    return true;
}

bool Inworld::Mic::GetAudio(std::string& Data)
{
    std::deque<std::string> Chunks;
    _AudioChuncks.PopAll(Chunks);
    for (const auto& Chunk : Chunks)
        Data.append(Chunk);

    return !Data.empty();
}

int Inworld::Mic::RecordCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime,
    RtAudioStreamStatus status, void *userData)
{
    if (status)
    {
        Inworld::LogWarning("Stream overflow detected!");
    }
 
    _AudioChuncks.PushBack(std::string((char*)inputBuffer, nBufferFrames * sizeof(int16_t)));

    //Inworld::Log("RecordCallback: %d, %.2f", nBufferFrames, streamTime);

    return 0;
}
