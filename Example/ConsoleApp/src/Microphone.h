/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <string>
#include "RtAudio.h"
#include "SharedQueue.h"

namespace Inworld
{
	class Mic
	{
	public:
	    static bool StartCapture();
	    static bool StopCapture();
	    static bool IsCapturing() { return bStarted; }

	    static void GetAudio(std::deque<std::string>& Chunks) { _AudioChuncks.PopAll(Chunks); }
	    static bool GetAudio(std::string& Data);

	private:
        static int RecordCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
            double streamTime, RtAudioStreamStatus status, void *userData);
	    
	    static inline RtAudio _Audio{RtAudio::WINDOWS_DS};
	    static inline SharedQueue<std::string> _AudioChuncks;
	    static inline bool bStarted = false;
	};
}
