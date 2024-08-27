/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once
#include <string>
#include <functional>
#include <memory>
#include <chrono>
#include "Packets.h"
#include "Define.h"

namespace Inworld {

	using TimeStamp = std::chrono::system_clock::time_point;

	class INWORLD_EXPORT PerceivedLatencyTracker : public Inworld::PacketVisitor
	{
	public:
		enum class PerceivedFromType : uint8_t
		{
			VoiceActivity,
			TextToSpeech,
			AudioSession,
			TypedIn,
			Trigger,
			MAX,
		};

		using ReportCallback = std::function<void(const std::string& InteractionId, uint32_t LatencyMs, PerceivedFromType PerceivedFrom)>;

		virtual ~PerceivedLatencyTracker() = default;

		void SetCallback(ReportCallback Cb) { _Callback = Cb; }

		virtual void Visit(const Inworld::TextEvent& Event) override;
		virtual void Visit(const Inworld::CustomEvent& Event) override;
		virtual void Visit(const Inworld::VADEvent& Event) override;
		virtual void Visit(const Inworld::AudioDataEvent& Event) override;
		virtual void Visit(const Inworld::ControlEvent& Event) override;

		void TrackAudioReplies(bool bVal) { _bTrackAudioReplies = bVal; }


	private:
		void ReceiveReply(const std::string& InteractionId);

		std::unordered_map<std::string, std::pair<TimeStamp, PerceivedFromType>> _InteractionTimeMap;
		std::optional<std::pair<TimeStamp, PerceivedFromType>> _PerceivedEnd;

		ReportCallback _Callback = nullptr;
		bool _bTrackAudioReplies = false;
	};

}
