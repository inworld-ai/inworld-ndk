#include "UnityPacketHandler.h"
#include "Utils/Log.h"

// TODO(Yan): Send those packets back to C#.
void NDKUnity::CUnityPacketHandler::Visit(const Inworld::TextEvent& Event)
{
	// Inworld::Log("{}: Text: {}", Event._Routing._Source._Name, Event.GetText());
	if (m_TextCallBack)
		m_TextCallBack(TextPacket(Event));
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::AudioDataEvent& Event)
{
	PacketVisitor::Visit(Event);
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::CustomEvent& Event)
{
	Inworld::Log("{}: Custom: {}", Event._Routing._Source._Name, Event.GetName());
}

void NDKUnity::CUnityPacketHandler::SetTextCallback(const TextCallBack& callBack)
{
	m_TextCallBack = callBack;
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
