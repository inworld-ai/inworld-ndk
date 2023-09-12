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
#include <string>
#include <thread>
#include <vector>
#include <fstream>

#include "Utils/Log.h"

//  500 -> 00:05.000
// 6000 -> 01:00.000
std::string to_timestamp(int64_t t) {
	int64_t sec = t / 100;
	int64_t msec = t - sec * 100;
	int64_t min = sec / 60;
	sec = sec - min * 60;

	char buf[32];
	snInworld::Log(buf, sizeof(buf), "%02d:%02d.%03d", (int)min, (int)sec, (int)msec);

	return std::string(buf);
}

// parameters
struct whisper_params {
	int32_t n_threads = std::min(4, (int32_t)std::thread::hardware_concurrency());
	int32_t step_ms = 3000;
	int32_t length_ms = 10000;
	int32_t keep_ms = 200;
	int32_t capture_id = -1;
	int32_t max_tokens = 32;
	int32_t audio_ctx = 0;

	float vad_thold = 0.6f;
	float freq_thold = 100.0f;

	bool speed_up = false;
	bool translate = false;
	bool no_fallback = false;
	bool print_special = false;
	bool no_context = true;
	bool no_timestamps = false;
	bool tinydiarize = false;

	std::string language = "en";
	std::string model = "models/ggml-base.en.bin";
	std::string fname_out;
};

void whisper_print_usage(int argc, char** argv, const whisper_params& params);

bool whisper_params_parse(int argc, char** argv, whisper_params& params) {
	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];

		if (arg == "-h" || arg == "--help") {
			whisper_print_usage(argc, argv, params);
			exit(0);
		}
		else if (arg == "-t" || arg == "--threads") { params.n_threads = std::stoi(argv[++i]); }
		else if (arg == "--step") { params.step_ms = std::stoi(argv[++i]); }
		else if (arg == "--length") { params.length_ms = std::stoi(argv[++i]); }
		else if (arg == "--keep") { params.keep_ms = std::stoi(argv[++i]); }
		else if (arg == "-c" || arg == "--capture") { params.capture_id = std::stoi(argv[++i]); }
		else if (arg == "-mt" || arg == "--max-tokens") { params.max_tokens = std::stoi(argv[++i]); }
		else if (arg == "-ac" || arg == "--audio-ctx") { params.audio_ctx = std::stoi(argv[++i]); }
		else if (arg == "-vth" || arg == "--vad-thold") { params.vad_thold = std::stof(argv[++i]); }
		else if (arg == "-fth" || arg == "--freq-thold") { params.freq_thold = std::stof(argv[++i]); }
		else if (arg == "-su" || arg == "--speed-up") { params.speed_up = true; }
		else if (arg == "-tr" || arg == "--translate") { params.translate = true; }
		else if (arg == "-nf" || arg == "--no-fallback") { params.no_fallback = true; }
		else if (arg == "-ps" || arg == "--print-special") { params.print_special = true; }
		else if (arg == "-kc" || arg == "--keep-context") { params.no_context = false; }
		else if (arg == "-l" || arg == "--language") { params.language = argv[++i]; }
		else if (arg == "-m" || arg == "--model") { params.model = argv[++i]; }
		else if (arg == "-f" || arg == "--file") { params.fname_out = argv[++i]; }
		else if (arg == "-tdrz" || arg == "--tinydiarize") { params.tinydiarize = true; }

		else {
			Inworld::LogError("error: unknown argument: %s", arg.c_str());
			whisper_print_usage(argc, argv, params);
			exit(0);
		}
	}

	return true;
}

void whisper_print_usage(int /*argc*/, char** argv, const whisper_params& params) {
	Inworld::LogError("");
	Inworld::LogError("usage: %s [options]", argv[0]);
	Inworld::LogError("");
	Inworld::LogError("options:");
	Inworld::LogError("  -h,       --help          [default] show this help message and exit");
	Inworld::LogError("  -t N,     --threads N     [%-7d] number of threads to use during computation", params.n_threads);
	Inworld::LogError("            --step N        [%-7d] audio step size in milliseconds", params.step_ms);
	Inworld::LogError("            --length N      [%-7d] audio length in milliseconds", params.length_ms);
	Inworld::LogError("            --keep N        [%-7d] audio to keep from previous step in ms", params.keep_ms);
	Inworld::LogError("  -c ID,    --capture ID    [%-7d] capture device ID", params.capture_id);
	Inworld::LogError("  -mt N,    --max-tokens N  [%-7d] maximum number of tokens per audio chunk", params.max_tokens);
	Inworld::LogError("  -ac N,    --audio-ctx N   [%-7d] audio context size (0 - all)", params.audio_ctx);
	Inworld::LogError("  -vth N,   --vad-thold N   [%-7.2f] voice activity detection threshold", params.vad_thold);
	Inworld::LogError("  -fth N,   --freq-thold N  [%-7.2f] high-pass frequency cutoff", params.freq_thold);
	Inworld::LogError("  -su,      --speed-up      [%-7s] speed up audio by x2 (reduced accuracy)", params.speed_up ? "true" : "false");
	Inworld::LogError("  -tr,      --translate     [%-7s] translate from source language to english", params.translate ? "true" : "false");
	Inworld::LogError("  -nf,      --no-fallback   [%-7s] do not use temperature fallback while decoding", params.no_fallback ? "true" : "false");
	Inworld::LogError("  -ps,      --print-special [%-7s] print special tokens", params.print_special ? "true" : "false");
	Inworld::LogError("  -kc,      --keep-context  [%-7s] keep context between audio chunks", params.no_context ? "false" : "true");
	Inworld::LogError("  -l LANG,  --language LANG [%-7s] spoken language", params.language.c_str());
	Inworld::LogError("  -m FNAME, --model FNAME   [%-7s] model path", params.model.c_str());
	Inworld::LogError("  -f FNAME, --file FNAME    [%-7s] text output file name", params.fname_out.c_str());
	Inworld::LogError("  -tdrz,     --tinydiarize  [%-7s] enable tinydiarize (requires a tdrz model)", params.tinydiarize ? "true" : "false");
	Inworld::LogError("");
}

