/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <functional>
#include <string>
#include "RtAudio.h"
#include "SharedQueue.h"

namespace Inworld
{
	class Mic
	{
	public:
	    static bool StartCapture(std::string& Error);
	    static bool StopCapture(std::string& Error);

	private:
        static int RecordCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
            double streamTime, RtAudioStreamStatus status, void *userData);
	    
	    static RtAudio _Audio;
	    static SharedQueue<std::string> _AudioChuncks;
	};
}
