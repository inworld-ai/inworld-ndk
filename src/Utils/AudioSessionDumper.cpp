/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "AudioSessionDumper.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <google/protobuf/stubs/common.h>

#include "SharedQueue.h"

#if INWORLD_AUDIO_DUMP

// Type aliases
using uint8 = google::protobuf::uint8;
using uint16 = google::protobuf::uint16;
using uint32 = google::protobuf::uint32;
using int32 = google::protobuf::int32;

constexpr uint32 gSamplesPerSec = 16000;

struct FWavHeader 
{
	uint8 RIFF[4] = { 'R', 'I', 'F', 'F' };
	uint32 ChunkSize;
	uint8 WAVE[4] = { 'W', 'A', 'V', 'E' };
	uint8 fmt[4] = { 'f', 'm', 't', ' ' };
	uint32 Subchunk1Size = 16;
	uint16 AudioFormat = 1;
	uint16 NumOfChan = 1;
	uint32 SamplesPerSec = gSamplesPerSec;
	uint32 bytesPerSec = gSamplesPerSec * 2;
	uint16 blockAlign = 2;
	uint16 bitsPerSample = 16;
	uint8 Subchunk2ID[4] = { 'd', 'a', 't', 'a' };
	uint32 Subchunk2Size;
};

void AudioSessionDumper::OnMessage(const std::string& Msg)
{
	std::ofstream OutStream(FileName, std::ios::binary | std::ios_base::app);
	OutStream.write((const char*)Msg.data(), Msg.size());
	OutStream.close();
}

void AudioSessionDumper::OnSessionStart(const std::string& InFileName)
{
	FileName = InFileName;

	std::ofstream OutStream(FileName, std::ios::binary);
	OutStream.clear();
	OutStream.close();
}

void AudioSessionDumper::OnSessionStop()
{
	std::ifstream InStream(FileName, std::ios::binary | std::ios::ate);
	const int32 WaveSize = InStream.tellg();
	std::vector<uint8> WaveData(WaveSize);

	InStream.seekg(0, std::ios::beg);
	InStream.read(reinterpret_cast<char*>(WaveData.data()), WaveSize);
	InStream.close();

	FWavHeader WavStruct;
	WavStruct.ChunkSize = sizeof(FWavHeader) - 8;
	WavStruct.Subchunk2Size = WaveSize;

	std::ofstream OutStream(FileName, std::ios::binary);
	OutStream.write(reinterpret_cast<const char*>(&WavStruct), sizeof(WavStruct));
	OutStream.write(reinterpret_cast<const char*>(WaveData.data()), WaveSize);
	OutStream.close();
}
#endif