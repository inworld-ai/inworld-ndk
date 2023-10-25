/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#ifdef INWORLD_STT_LOCAL && INWORLD_WHISPERCPP

#include "STT.h"

//#include "common-sdl.h"
#include "examples/common.h"
#include "whisper.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
#include <fstream>

#include "Utils/Log.h"



class WhisperProcessor
{
public:
	WhisperProcessor(const std::string& ModelPath)
	{
		params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
		params.print_progress = false;
		params.print_special = false;
		params.print_realtime = false;
		params.print_timestamps = false;
		params.translate = false;
		params.single_segment = true;
		params.max_tokens = 32;
		params.language = "en";
		params.audio_ctx = 0;
		params.speed_up = false;
		params.tdrz_enable = false;

		ctx = whisper_init_from_file(ModelPath.c_str());
		if (!ctx)
		{
			Inworld::LogError("WhisperProcessor couldn't load model '%s'", ModelPath.c_str());
		}
	}

	~WhisperProcessor()
	{
		delete ctx;
	}

	void ProcessAudio(const std::vector<float>& pcmf32_new, std::vector<std::string>& text_chunks)
	{
		if (!ctx)
		{
			Inworld::LogError("WhisperProcessor couldn't process audio, ctx is nullptr");
			return;
		}

		// streaming code
		/*constexpr int32_t length_ms = 5000;
		constexpr int32_t keep_ms = 200;
		constexpr int32_t n_samples_len = (1e-3 * length_ms) * WHISPER_SAMPLE_RATE;
		constexpr int32_t n_samples_keep = (1e-3 * keep_ms) * WHISPER_SAMPLE_RATE;

		std::vector<float> pcmf32;
		const int32_t n_samples_new = pcmf32_new.size();

		// take up to length_ms audio from previous iteration
		const int32_t n_samples_take =  std::min((int32_t)pcmf32_old.size(), std::max(0, n_samples_keep + n_samples_len - n_samples_new));

		pcmf32.resize(n_samples_new + n_samples_take);

		for (int32_t i = 0; i < n_samples_take; i++) {
			pcmf32[i] = pcmf32_old[pcmf32_old.size() - n_samples_take + i];
		}

		memcpy(pcmf32.data() + n_samples_take, pcmf32_new.data(), n_samples_new * sizeof(float));

		pcmf32_old = pcmf32;*/

		if (whisper_full(ctx, params, pcmf32_new.data(), pcmf32_new.size()) != 0) {
			Inworld::LogError("WhisperProcessor couldn't process audio");
			return;
		}

		const int n_segments = whisper_full_n_segments(ctx);
		for (int i = 0; i < n_segments; ++i) {
			const char* text = whisper_full_get_segment_text(ctx, i);
			text_chunks.emplace_back(text);
		}
	}

private:
	whisper_context* ctx = nullptr;
	whisper_full_params params;

	std::vector<float> pcmf32_old;
}; 

static std::unique_ptr<WhisperProcessor> Processor;

void Inworld::STT_Initialize(const std::string& ModelPath)
{
	Processor = std::make_unique<WhisperProcessor>(ModelPath);
}

void Inworld::STT_Terminate()
{
	Processor.reset();
}

void Inworld::RunnableSTT::Run()
{
	while (!_IsDone && Processor)
	{
		std::shared_ptr<AudioDataEvent> Event;
		if (_InEvents.PopFront(Event))
		{
			const std::string& Data = Event->GetDataChunk();
			Inworld::Log("Inworld::RunnableSTT::Run. Data size: %d", Data.size());

			std::vector<float> pcmf32_new(Data.size() / 2, 0.f);
			Inworld::Log("Inworld::RunnableSTT::Run. pcmf32_new size: %d", pcmf32_new.size());
			for (int i = 0, j = 0; i < pcmf32_new.size(); i++, j += 2)
			{
				const float fVal = (float)(int16_t)*(uint16_t*)(Data.data() + j);
				pcmf32_new[i] = fVal / 32767.f;
				//Inworld::Log("Text chunk: %f", pcmf32_new[i]);
			}

			std::vector<std::string> TextChunks;
			//Processor->ProcessAudio(pcmf32_new, TextChunks);
			Processor->ProcessAudio(Event->_SdlData, TextChunks);
			Inworld::Log("Inworld::RunnableSTT::Run. audio processed. Text chunks: %d", TextChunks.size());
			for (auto& Chunk : TextChunks)
			{
				// checking for a blank audio or noise
				if (Chunk.find('[') != std::string::npos)
				{
					continue;
				}

				Inworld::Log("Inworld::RunnableSTT::Run. Chunk: %s", Chunk.c_str());
				std::shared_ptr<TextEvent> E = std::make_shared<TextEvent>(Chunk, Event->GetRouting());
				_OutCallback(E);
			}
		}
	}
}

#endif // INWORLD_WHISPERCPP
