/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "PerceivedLatencyTracker.h"
#include "Log.h"
#include "ai/inworld/packets/packets.pb.h"

void Inworld::PerceivedLatencyTracker::HandlePacket(std::shared_ptr<Inworld::Packet> Packet)
{
	if (Packet)
	{
		Packet->Accept(*this);
	}
}

void Inworld::PerceivedLatencyTracker::Visit(const Inworld::TextEvent& Event)
{
	if (Event._Routing._Source._Type == InworldPackets::Actor_Type_PLAYER && Event.IsFinal())
	{
		const auto& Interaction = Event._PacketId._InteractionId;
		if (_InteractionTimeMap.find(Interaction) != _InteractionTimeMap.end())
		{
			Inworld::LogError("PerceivedLatencyTracker visit TextEvent. Final player text already exists, Interaction: %s", Interaction.c_str());
		}
		else
		{
			_InteractionTimeMap.emplace(Interaction, std::chrono::system_clock::now());
		}
	}
	else if (Event._Routing._Source._Type == InworldPackets::Actor_Type_AGENT && !_TrackAudioReplies)
	{
		VisitReply(Event);
	}
}

void Inworld::PerceivedLatencyTracker::Visit(const Inworld::AudioDataEvent& Event)
{
	if (_TrackAudioReplies)
	{
		VisitReply(Event);
	}
}

void Inworld::PerceivedLatencyTracker::VisitReply(const Inworld::Packet& Event)
{
	const auto& Interaction = Event._PacketId._InteractionId;
	const auto It = _InteractionTimeMap.find(Interaction);
	if (It != _InteractionTimeMap.end())
	{
		const auto Duration = std::chrono::system_clock::now() - It->second;
		_InteractionTimeMap.erase(It);

		const int32_t Ms = std::chrono::duration_cast<std::chrono::milliseconds>(Duration).count();
		Inworld::Log("PerceivedLatencyTracker. Latency is %dms, Interaction: %s", Ms, Interaction.c_str());

		if (_Callback)
		{
			_Callback(Interaction, Ms);
		}
	}
}

void Inworld::PerceivedLatencyTracker::Visit(const Inworld::ControlEvent& Event)
{
	if (Event.GetControlAction() != InworldPackets::ControlEvent_Action_INTERACTION_END)
	{
		return;
	}

	const auto& Interaction = Event._PacketId._InteractionId;
	const auto It = _InteractionTimeMap.find(Interaction);
	if (It != _InteractionTimeMap.end())
	{
		Inworld::LogError("PerceivedLatencyTracker visit ControlEvent INTERACTION_END. Text timestamp is still in the map, Interaction: %s", Interaction.c_str());
		_InteractionTimeMap.erase(It);
	}
}
