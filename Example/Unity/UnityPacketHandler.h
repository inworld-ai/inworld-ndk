#pragma once
#include "Packets.h"

namespace NDKUnity
{
	class CUnityPacketHandler : public Inworld::PacketVisitor
	{
	public:
		virtual void Visit(const Inworld::TextEvent& Event) override;
		virtual void Visit(const Inworld::DataEvent& Event) override;
		virtual void Visit(const Inworld::SilenceEvent& Event) override;
		virtual void Visit(const Inworld::ControlEvent& Event) override;
		virtual void Visit(const Inworld::EmotionEvent& Event) override;
		virtual void Visit(const Inworld::CancelResponseEvent& Event) override;
		virtual void Visit(const Inworld::CustomGestureEvent& Event) override;
		virtual void Visit(const Inworld::CustomEvent& Event) override;
	};
}

