/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Microphone.h"
#include <iostream>
#include <cstdlib>

#include "Log.h"

bool Inworld::Mic::StartCapture(std::string& Error)
{
    std::vector<unsigned int> deviceIds = _Audio.getDeviceIds();
    if ( deviceIds.size() < 1 ) {
        Inworld::LogError("No audio devices found!");
        return false;
    }
 
    RtAudio::StreamParameters parameters;
    parameters.deviceId = _Audio.getDefaultInputDevice();
    parameters.nChannels = 1;
    parameters.firstChannel = 0;
    unsigned int sampleRate = 16000;
    unsigned int bufferFrames = 256; // 256 sample frames
    
    if ( _Audio.openStream( NULL, &parameters, RTAUDIO_SINT16,
    sampleRate, &bufferFrames, RecordCallback ) ) 
    {
        Inworld::LogError(_Audio.getErrorText());
        return false;
    }
 
    if ( _Audio.startStream() ) {
        Inworld::LogError(_Audio.getErrorText());
        return false;
    }
}

bool Inworld::Mic::StopCapture(std::string& Error)
{
    if ( _Audio.isStreamRunning() )
        _Audio.stopStream();
 
     if ( _Audio.isStreamOpen() )
        _Audio.closeStream();
}

int Inworld::Mic::RecordCallback(void* outputBuffer, void* inputBuffer, unsigned nBufferFrames, double streamTime,
    RtAudioStreamStatus status, void* userData)
{
    if ( status )
        std::cout << "Stream overflow detected!" << std::endl;
 
    // Do something with the data in the "inputBuffer" buffer.
    _AudioChuncks.PushBack(std::string((char*)inputBuffer, nBufferFrames));
    return 0;
}
