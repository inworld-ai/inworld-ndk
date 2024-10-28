/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "PerceivedLatencyTracker.h"
#include "Log.h"
#include "ai/inworld/packets/packets.pb.h"

void Inworld::PerceivedLatencyTracker::Visit(const Inworld::TextEvent& Event)
{
	if (Event._Routing._Source._Type == InworldPackets::Actor_Type_PLAYER && Event.IsFinal())
	{
		const auto& Interaction = Event._PacketId._InteractionId;
		if (_InteractionTimeMap.find(Interaction) != _InteractionTimeMap.end())
		{
			Inworld::LogWarning("PerceivedLatencyTracker visit TextEvent. Final player text already exists, Interaction: %s", Interaction.c_str());
		}
		else
		{
			if (Event.GetSourceType() == InworldPackets::TextEvent_SourceType::TextEvent_SourceType_TYPED_IN)
			{
				_PerceivedEnd = { std::chrono::system_clock::now(), PerceivedFromType::TypedIn };
			}
			else
			{
				if (!_PerceivedEnd.has_value())
				{
					_PerceivedEnd = { std::chrono::system_clock::now(), PerceivedFromType::TextToSpeech };
				}
			}
			_InteractionTimeMap.emplace(Interaction, *_PerceivedEnd);
			_PerceivedEnd = {};
		}
	}
	else if (!_bTrackAudioReplies && Event._Routing._Source._Type == InworldPackets::Actor_Type_AGENT)
	{
		ReceiveReply(Event._PacketId._InteractionId);
	}
}

void Inworld::PerceivedLatencyTracker::Visit(const Inworld::CustomEvent& Event)
{
	if (Event._Routing._Source._Type == InworldPackets::Actor_Type_PLAYER)
	{
		const auto& Interaction = Event._PacketId._InteractionId;
		const std::pair<TimeStamp, PerceivedFromType> Instant = { std::chrono::system_clock::now(), PerceivedFromType::Trigger };
		_InteractionTimeMap.emplace(Interaction, Instant);
	}
}

void Inworld::PerceivedLatencyTracker::Visit(const Inworld::VADEvent& Event)
{
	if (!Event.IsVoiceDetected())
	{
		if (!_PerceivedEnd.has_value() || (*_PerceivedEnd).second == PerceivedFromType::VoiceActivity)
		{
			_PerceivedEnd = { std::chrono::system_clock::now(), PerceivedFromType::VoiceActivity };
		}
	}
}

void Inworld::PerceivedLatencyTracker::Visit(const Inworld::AudioDataEvent& Event)
{
	if (_bTrackAudioReplies && Event._Routing._Source._Type == InworldPackets::Actor_Type_AGENT)
	{
		ReceiveReply(Event._PacketId._InteractionId);
	}
}

void Inworld::PerceivedLatencyTracker::ReceiveReply(const std::string& InteractionId)
{
	const auto It = _InteractionTimeMap.find(InteractionId);
	if (It != _InteractionTimeMap.end())
	{
		const auto Duration = std::chrono::system_clock::now() - It->second.first;
		const int32_t Ms = std::chrono::duration_cast<std::chrono::milliseconds>(Duration).count();

		const PerceivedFromType Type = It->second.second;
		const std::string TypeStr = std::unordered_map<PerceivedFromType, std::string>{
				{ PerceivedFromType::VoiceActivity, "Voice Activity"},
				{ PerceivedFromType::AudioSession, "Audio Session" },
				{ PerceivedFromType::TypedIn, "Typed In" },
				{ PerceivedFromType::Trigger, "Trigger" },
		}[Type];
		
		Inworld::Log("PerceivedLatencyTracker (%s): %d ms, Interaction: %s", TypeStr.c_str(), Ms, InteractionId.c_str());

	    if (_Callback)
	    {
	        _Callback(InteractionId, Ms, Type);
	    }

		_InteractionTimeMap.erase(It);
	}
}

void Inworld::PerceivedLatencyTracker::Visit(const Inworld::ControlEvent& Event)
{
	if (Event.GetControlAction() == InworldPackets::ControlEvent_Action_INTERACTION_END)
	{
		const auto& Interaction = Event._PacketId._InteractionId;
		const auto It = _InteractionTimeMap.find(Interaction);
		if (It != _InteractionTimeMap.end())
		{
			Inworld::LogWarning("PerceivedLatencyTracker visit ControlEvent INTERACTION_END. Text timestamp is still in the map, Interaction: %s", Interaction.c_str());
			_InteractionTimeMap.erase(It);
		}
	}
	if (Event.GetControlAction() == InworldPackets::ControlEvent_Action_AUDIO_SESSION_END)
	{
		if (!_PerceivedEnd.has_value())
		{
			_PerceivedEnd = { std::chrono::system_clock::now(), PerceivedFromType::AudioSession };
		}
	}
}