void Inworld::RunnableSTT::Run()
{
	whisper_params params;
	params.keep_ms = std::min(params.keep_ms, params.step_ms);
	params.length_ms = std::max(params.length_ms, params.step_ms);

	const int n_samples_step = (1e-3 * params.step_ms) * WHISPER_SAMPLE_RATE;
	const int n_samples_len = (1e-3 * params.length_ms) * WHISPER_SAMPLE_RATE;
	const int n_samples_keep = (1e-3 * params.keep_ms) * WHISPER_SAMPLE_RATE;
	const int n_samples_30s = (1e-3 * 30000.0) * WHISPER_SAMPLE_RATE;

	const bool use_vad = n_samples_step <= 0; // sliding window mode uses VAD

	const int n_new_line = !use_vad ? std::max(1, params.length_ms / params.step_ms - 1) : 1; // number of steps to print new line

	params.no_timestamps = !use_vad;
	params.no_context |= use_vad;
	params.max_tokens = 0;

	// whisper init
	if (params.language != "auto" && whisper_lang_id(params.language.c_str()) == -1) {
		Inworld::LogError("error: unknown language '%s'", params.language.c_str());
		exit(0);
	}

	struct whisper_context* ctx = whisper_init_from_file(params.model.c_str());

	std::vector<float> pcmf32(n_samples_30s, 0.0f);
	std::vector<float> pcmf32_old;
	std::vector<float> pcmf32_new(n_samples_30s, 0.0f);

	std::vector<whisper_token> prompt_tokens;

	// print some info about the processing
	{
		if (!whisper_is_multilingual(ctx)) {
			if (params.language != "en" || params.translate) {
				params.language = "en";
				params.translate = false;
				Inworld::LogError("%s: WARNING: model is not multilingual, ignoring language and translation options", __func__);
			}
		}
		Inworld::LogError("%s: processing %d samples (step = %.1f sec / len = %.1f sec / keep = %.1f sec), %d threads, lang = %s, task = %s, timestamps = %d ...",
			__func__,
			n_samples_step,
			float(n_samples_step) / WHISPER_SAMPLE_RATE,
			float(n_samples_len) / WHISPER_SAMPLE_RATE,
			float(n_samples_keep) / WHISPER_SAMPLE_RATE,
			params.n_threads,
			params.language.c_str(),
			params.translate ? "translate" : "transcribe",
			params.no_timestamps ? 0 : 1);

		if (!use_vad) {
			Inworld::Log("%s: n_new_line = %d, no_context = %d", __func__, n_new_line, params.no_context);
		}
		else {
			Inworld::Log("%s: using VAD, will transcribe on speech activity", __func__);
		}
	}

	int n_iter = 0;

	bool is_running = true;

	auto t_last = std::chrono::high_resolution_clock::now();
	const auto t_start = t_last;

	// main audio loop
	while (is_running) {
		// handle Ctrl + C
		is_running = sdl_poll_events();

		if (!is_running) {
			break;
		}

		// process new audio

		if (!use_vad) {
			while (true) {
				audio.get(params.step_ms, pcmf32_new);

				if ((int)pcmf32_new.size() > 2 * n_samples_step) {
					Inworld::LogWarning("%s: WARNING: cannot process audio fast enough, dropping audio ...", __func__);
					audio.clear();
					continue;
				}

				if ((int)pcmf32_new.size() >= n_samples_step) {
					audio.clear();
					break;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			const int n_samples_new = pcmf32_new.size();

			// take up to params.length_ms audio from previous iteration
			const int n_samples_take = std::min((int)pcmf32_old.size(), std::max(0, n_samples_keep + n_samples_len - n_samples_new));

			//Inworld::Log("processing: take = %d, new = %d, old = %d", n_samples_take, n_samples_new, (int) pcmf32_old.size());

			pcmf32.resize(n_samples_new + n_samples_take);

			for (int i = 0; i < n_samples_take; i++) {
				pcmf32[i] = pcmf32_old[pcmf32_old.size() - n_samples_take + i];
			}

			memcpy(pcmf32.data() + n_samples_take, pcmf32_new.data(), n_samples_new * sizeof(float));

			pcmf32_old = pcmf32;
		}
		else {
			const auto t_now = std::chrono::high_resolution_clock::now();
			const auto t_diff = std::chrono::duration_cast<std::chrono::milliseconds>(t_now - t_last).count();

			if (t_diff < 2000) {
				//std::this_thread::sleep_for(std::chrono::milliseconds(100));

				continue;
			}

			audio.get(2000, pcmf32_new);

			if (::vad_simple(pcmf32_new, WHISPER_SAMPLE_RATE, 1000, params.vad_thold, params.freq_thold, false)) {
				audio.get(params.length_ms, pcmf32);
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));

				continue;
			}

			t_last = t_now;
		}

		// run the inference
		{
			whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

			wparams.print_progress = false;
			wparams.print_special = params.print_special;
			wparams.print_realtime = false;
			wparams.print_timestamps = !params.no_timestamps;
			wparams.translate = params.translate;
			wparams.single_segment = !use_vad;
			wparams.max_tokens = params.max_tokens;
			wparams.language = params.language.c_str();
			wparams.n_threads = params.n_threads;

			wparams.audio_ctx = params.audio_ctx;
			wparams.speed_up = params.speed_up;

			wparams.tdrz_enable = params.tinydiarize; // [TDRZ]

			// disable temperature fallback
			//wparams.temperature_inc  = -1.0f;
			wparams.temperature_inc = params.no_fallback ? 0.0f : wparams.temperature_inc;

			wparams.prompt_tokens = params.no_context ? nullptr : prompt_tokens.data();
			wparams.prompt_n_tokens = params.no_context ? 0 : prompt_tokens.size();

			if (whisper_full(ctx, wparams, pcmf32.data(), pcmf32.size()) != 0) {
				Inworld::LogError("%s: failed to process audio", argv[0]);
				return 6;
			}

			// print result;
			{
				if (!use_vad) {
					Inworld::Log("\33[2K\r");

					// print long empty line to clear the previous line
					Inworld::Log("%s", std::string(100, ' ').c_str());

					Inworld::Log("\33[2K\r");
				}
				else {
					const int64_t t1 = (t_last - t_start).count() / 1000000;
					const int64_t t0 = std::max(0.0, t1 - pcmf32.size() * 1000.0 / WHISPER_SAMPLE_RATE);

					Inworld::Log("");
					Inworld::Log("### Transcription %d START | t0 = %d ms | t1 = %d ms", n_iter, (int)t0, (int)t1);
					Inworld::Log("");
				}

				const int n_segments = whisper_full_n_segments(ctx);
				for (int i = 0; i < n_segments; ++i) {
					const char* text = whisper_full_get_segment_text(ctx, i);

					if (params.no_timestamps) {
						Inworld::Log("%s", text);
					}
					else {
						const int64_t t0 = whisper_full_get_segment_t0(ctx, i);
						const int64_t t1 = whisper_full_get_segment_t1(ctx, i);

						std::string output = "[" + to_timestamp(t0) + " --> " + to_timestamp(t1) + "]  " + text;

						if (whisper_full_get_segment_speaker_turn_next(ctx, i)) {
							output += " [SPEAKER_TURN]";
						}

						output += "";

						Inworld::Log("%s", output.c_str());
					}
				}

				if (use_vad) {
					Inworld::Log("### Transcription %d END", n_iter);
				}
			}

			++n_iter;

			if (!use_vad && (n_iter % n_new_line) == 0) {
				Inworld::Log("");

				// keep part of the audio for next iteration to try to mitigate word boundary issues
				pcmf32_old = std::vector<float>(pcmf32.end() - n_samples_keep, pcmf32.end());

				// Add tokens of the last full length segment as the prompt
				if (!params.no_context) {
					prompt_tokens.clear();

					const int n_segments = whisper_full_n_segments(ctx);
					for (int i = 0; i < n_segments; ++i) {
						const int token_count = whisper_full_n_tokens(ctx, i);
						for (int j = 0; j < token_count; ++j) {
							prompt_tokens.push_back(whisper_full_get_token_id(ctx, i, j));
						}
					}
				}
			}
			fflush(stdout);
		}
	}

	audio.pause();

	whisper_print_timings(ctx);
	whisper_free(ctx);
}


void Inworld::RunnableSTT::Deinitialize()
{

}

#endif // INWORLD_WHISPERCPP
