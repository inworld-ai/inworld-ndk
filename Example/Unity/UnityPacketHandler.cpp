#include "UnityPacketHandler.h"
#include "Utils/Log.h"

// TODO(Yan): Send those packets back to C#.
void NDKUnity::CUnityPacketHandler::Visit(const Inworld::TextEvent& Event)
{
	Inworld::Log("{}: Text: {}", Event._Routing._Source._Name, Event.GetText());
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::CustomEvent& Event)
{
	Inworld::Log("{}: Custom: {}", Event._Routing._Source._Name, Event.GetName());
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::CustomGestureEvent& Event)
{
	Inworld::Log("{}: Custom gesture: {}", Event._Routing._Source._Name, Event.GetCustomGesture());
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::CancelResponseEvent& Event)
{

}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::EmotionEvent& Event)
{
	Inworld::Log("{}: Emotion: Behavior {}, Strengths {}", Event._Routing._Source._Name, (int32_t)Event.GetEmotionalBehavior(), (int32_t)Event.GetStrength());
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::ControlEvent& Event)
{
	Inworld::Log("{}: Control: {}", Event._Routing._Source._Name, (int32_t)Event.GetControlAction());
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::SilenceEvent& Event)
{
	Inworld::Log("{}: Silence: Duration {}", Event._Routing._Source._Name, Event.GetDuration());
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::DataEvent& Event)
{
	Inworld::Log("{}: Data: Size {}", Event._Routing._Source._Name, Event.GetDataChunk().size());
}