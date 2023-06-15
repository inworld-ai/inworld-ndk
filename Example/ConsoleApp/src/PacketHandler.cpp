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
	Inworld::Log("{}: Text: {}", Event._Routing._Source._Name, Event.GetText());
}

void NDKApp::PacketHandler::Visit(const Inworld::CustomEvent& Event)
{
	Inworld::Log("{}: Custom: {}", Event._Routing._Source._Name, Event.GetName());
}

void NDKApp::PacketHandler::Visit(const Inworld::CustomGestureEvent& Event)
{
	Inworld::Log("{}: Custom gesture: {}", Event._Routing._Source._Name, Event.GetCustomGesture());
}

void NDKApp::PacketHandler::Visit(const Inworld::CancelResponseEvent& Event)
{

}

void NDKApp::PacketHandler::Visit(const Inworld::EmotionEvent& Event)
{
	Inworld::Log("{}: Emotion: Behavior {}, Strengths {}", Event._Routing._Source._Name, (int32_t)Event.GetEmotionalBehavior(), (int32_t)Event.GetStrength());
}

void NDKApp::PacketHandler::Visit(const Inworld::ControlEvent& Event)
{
	Inworld::Log("{}: Control: {}", Event._Routing._Source._Name, (int32_t)Event.GetControlAction());
}

void NDKApp::PacketHandler::Visit(const Inworld::SilenceEvent& Event)
{
	Inworld::Log("{}: Silence: Duration {}", Event._Routing._Source._Name, Event.GetDuration());
}

void NDKApp::PacketHandler::Visit(const Inworld::DataEvent& Event)
{
	Inworld::Log("{}: Data: Size {}", Event._Routing._Source._Name, Event.GetDataChunk().size());
}
