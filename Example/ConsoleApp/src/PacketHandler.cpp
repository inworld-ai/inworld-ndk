/**
 * Copyright  Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "PacketHandler.h"
#include "Utils/Log.h"
#include "ai/inworld/packets/packets.pb.h"

void NDKApp::PacketHandler::Visit(const Inworld::TextEvent& Event)
{
	std::string Targets = (Event._Routing._Target._Type == InworldPackets::Actor_Type_PLAYER) ? "Player" : 
		(Event._Routing._Target._Name.empty() ? "" : GetGivenName(Event._Routing._Target._Name));
	for (auto& Target : Event._Routing._Targets)
	{
		if (!Targets.empty())
			Targets += ", ";
		Targets += (Target._Type == InworldPackets::Actor_Type_PLAYER) ? "Player" : GetGivenName(Target._Name);
	}

	Inworld::Log("%s to %s: Text: %s", ARG_STR(GetGivenName(Event._Routing._Source._Name)), ARG_STR(Targets), ARG_STR(Event.GetText()));
}

void NDKApp::PacketHandler::Visit(const Inworld::CustomEvent& Event)
{
	Inworld::Log("%s: Custom: %s", ARG_STR(GetGivenName(Event._Routing._Source._Name)), ARG_STR(Event.GetName()));
}

void NDKApp::PacketHandler::Visit(const Inworld::CustomGestureEvent& Event)
{
	//Inworld::Log("%s: Custom gesture: %s", ARG_STR(GetGivenName(Event._Routing._Source._Name)), ARG_STR(Event.GetCustomGesture()));
}

void NDKApp::PacketHandler::Visit(const Inworld::CancelResponseEvent& Event)
{

}

void NDKApp::PacketHandler::Visit(const Inworld::EmotionEvent& Event)
{
	//Inworld::Log("%s: Emotion: Behavior %d, Strengths %d", ARG_STR(GetGivenName(Event._Routing._Source._Name)), (int32_t)Event.GetEmotionalBehavior(), (int32_t)Event.GetStrength());
}

void NDKApp::PacketHandler::Visit(const Inworld::ControlEvent& Event)
{
	Inworld::Log("%s: Control: %d %s", ARG_STR(GetGivenName(Event._Routing._Source._Name)), (int32_t)Event.GetControlAction(), ARG_STR(Event.GetDescription()));
}

void NDKApp::PacketHandler::Visit(const Inworld::SilenceEvent& Event)
{
	Inworld::Log("%s: Silence: Duration %f", ARG_STR(GetGivenName(Event._Routing._Source._Name)), Event.GetDuration());
}

void NDKApp::PacketHandler::Visit(const Inworld::DataEvent& Event)
{
	Inworld::Log("%s: Data: Size %d", ARG_STR(GetGivenName(Event._Routing._Source._Name)), Event.GetDataChunk().size());
}

std::string NDKApp::PacketHandler::GetGivenName(const std::string& AgentId) const
{
	auto it = std::find_if(_AgentInfos.begin(), _AgentInfos.end(), [&AgentId](const auto& Info) { return Info.AgentId == AgentId; });
	if (it == _AgentInfos.end())
	{
		return "InvalidName";
	}
	return it->GivenName;
}
