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

	using PerceivedLatencyCallback = std::function<void(const std::string& InteractionId, uint32_t LatancyMs)>;
	using TimeStamp = std::chrono::system_clock::time_point;

	class INWORLD_EXPORT PerceivedLatencyTracker : Inworld::PacketVisitor
	{
	public:
		virtual ~PerceivedLatencyTracker() = default;

		void SetCallback(PerceivedLatencyCallback Cb) { _Callback = Cb; }
		void ClearCallback() { _Callback = nullptr; }
		bool HasCallback() const { return _Callback != nullptr; }

		void HandlePacket(std::shared_ptr<Inworld::Packet> Packet);

		virtual void Visit(const Inworld::TextEvent& Event) override;
		virtual void Visit(const Inworld::AudioDataEvent& Event) override;
		virtual void Visit(const Inworld::ControlEvent& Event) override;

		void TrackAudioReplies(bool bVal) { _TrackAudioReplies = bVal; }

	private:
		void VisitReply(const Inworld::Packet& Event);

		std::unordered_map<std::string, TimeStamp> _InteractionTimeMap;
		PerceivedLatencyCallback _Callback = nullptr;
		bool _TrackAudioReplies = false;
	};

}
