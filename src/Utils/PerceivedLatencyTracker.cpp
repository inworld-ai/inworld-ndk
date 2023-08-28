/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "PerceivedLatencyTracker.h"
#include "Log.h"

void Inworld::PerceivedLatencyTracker::HandlePacket(std::shared_ptr<Inworld::Packet> Packet)
{
	if (Packet)
	{
		Packet->Accept(*this);
	}
}

void Inworld::PerceivedLatencyTracker::Visit(const Inworld::TextEvent& Event)
{
	if (Event._Routing._Source._Type == InworldPakets::Actor_Type_PLAYER &&
		Event.IsFinal())
	{
		const auto& Interaction = Event._PacketId._InteractionId;
		if (_InteractionTimeMap.find(Interaction) != _InteractionTimeMap.end())
		{
			Inworld::LogError("PerceivedLatencyTracker visit TextEvent. Final player text already exists, Interaction: %s", ARG_STR(Interaction));
		}
		else
		{
			_InteractionTimeMap.emplace(Interaction, Event._Timestamp);
		}
	}
}

void Inworld::PerceivedLatencyTracker::Visit(const Inworld::AudioDataEvent& Event)
{
	const auto& Interaction = Event._PacketId._InteractionId;
	const auto It = _InteractionTimeMap.find(Interaction);
	if (It == _InteractionTimeMap.end())
	{
		Inworld::LogError("PerceivedLatencyTracker visit AudioDataEvent. No final player text exists, Interaction: %s", ARG_STR(Interaction));
	}
	else
	{
		const auto Duration = Event._Timestamp - It->second;
		_InteractionTimeMap.erase(It);

		const int32_t Ms = std::chrono::duration_cast<std::chrono::milliseconds>(Duration).count();
		if (Ms > _WarningLatency)
		{
			Inworld::LogWarning("PerceivedLatencyTracker. Latency is '%d', Interaction: %s", Ms, ARG_STR(Interaction));
		}

		if (_Callback)
		{
			_Callback(Interaction, Ms);
		}
	}
}

void Inworld::PerceivedLatencyTracker::Visit(const Inworld::ControlEvent& Event)
{

}
