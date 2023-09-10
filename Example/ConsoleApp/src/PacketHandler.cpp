/**
 * Copyright  Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "PacketHandler.h"
#include "Utils/Log.h"

void NDKApp::PacketHandler::Visit(const Inworld::TextEvent& Event)
{
	Inworld::Log("{}: Text: {}", ARG_STR(Event._Routing._Source._Name), ARG_STR(Event.GetText()));
}

void NDKApp::PacketHandler::Visit(const Inworld::CustomEvent& Event)
{
	Inworld::Log("{}: Custom: {}", ARG_STR(Event._Routing._Source._Name), ARG_STR(Event.GetName()));
}

void NDKApp::PacketHandler::Visit(const Inworld::CustomGestureEvent& Event)
{
	Inworld::Log("{}: Custom gesture: {}", ARG_STR(Event._Routing._Source._Name), ARG_STR(Event.GetCustomGesture()));
}

void NDKApp::PacketHandler::Visit(const Inworld::CancelResponseEvent& Event)
{

}

void NDKApp::PacketHandler::Visit(const Inworld::EmotionEvent& Event)
{
	Inworld::Log("{}: Emotion: Behavior {}, Strengths {}", ARG_STR(Event._Routing._Source._Name), (int32_t)Event.GetEmotionalBehavior(), (int32_t)Event.GetStrength());
}

void NDKApp::PacketHandler::Visit(const Inworld::ControlEvent& Event)
{
	Inworld::Log("{}: Control: {}", ARG_STR(Event._Routing._Source._Name), (int32_t)Event.GetControlAction());
}

void NDKApp::PacketHandler::Visit(const Inworld::SilenceEvent& Event)
{
	Inworld::Log("{}: Silence: Duration {}", ARG_STR(Event._Routing._Source._Name), Event.GetDuration());
}

void NDKApp::PacketHandler::Visit(const Inworld::DataEvent& Event)
{
	Inworld::Log("{}: Data: Size {}", ARG_STR(Event._Routing._Source._Name), Event.GetDataChunk().size());
}
